# Power Consumption Integration

Simple integration for logging power consumption test results to GitHub Actions and PR comments.

## Usage

### In your test file:

```python
from power_logger import log_power_measurement

@pytest.mark.ppk2
async def test_your_power_scenario(device_config, ppk2_instance):
    # Your test logic here...
    samples = ppk2_instance.measure_current(duration_s=5)
    stats = analyze_current_samples(samples)
    
    # Log the measurement
    log_power_measurement(
        test_name="your_power_scenario",
        average_ma=stats["average_ma"],
        min_ma=stats["min_ma"],
        max_ma=stats["max_ma"],
        duration_s=5
    )
```

### Files created:

- `power_results.json` - Contains all measurements from test run
- `power_comment.md` - Generated markdown for PR comments

### GitHub Actions:

The workflow automatically:
1. Runs power consumption tests (if PPK2 hardware available)
2. Uploads `power_results.json` as artifact
3. Creates/updates PR comment with power consumption table

### Result:

PR comments will show a table like:

| Test | Average Current | Range | Battery Life* |
|------|----------------|-------|---------------|
| Idle Current After Boot | 2.345 mA | 1.2 mA - 4.5 mA | 3.6 days |

*Estimated with 200mAh battery
