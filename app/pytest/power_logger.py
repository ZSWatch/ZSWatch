"""
Simple power consumption logging for GitHub Actions integration
"""

import json
import os
from typing import Dict, Any
from datetime import datetime


POWER_RESULTS_FILE = "power_results.json"


def log_power_measurement(test_name: str, average_ma: float, min_ma: float = None, 
                         max_ma: float = None, duration_s: int = None, **kwargs):
    """
    Log a power measurement result to file for GitHub Actions pickup
    
    Args:
        test_name: Name of the test (e.g., "idle_current_after_boot")
        average_ma: Average current in milliamps
        min_ma: Minimum current in milliamps (optional)
        max_ma: Maximum current in milliamps (optional)
        duration_s: Measurement duration in seconds (optional)
        **kwargs: Additional data to store
    """
    
    # Prepare measurement data
    measurement = {
        "test": test_name,
        "timestamp": datetime.now().isoformat(),
        "average_ma": round(average_ma, 3),
    }
    
    if min_ma is not None:
        measurement["min_ma"] = round(min_ma, 3)
    if max_ma is not None:
        measurement["max_ma"] = round(max_ma, 3)
    if duration_s is not None:
        measurement["duration_s"] = duration_s
    
    # Add any additional data
    measurement.update(kwargs)
    
    # Load existing results or create new structure
    if os.path.exists(POWER_RESULTS_FILE):
        with open(POWER_RESULTS_FILE, 'r') as f:
            try:
                results = json.load(f)
            except json.JSONDecodeError:
                results = {"measurements": []}
    else:
        results = {"measurements": []}
    
    # Add new measurement
    results["measurements"].append(measurement)
    
    # Save back to file
    with open(POWER_RESULTS_FILE, 'w') as f:
        json.dump(results, f, indent=2)


def format_current_value(value_ma: float) -> str:
    """Format current value with appropriate units"""
    if value_ma < 1:
        return f"{value_ma * 1000:.0f} µA"
    else:
        return f"{value_ma:.3f} mA"


def generate_power_comment() -> str:
    """Generate markdown comment for GitHub PR"""
    
    if not os.path.exists(POWER_RESULTS_FILE):
        return "## ⚡ Power Consumption\n\n❌ No power measurements found."
    
    with open(POWER_RESULTS_FILE, 'r') as f:
        try:
            results = json.load(f)
        except json.JSONDecodeError:
            return "## ⚡ Power Consumption\n\n❌ Error reading power measurements."
    
    measurements = results.get("measurements", [])
    if not measurements:
        return "## ⚡ Power Consumption\n\n❌ No power measurements found."
    
    comment = "## ⚡ Power Consumption Results\n\n"
    comment += "| Test | Average Current | Range | Battery Life* |\n"
    comment += "|------|----------------|-------|---------------|\n"
    
    for m in measurements:
        test_name = m["test"].replace("_", " ").title()
        avg_current = format_current_value(m["average_ma"])
        
        # Format range if available
        if "min_ma" in m and "max_ma" in m:
            range_str = f"{format_current_value(m['min_ma'])} - {format_current_value(m['max_ma'])}"
        else:
            range_str = "N/A"
        
        # Calculate battery life (200mAh battery)
        battery_life = "N/A"
        if m["average_ma"] > 0:
            hours = 200 / m["average_ma"]
            if hours > 24:
                battery_life = f"{hours / 24:.1f} days"
            else:
                battery_life = f"{hours:.1f} hours"
        
        comment += f"| {test_name} | {avg_current} | {range_str} | {battery_life} |\n"
    
    comment += "\n*Estimated with 200mAh battery\n"
    
    return comment


if __name__ == "__main__":
    # Generate and print comment when run as script
    print(generate_power_comment())
