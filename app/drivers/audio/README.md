# DMIC Emulator Driver

This is an out-of-tree DMIC emulator driver for Zephyr, designed for testing DMIC applications on platforms without real DMIC hardware (such as native_sim).

## Features

- **Fixed Sine Wave Generation**: Generates a configurable sine wave signal (default: 1kHz at 50% amplitude)
- **Standard DMIC API**: Implements the full Zephyr DMIC API (configure, trigger, read)
- **Mono/Stereo Support**: Supports both mono and stereo output configurations
- **Configurable Sample Rates**: Works with any sample rate requested by the application
- **Thread-based Generation**: Uses a dedicated thread for real-time audio data generation
- **Memory Slab Integration**: Uses the application's memory slab for buffer management

## Configuration

The driver can be configured through Device Tree properties:

```dts
dmic0: dmic-emulator {
    compatible = "zephyr,dmic-emul";
    status = "okay";
    max-streams = <1>;                  /* Maximum number of streams */
    default-sine-freq = <1000>;         /* Sine wave frequency in Hz */
    default-amplitude = <16384>;        /* Amplitude (0-32767 for 16-bit) */
};
```

### Kconfig Options

- `CONFIG_AUDIO_DMIC_EMUL`: Enable the DMIC emulator driver
- `CONFIG_DMIC_EMUL_THREAD_STACK_SIZE`: Stack size for audio generation thread (default: 2048)
- `CONFIG_DMIC_EMUL_THREAD_PRIORITY`: Priority of the generation thread (default: 5)
- `CONFIG_DMIC_EMUL_QUEUE_SIZE`: Number of buffers that can be queued (default: 4)

## Usage

### Building for native_sim

1. Ensure the driver is enabled in your configuration:
   ```
   CONFIG_AUDIO=y
   CONFIG_AUDIO_DMIC=y
   CONFIG_AUDIO_DMIC_EMUL=y
   CONFIG_NEWLIB_LIBC=y
   CONFIG_NEWLIB_LIBC_FLOAT_PRINTF=y
   ```

2. Add the device tree overlay (already provided in `app/boards/native_sim.overlay`)

3. Build your application:
   ```bash
   west build -b native_sim
   ```

### Application Code

```c
#include <zephyr/audio/dmic.h>

/* Get the DMIC device */
const struct device *dmic_dev = DEVICE_DT_GET(DT_NODELABEL(dmic0));

/* Configure DMIC with your desired settings */
struct dmic_cfg config = {
    /* ... your configuration ... */
};
dmic_configure(dmic_dev, &config);

/* Start audio capture */
dmic_trigger(dmic_dev, DMIC_TRIGGER_START);

/* Read audio data */
void *buffer;
size_t size;
int ret = dmic_read(dmic_dev, 0, &buffer, &size, timeout);

/* Process your audio data */
/* ... */

/* Return buffer when done */
k_mem_slab_free(your_mem_slab, buffer);
```

## Test Application

A test application is provided in `app/test/dmic_emul_test.c` that demonstrates:

- DMIC configuration
- Starting/stopping audio capture
- Reading and analyzing audio buffers
- Basic audio statistics calculation

To run the test:

```bash
cd app/test
west build -b native_sim -DCONF_FILE=prj_dmic_test.conf -DDTC_OVERLAY_FILE=../boards/native_sim.overlay src/dmic_emul_test.c
west build -t run
```

## Generated Audio Signal

The emulator generates a pure sine wave with the following characteristics:

- **Frequency**: Configurable via device tree (default: 1kHz)
- **Amplitude**: Configurable via device tree (default: 50% of full scale)
- **Sample Width**: 16-bit signed integers
- **Channels**: Mono or stereo (stereo outputs the same signal on both channels)
- **Phase**: Continuous across buffer boundaries for seamless audio

The sine wave provides a predictable, non-silent signal that's perfect for:
- Testing DMIC-based applications
- Verifying audio processing algorithms
- Automated testing and continuous integration
- Development on platforms without real microphones

## File Structure

```
app/drivers/audio/
├── CMakeLists.txt          # Build configuration
├── Kconfig                 # Audio drivers Kconfig
├── Kconfig.dmic_emul      # DMIC emulator specific options
└── dmic_emul.c            # Main driver implementation

app/boards/
├── native_sim.overlay     # Device tree overlay for native_sim
└── native_sim.conf        # Configuration for native_sim

app/test/
├── dmic_emul_test.c       # Test application
└── prj_dmic_test.conf     # Test configuration
```

## Integration with Existing Projects

To integrate this driver into your existing DMIC-based application:

1. Copy the driver files to your project's `drivers/` directory
2. Add the device tree overlay for your target platform
3. Enable the driver in your project configuration
4. Your existing DMIC application code should work without changes

The emulator is designed as a drop-in replacement for real DMIC drivers.
