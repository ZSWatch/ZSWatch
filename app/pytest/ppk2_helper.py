"""
PPK2 Helper Module

This module provides utilities for Nordic PPK2 power profiler integration
in automated tests, including setup, measurement, and cleanup operations.
"""

import time
import logging
import statistics
from typing import List, Dict, Optional, Tuple

try:
    from ppk2_api.ppk2_api import PPK2_API as _PPK2_API
except ImportError:
    _PPK2_API = None

log = logging.getLogger(__name__)


def sample_offset(buf: bytes) -> int:
    """Byte offset where 4-byte samples start: the 6-bit counter (bits 18-23) counts up by one."""
    def score(offset):
        counters = [(int.from_bytes(buf[i:i + 4], "little") >> 18) & 0x3F
                    for i in range(offset, min(len(buf), offset + 4 * 256) - 3, 4)]
        return sum((b - a) % 64 == 1 for a, b in zip(counters, counters[1:]))
    return max(range(4), key=score)


class AlignedDecoder:
    """Decode PPK2 samples, realigning when the per-sample counter breaks (ppk2_api never does)."""

    def __init__(self, ppk):
        self.ppk = ppk
        self.pending = b""
        self.last_counter = None
        self.realigned = 0
        self.dropped = 0

    def feed(self, data: bytes) -> list:
        self.pending += data
        values = []
        if self.last_counter is None:
            if len(self.pending) < 64:
                return values
            self.pending = self.pending[sample_offset(self.pending):]
        i = 0
        while len(self.pending) - i >= 4:
            raw = int.from_bytes(self.pending[i:i + 4], "little")
            counter = (raw >> 18) & 0x3F
            if self.last_counter is not None and (counter - self.last_counter) % 64 != 1:
                rest = self.pending[i:]
                if len(rest) < 64:
                    break
                offset = sample_offset(rest)
                if offset:
                    self.realigned += 1
                    self.dropped += offset
                    self.pending = rest[offset:]
                    i = 0
                    self.last_counter = None
                    continue
            self.last_counter = counter
            values.append(self.ppk.get_adc_result(min((raw >> 14) & 0x7, 4), (raw & 0x3FFF) * 4) * 1e6)
            i += 4
        self.pending = self.pending[i:]
        return values


if _PPK2_API:

    class PPK2_API(_PPK2_API):
        """ppk2-api reads metadata with one fixed read, which can return it partially."""

        def _read_metadata(self):
            data = bytearray()
            deadline = time.monotonic() + 3
            while time.monotonic() < deadline:
                data.extend(self.ser.read(self.ser.in_waiting))
                if b"END\n" in data:
                    return data.decode()
                time.sleep(0.02)
            raise RuntimeError("Incomplete PPK2 metadata")

else:
    PPK2_API = None


