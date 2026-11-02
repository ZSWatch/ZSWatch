# Calculator App

The `smf_calculator_thread.c` is copied from `zephyr/samples/subsys/smf/smf_calculator/src/smf_calculator_thread.c` with minor changes allowing us to use it. The state machine runs directly in the LVGL button callback instead of in its own thread.
This is what handles all the calculations.