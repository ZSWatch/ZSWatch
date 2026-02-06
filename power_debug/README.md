# Power Debug App
Boilerplate app to experiment in a more minimal codebase to optimize power consumption of the WatchDK hardware.

- Purpose: quick power profiling on WatchDK by stepping through an active phase, a suspended phase, and a final deep sleep/System OFF.
- Peripherals: all peripherals such as display, touch, sensors, and flash are asked to enter their lowest supported power states before rails are cut.
- Build (with UART logging example): `west build -b watchdk_nrf5340_cpuapp power_debug -- -DEXTRA_CONF_FILE=boards/watchdk_nrf5340_cpuapp_debug.conf -DDTC_OVERLAY_FILE=boards/watchdk_nrf5340_cpuapp_debug.overlay`
- Tuning: edit the timing/log-level `#define`s near the top of `src/main.c`; no Kconfig changes are needed.