class PPK2Manager:
    """Manager class for PPK2 operations"""

    def __init__(self, port: str, voltage_mv: int = 1800):
        """
        Initialize PPK2 manager

        Args:
            port: Serial port for PPK2 (e.g., '/dev/ttyACM0')
            voltage_mv: Output voltage in millivolts (default: 1800mV = 1.8V)
        """
        if not PPK2_API:
            raise ImportError("ppk2-api not available")

        self.port = port
        self.voltage_mv = voltage_mv
        self.ppk2: Optional[PPK2_API] = None
        self._modifiers_loaded = False

    def _load_modifiers(self, retries: int = 2, delay_s: float = 0.2) -> Optional[Dict]:
        """Load PPK2 calibration modifiers with a couple of retries."""
        if not self.ppk2:
            raise RuntimeError("PPK2 not initialized - call setup() first")

        modifiers: Optional[Dict] = None
        for attempt in range(1, retries + 1):
            try:
                modifiers = self.ppk2.get_modifiers()
                if modifiers:
                    self._modifiers_loaded = True
                    log.info("PPK2 modifiers loaded (attempt %d)", attempt)
                    break
            except Exception as exc:
                log.warning("PPK2 get_modifiers failed (attempt %d): %s", attempt, exc)
                # If the response was garbage, clear buffers before retrying.
                try:
                    self.ppk2.ser.reset_input_buffer()
                    self.ppk2.ser.reset_output_buffer()
                except Exception:
                    pass
                time.sleep(delay_s)

        return modifiers

    def ensure_calibrated(self):
        """Ensure calibration modifiers are loaded before measuring."""
        if not self._modifiers_loaded:
            self._load_modifiers()

    def setup(self) -> PPK2_API:
        """Setup and configure PPK2 in source mode"""
        log.info(f"Setting up PPK2 on {self.port}")
        self.ppk2 = PPK2_API(self.port)

        # Clear buffers
        time.sleep(0.5)
        self.ppk2.ser.reset_input_buffer()
        self.ppk2.ser.reset_output_buffer()

        # Initialize connection (ignore metadata errors)
        try:
            self.ppk2.get_modifiers()
        except Exception:
            pass  # Continue with default values

        # IMPORTANT: Set source voltage BEFORE starting measurement and setting mode
        self.ppk2.set_source_voltage(self.voltage_mv)

        # Configure in source mode
        self.ppk2.use_source_meter()
        self.ppk2.toggle_DUT_power("ON")

        time.sleep(0.1)

        log.info(f"PPK2 ready - providing {self.voltage_mv}mV power")
        return self.ppk2

    def flush(self):
        """Drop bytes still in flight from an earlier capture."""
        self.ppk2.stop_measuring()
        time.sleep(0.05)
        self.ppk2.ser.reset_input_buffer()
        self.ppk2.remainder = {"sequence": b"", "len": 0}

    def measure_current(
        self, duration_s: int, sample_interval_s: float = 0.001
    ) -> List[float]:
        """
        Measure current for specified duration

        Args:
            duration_s: Measurement duration in seconds
            sample_interval_s: Sampling interval in seconds

        Returns:
            List of current samples in microamps
        """
        if not self.ppk2:
            raise RuntimeError("PPK2 not initialized - call setup() first")

        # Make sure calibration modifiers are present
        self.ensure_calibrated()
        if not self._modifiers_loaded:
            log.warning("PPK2 modifiers are not loaded; readings may be uncalibrated")

        log.info(f"Measuring current for {duration_s} seconds...")

        # Start fresh measurement
        self.flush()
        self.ppk2.start_measuring()

        # Brief delay and clear any initial data
        time.sleep(0.1)
        decoder = AlignedDecoder(self.ppk2)
        decoder.feed(self.ppk2.get_data() or b"")  # Discard initial samples

        start_time = time.time()
        samples: List[float] = []

        try:
            while time.time() - start_time < duration_s:
                raw_data = self.ppk2.get_data()
                if raw_data:
                    samples.extend(decoder.feed(raw_data))

                time.sleep(sample_interval_s)
        finally:
            self.ppk2.stop_measuring()
        if decoder.realigned:
            log.warning("PPK2 sample stream realigned %d time(s)", decoder.realigned)

        log.info(f"Collected {len(samples)} current samples")
        return samples

    def power_cycle_device(self, off_duration_s: float = 1.0):
        """Power cycle the connected device"""
        if not self.ppk2:
            raise RuntimeError("PPK2 not initialized")

        log.info("Power cycling device...")
        self.ppk2.toggle_DUT_power("OFF")
        time.sleep(off_duration_s)
        self.ppk2.toggle_DUT_power("ON")
        time.sleep(1)  # Allow device to boot

    def cleanup(self):
        """Cleanup PPK2 connection and turn off power"""
        if self.ppk2:
            try:
                self.ppk2.stop_measuring()
            except:
                pass

            try:
                self.ppk2.toggle_DUT_power("OFF")
            except:
                pass

            log.info("PPK2 cleanup completed")


def analyze_current_samples(samples: List[float]) -> Dict[str, float]:
    """
    Analyze current measurement samples

    Args:
        samples: List of current samples in microamps

    Returns:
        Dictionary with statistical analysis in milliamps
    """
    if not samples:
        raise ValueError("No samples to analyze")

    # Convert to milliamps for easier reading
    samples_ma = [sample / 1000 for sample in samples]

    stats = {
        "average_ma": statistics.mean(samples_ma),
        "min_ma": min(samples_ma),
        "max_ma": max(samples_ma),
        "median_ma": statistics.median(samples_ma),
        "sample_count": len(samples_ma),
    }

    # Log results
    log.info("Current consumption statistics:")
    log.info(f"  Average: {stats['average_ma']:.3f} mA")
    log.info(f"  Minimum: {stats['min_ma']:.3f} mA")
    log.info(f"  Maximum: {stats['max_ma']:.3f} mA")
    log.info(f"  Median:  {stats['median_ma']:.3f} mA")
    log.info(f"  Samples: {stats['sample_count']}")

    return stats


def setup_ppk2(device_config: Dict) -> Optional[PPK2Manager]:
    """
    Setup PPK2 from device configuration

    Args:
        device_config: Device configuration dictionary

    Returns:
        PPK2Manager instance or None if not available
    """
    if "ppk2_port" not in device_config or not PPK2_API:
        return None

    try:
        manager = PPK2Manager(device_config["ppk2_port"], device_config.get("ppk2_voltage_mv", 4100))
        manager.setup()
        return manager
    except Exception as e:
        log.error(f"Failed to setup PPK2: {e}")
        return None
