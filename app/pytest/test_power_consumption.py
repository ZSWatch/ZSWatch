import time
import logging
import pytest
from ppk2_helper import analyze_current_samples
from power_logger import log_power_measurement

log = logging.getLogger()


def _measure_current(
    device_config,
    ppk2_instance,
    test_name: str,
    measurement_duration_s: int = 10,
    delay_s: int = 0,
):
    """
    Helper function to measure current consumption with common measurement logic.

    Args:
        device_config: Device configuration dictionary
        ppk2_instance: PPK2 measurement instance
        test_name: Name of the test for logging
        measurement_duration_s: Duration to measure current in seconds
        delay_s: Time to wait before measurement (for display sleep)

    Returns:
        dict: Current measurement statistics
    """
    log.info(f"Starting current measurement test: {test_name}")

    ppk2_instance.power_cycle_device()

    if delay_s > 0:
        time.sleep(delay_s)

    samples = ppk2_instance.measure_current(measurement_duration_s)

    if not samples:
        pytest.fail("No current samples collected during measurement")

    # Get statistics using helper function
    stats = analyze_current_samples(samples)

    # Store results for potential further analysis
    device_config["idle_current_stats"] = stats

    # Log power measurement for GitHub Actions
    log_power_measurement(
        test_name=test_name,
        average_ma=stats["average_ma"],
        min_ma=stats["min_ma"],
        max_ma=stats["max_ma"],
        duration_s=measurement_duration_s,
        sample_count=stats["sample_count"],
    )

    log.info(f"Current measurement test completed successfully: {test_name}")
    return stats


@pytest.mark.asyncio
@pytest.mark.ppk2
async def test_current_display_on_active(device_config, ppk2_instance):
    """
    Test to measure current consumption while the display is active (immediately after boot).

    This test:
    1. Power cycles the device using PPK2
    2. Immediately measures current for 10 seconds while display is on
    3. Analyzes and reports current consumption statistics
    """
    stats = _measure_current(
        device_config,
        ppk2_instance,
        test_name="current_display_on_active",
        measurement_duration_s=10,
    )

    # Sanity checks specific to display-on state
    # assert stats["average_ma"] > 0, "Average current should be positive"
    # assert stats["average_ma"] > 5, f"Display-on current seems too low: {stats['average_ma']:.3f} mA"
    # assert stats["average_ma"] < 100, f"Display-on current seems too high: {stats['average_ma']:.3f} mA"
    # assert stats["sample_count"] > 10, f"Too few samples collected: {stats['sample_count']}"


@pytest.mark.asyncio
@pytest.mark.ppk2
async def test_display_sleeping_inactive(device_config, ppk2_instance):
    """
    Test to measure current consumption when the display has gone to sleep.

    This test:
    1. Power cycles the device using PPK2
    2. Waits 40 seconds (20s display timeout + 20s safety margin) for display to sleep
    3. Measures current for 10 seconds while display is sleeping
    4. Analyzes and reports current consumption statistics
    """
    stats = _measure_current(
        device_config,
        ppk2_instance,
        test_name="display_sleeping_inactive",
        measurement_duration_s=10,
        delay_s=40,  # 20 seconds is the default display timeout, plus 5 seconds for safety
    )

    # Sanity checks specific to display-sleeping state
    # assert stats["average_ma"] > 0, "Average current should be positive"
    # assert stats["average_ma"] < 5, f"Display-sleeping current seems too high: {stats['average_ma']:.3f} mA"
    # assert stats["average_ma"] > 0.001, f"Display-sleeping current seems too low: {stats['average_ma']:.3f} mA"
    # assert stats["sample_count"] > 10, f"Too few samples collected: {stats['sample_count']}"

@pytest.mark.asyncio
@pytest.mark.ppk2
async def test_not_worn_stationary(device_config, ppk2_instance):
    """
    Test to measure current consumption when the watch is not worn and stationary.

    This test:
    1. Power cycles the device using PPK2
    2. Waits 180 seconds (160s display timeout + 20s safety margin) watch to go to stationary mode
    3. Measures current for 10 seconds while watch is sleeping
    4. Analyzes and reports current consumption statistics
    """
    stats = _measure_current(
        device_config,
        ppk2_instance,
        test_name="not_worn_stationary",
        measurement_duration_s=10,
        delay_s=180,  # 160 seconds is the default display timeout, plus 20 seconds for safety
    )