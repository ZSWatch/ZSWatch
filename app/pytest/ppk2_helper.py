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
    from ppk2_api.ppk2_api import PPK2_API
except ImportError:
    PPK2_API = None

log = logging.getLogger(__name__)


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

        log.info(f"Measuring current for {duration_s} seconds...")

        # Start fresh measurement
        self.ppk2.start_measuring()

        # Brief delay and clear any initial data
        time.sleep(0.1)
        initial_data = self.ppk2.get_data()
        if initial_data:
            self.ppk2.get_samples(initial_data)  # Discard initial samples

        start_time = time.time()
        samples: List[float] = []

        try:
            while time.time() - start_time < duration_s:
                raw_data = self.ppk2.get_data()
                if raw_data != b"":
                    current_samples, _ = self.ppk2.get_samples(raw_data)
                    if current_samples:
                        samples.extend(current_samples)

                time.sleep(sample_interval_s)
        finally:
            self.ppk2.stop_measuring()

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
        manager = PPK2Manager(device_config["ppk2_port"])
        manager.setup()
        return manager
    except Exception as e:
        log.error(f"Failed to setup PPK2: {e}")
        return None
