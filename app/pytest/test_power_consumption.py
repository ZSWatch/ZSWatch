import time
import logging
import pytest
import utils
from ppk2_helper import analyze_current_samples
from power_logger import log_power_measurement

log = logging.getLogger()


@pytest.mark.asyncio
@pytest.mark.ppk2
async def test_idle_current_after_boot(device_config, ppk2_instance):
    """
    Test to measure idle current consumption during the first 10 seconds after boot.

    This test:
    1. Power cycles the device using PPK2
    2. Measures current for 10 seconds after boot
    3. Analyzes and reports current consumption statistics
    """
    log.info("Starting idle current measurement test")

    # Power cycle device to ensure clean boot
    ppk2_instance.power_cycle_device()

    # Measure current for 10 seconds
    measurement_duration_s = 10
    samples = ppk2_instance.measure_current(measurement_duration_s)

    # Analyze results
    if not samples:
        pytest.fail("No current samples collected during measurement")

    # Get statistics using helper function
    stats = analyze_current_samples(samples)

    # Basic sanity checks
    assert stats["average_ma"] > 0, "Average current should be positive"
    assert (
        stats["average_ma"] < 1000
    ), f"Average current seems too high: {stats['average_ma']:.3f} mA"
    assert (
        stats["sample_count"] > 10
    ), f"Too few samples collected: {stats['sample_count']}"

    # Store results for potential further analysis
    device_config["idle_current_stats"] = stats

    # Log power measurement for GitHub Actions
    log_power_measurement(
        test_name="idle_current_after_boot",
        average_ma=stats["average_ma"],
        min_ma=stats["min_ma"],
        max_ma=stats["max_ma"],
        duration_s=measurement_duration_s,
        sample_count=stats["sample_count"]
    )

    log.info("Idle current measurement test completed successfully")
