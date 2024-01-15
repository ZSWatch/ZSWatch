#define DT_DRV_COMPAT buydisplay_gc9a01

#include <string.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <math.h>
#include <zephyr/logging/log.h>
#include <inttypes.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>

LOG_MODULE_REGISTER(gc9a01, CONFIG_DISPLAY_LOG_LEVEL);

#define GC9A01_SPI_PROFILING

/**
 * gc9a01 display controller driver.
 *
 */

#define GC9A01A_SLPIN 0x10  ///< Enter Sleep Mode
#define GC9A01A_SLPOUT 0x11 ///< Sleep Out
#define GC9A01A_PTLON 0x12  ///< Partial Mode ON
#define GC9A01A_NORON 0x13  ///< Normal Display Mode ON

#define GC9A01A_INVOFF 0x20   ///< Display Inversion OFF
#define GC9A01A_INVON 0x21    ///< Display Inversion ON
#define GC9A01A_DISPOFF 0x28  ///< Display OFF
#define GC9A01A_DISPON 0x29   ///< Display ON

#define GC9A01A_CASET 0x2A ///< Column Address Set
#define GC9A01A_PASET 0x2B ///< Page Address Set
#define GC9A01A_RAMWR 0x2C ///< Memory Write

#define GC9A01A_PTLAR 0x30    ///< Partial Area
#define GC9A01A_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define GC9A01A_TEOFF 0x34    ///< Tearing effect line off
#define GC9A01A_TEON 0x35     ///< Tearing effect line on
#define GC9A01A_MADCTL 0x36   ///< Memory Access Control
#define GC9A01A_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define GC9A01A_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define GC9A01A1_DFUNCTR 0xB6 ///< Display Function Control

#define GC9A01A1_VREG1A 0xC3 ///< Vreg1a voltage control
#define GC9A01A1_VREG1B 0xC4 ///< Vreg1b voltage control
#define GC9A01A1_VREG2A 0xC9 ///< Vreg2a voltage control

#define GC9A01A_RDID1 0xDA ///< Read ID 1
#define GC9A01A_RDID2 0xDB ///< Read ID 2
#define GC9A01A_RDID3 0xDC ///< Read ID 3

#define GC9A01A1_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define GC9A01A1_GMCTRN1 0xE1 ///< Negative Gamma Correction
#define GC9A01A_FRAMERATE 0xE8 ///< Frame rate control

#define GC9A01A_INREGEN2 0xEF ///< Inter register enable 2
#define GC9A01A_GAMMA1 0xF0 ///< Set gamma 1
#define GC9A01A_GAMMA2 0xF1 ///< Set gamma 2
#define GC9A01A_GAMMA3 0xF2 ///< Set gamma 3
#define GC9A01A_GAMMA4 0xF3 ///< Set gamma 4

#define GC9A01A_INREGEN1 0xFE ///< Inter register enable 1

#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

#define DISPLAY_WIDTH         DT_INST_PROP(0, width)
#define DISPLAY_HEIGHT        DT_INST_PROP(0, height)

// Command codes:
#define COL_ADDR_SET        0x2A
#define ROW_ADDR_SET        0x2B
#define MEM_WR              0x2C
#define MEM_WR_CONT         0x3C
#define COLOR_MODE          0x3A
#define COLOR_MODE_12_BIT  0x03
#define COLOR_MODE_16_BIT  0x05
#define COLOR_MODE_18_BIT  0x06
#define SLPIN               0x10
#define SLPOUT              0x11

static const uint8_t initcmd[] = {
    GC9A01A_INREGEN2, 0,
    0xEB, 1, 0x14,
    GC9A01A_INREGEN1, 0,
    GC9A01A_INREGEN2, 0,
    0xEB, 1, 0x14,
    0x84, 1, 0x40,
    0x85, 1, 0xFF,
    0x86, 1, 0xFF,
    0x87, 1, 0xFF,
    0x88, 1, 0x0A,
    0x89, 1, 0x21,
    0x8A, 1, 0x00,
    0x8B, 1, 0x80,
    0x8C, 1, 0x01,
    0x8D, 1, 0x01,
    0x8E, 1, 0xFF,
    0x8F, 1, 0xFF,
    0xB6, 2, 0x00, 0x00,
#if DT_PROP(DT_INST(0, buydisplay_gc9a01), rotation) == 0
    GC9A01A_MADCTL, 1,  MADCTL_MV | MADCTL_MY | MADCTL_MX | MADCTL_BGR,
#elif DT_PROP(DT_INST(0, buydisplay_gc9a01), rotation) == 90
    GC9A01A_MADCTL, 1,  MADCTL_MH | MADCTL_MY | 0 | MADCTL_BGR,
#elif DT_PROP(DT_INST(0, buydisplay_gc9a01), rotation) == 180
    GC9A01A_MADCTL, 1,  MADCTL_MV | 0 | 0 | MADCTL_BGR,
#elif DT_PROP(DT_INST(0, buydisplay_gc9a01), rotation) == 270
    GC9A01A_MADCTL, 1,  MADCTL_MH | MADCTL_MX | 0 | 0 | MADCTL_BGR,
#else
#error "Unsupported rotation. Use 0, 90, 180 or 270."
#endif
    GC9A01A_PIXFMT, 1, COLOR_MODE_16_BIT,
    0x90, 4, 0x08, 0x08, 0x08, 0x08,
    0xBD, 1, 0x06,
    0xBC, 1, 0x00,
    0xFF, 3, 0x60, 0x01, 0x04,
    GC9A01A1_VREG1A, 1, 0x13,
    GC9A01A1_VREG1B, 1, 0x13,
    GC9A01A1_VREG2A, 1, 0x22,
    0xBE, 1, 0x11,
    GC9A01A1_GMCTRN1, 2, 0x10, 0x0E,
    0xDF, 3, 0x21, 0x0c, 0x02,
    GC9A01A_GAMMA1, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
    GC9A01A_GAMMA2, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
    GC9A01A_GAMMA3, 6, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
    GC9A01A_GAMMA4, 6, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
    0xED, 2, 0x1B, 0x0B,
    0xAE, 1, 0x77,
    0xCD, 1, 0x63,
    0x70, 9, 0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03,
    GC9A01A_FRAMERATE, 1, 0x34,
    0x62, 12, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70,
    0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70,
    0x63, 12, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70,
    0x18, 0x13, 0x71, 0xF3, 0x70, 0x70,
    0x64, 7, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07,
    0x66, 10, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00,
    0x67, 10, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98,
    0x74, 7, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00,
    0x98, 2, 0x3e, 0x07,
    GC9A01A_TEON, 1, GC9A01A_INVOFF,
    GC9A01A_INVON, 0,
    GC9A01A_DISPON, 0x80, // Display on
    GC9A01A_SLPOUT, 0x80, // Exit sleep
    0x00                  // End of list
};

struct gc9a01_config {
    struct spi_dt_spec bus;
    struct gpio_dt_spec dc_gpio;
    struct gpio_dt_spec bl_gpio;
    struct gpio_dt_spec reset_gpio;
};

struct gc9a01_point {
    uint16_t X, Y;
};

struct gc9a01_frame {
    struct gc9a01_point start, end;
};

static struct gc9a01_frame frame = {{0, 0}, {DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1}};

static inline int gc9a01_write_cmd(const struct device *dev, uint8_t cmd,
                                   const uint8_t *data, size_t len)
{
    const struct gc9a01_config *config = dev->config;
    struct spi_buf buf = {.buf = &cmd, .len = sizeof(cmd)};
    struct spi_buf_set buf_set = {.buffers = &buf, .count = 1};
    gpio_pin_set_dt(&config->dc_gpio, 0);
    if (spi_write_dt(&config->bus, &buf_set) != 0) {
        LOG_ERR("Failed sending data");
        return -EIO;
    }

    if (data != NULL && len != 0) {
        buf.buf = (void *)data;
        buf.len = len;
        gpio_pin_set_dt(&config->dc_gpio, 1);
        if (spi_write_dt(&config->bus, &buf_set) != 0) {
            LOG_ERR("Failed sending data");
            return -EIO;
        }
    }

    return 0;
}

static void gc9a01_set_frame(const struct device *dev, struct gc9a01_frame frame)
{
    uint8_t data[4];

    data[0] = (frame.start.X >> 8) & 0xFF;
    data[1] = frame.start.X & 0xFF;
    data[2] = (frame.end.X >> 8) & 0xFF;
    data[3] = frame.end.X & 0xFF;
    gc9a01_write_cmd(dev, COL_ADDR_SET, data, sizeof(data));

    data[0] = (frame.start.Y >> 8) & 0xFF;
    data[1] = frame.start.Y & 0xFF;
    data[2] = (frame.end.Y >> 8) & 0xFF;
    data[3] = frame.end.Y & 0xFF;
    gc9a01_write_cmd(dev, ROW_ADDR_SET, data, sizeof(data));
}

static int gc9a01_blanking_off(const struct device *dev)
{
    return gc9a01_write_cmd(dev, GC9A01A_DISPON, NULL, 0);
}

static int gc9a01_blanking_on(const struct device *dev)
{
    return gc9a01_write_cmd(dev, GC9A01A_DISPOFF, NULL, 0);
}

static int gc9a01_write(const struct device *dev, const uint16_t x, const uint16_t y,
                        const struct display_buffer_descriptor *desc,
                        const void *buf)
{
    const struct gc9a01_config *config = dev->config;
    __ASSERT(pm_device_action_run(config->bus.bus, PM_DEVICE_ACTION_RESUME) == 0, "Failed resume SPI Bus");
#ifdef GC9A01_SPI_PROFILING
    uint32_t start_time;
    uint32_t stop_time;
    uint32_t cycles_spent;
    uint32_t nanoseconds_spent;
#endif
    uint16_t x_end_idx = x + desc->width - 1;
    uint16_t y_end_idx = y + desc->height - 1;

    frame.start.X = x;
    frame.end.X = x_end_idx;
    frame.start.Y = y;
    frame.end.Y = y_end_idx;
    gc9a01_set_frame(dev, frame);

    size_t len = (x_end_idx + 1 - x) * (y_end_idx + 1 - y) * 16 / 8;
    //printk("x_start: %d, y_start: %d, x_end: %d, y_end: %d, buf_size: %d, pitch: %d len: %d\n", x, y, x_end_idx, y_end_idx, desc->buf_size, desc->pitch, len);

#ifdef GC9A01_SPI_PROFILING
    start_time = k_cycle_get_32();
#endif
    gc9a01_write_cmd(dev, GC9A01A_RAMWR, buf, len);
#ifdef GC9A01_SPI_PROFILING
    stop_time = k_cycle_get_32();
    cycles_spent = stop_time - start_time;
    nanoseconds_spent = k_cyc_to_ns_ceil32(cycles_spent);
    LOG_DBG("%d =>: %dns", len, nanoseconds_spent);
#endif
    __ASSERT(pm_device_action_run(config->bus.bus, PM_DEVICE_ACTION_SUSPEND) == 0, "Failed suspend SPI Bus");
    return 0;
}

static int gc9a01_read(const struct device *dev, const uint16_t x, const uint16_t y,
                       const struct display_buffer_descriptor *desc, void *buf)
{
    LOG_ERR("not supported");
    return -ENOTSUP;
}

static void *gc9a01_get_framebuffer(const struct device *dev)
{
    LOG_ERR("not supported");
    return NULL;
}

static int gc9a01_set_brightness(const struct device *dev,
                                 const uint8_t brightness)
{
    LOG_WRN("not supported");
    return -ENOTSUP;
}

static int gc9a01_set_contrast(const struct device *dev, uint8_t contrast)
{
    LOG_WRN("not supported");
    return -ENOTSUP;
}

static void gc9a01_get_capabilities(const struct device *dev,
                                    struct display_capabilities *caps)
{
    memset(caps, 0, sizeof(struct display_capabilities));
    caps->x_resolution = DISPLAY_WIDTH;
    caps->y_resolution = DISPLAY_HEIGHT;
    caps->supported_pixel_formats = PIXEL_FORMAT_BGR_565;
    caps->current_pixel_format = PIXEL_FORMAT_BGR_565;
    caps->screen_info = SCREEN_INFO_MONO_MSB_FIRST;
}

static int gc9a01_set_orientation(const struct device *dev,
                                  const enum display_orientation
                                  orientation) {
    LOG_ERR("Unsupported");
    return -ENOTSUP;
}

static int gc9a01_set_pixel_format(const struct device *dev,
                                   const enum display_pixel_format pf)
{
    LOG_ERR("not supported");
    return -ENOTSUP;
}

static int gc9a01_controller_init(const struct device *dev)
{
    int rc;
    int i = 0;
    uint8_t cmd, x, numArgs;
    const uint8_t *addr;
    const struct gc9a01_config *config = dev->config;

    LOG_DBG("Initialize GC9A01 controller");
    gpio_pin_set_dt(&config->reset_gpio, 0);
    k_msleep(5);
    gpio_pin_set_dt(&config->reset_gpio, 1);
    k_msleep(150);
    rc = pm_device_action_run(config->bus.bus, PM_DEVICE_ACTION_RESUME);
    __ASSERT(rc == -EALREADY || rc == 0, "Failed resume SPI Bus");

    addr = initcmd;
    while ((cmd = *addr++) > 0) {
        x = *addr++;
        numArgs = x & 0x7F;
        gc9a01_write_cmd(dev, cmd, addr, numArgs);
        addr += numArgs;
        if (x & 0x80) {
            k_msleep(150);
        }
        i++;
    }

    __ASSERT(pm_device_action_run(config->bus.bus, PM_DEVICE_ACTION_SUSPEND) == 0, "Failed suspend SPI Bus");
    return 0;
}

static int gc9a01_init(const struct device *dev)
{
    const struct gc9a01_config *config = dev->config;
    LOG_DBG("");

    if (!device_is_ready(config->reset_gpio.port)) {
        LOG_ERR("Reset GPIO device not ready");
        return -ENODEV;
    }

    if (!device_is_ready(config->dc_gpio.port)) {
        LOG_ERR("DC GPIO device not ready");
        return -ENODEV;
    }
    gpio_pin_configure_dt(&config->reset_gpio, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&config->dc_gpio, GPIO_OUTPUT_INACTIVE);

    if (!device_is_ready(config->bl_gpio.port)) {
        LOG_ERR("Busy GPIO device not ready");
        return -ENODEV;
    }

    // Default to 0 brightness
    gpio_pin_configure_dt(&config->bl_gpio, GPIO_OUTPUT_INACTIVE);
    return gc9a01_controller_init(dev);
}

static int gc9a01_pm_action(const struct device *dev,
                            enum pm_device_action action)
{
    int err = 0;
    const struct gc9a01_config *config = dev->config;
    __ASSERT(pm_device_action_run(config->bus.bus, PM_DEVICE_ACTION_RESUME) == 0, "Failed resume SPI Bus");

    switch (action) {
        case PM_DEVICE_ACTION_RESUME:
            err = gc9a01_write_cmd(dev, GC9A01A_SLPOUT, NULL, 0);
            k_msleep(5); // According to datasheet wait 5ms after SLPOUT before next command.
            err = gc9a01_write_cmd(dev, GC9A01A_DISPON, NULL, 0);
            break;
        case PM_DEVICE_ACTION_SUSPEND:
            err = gc9a01_write_cmd(dev, GC9A01A_DISPOFF, NULL, 0);
            err = gc9a01_write_cmd(dev, GC9A01A_SLPIN, NULL, 0);
            break;
        case PM_DEVICE_ACTION_TURN_ON:
            err = gc9a01_init(dev);
            break;
        case PM_DEVICE_ACTION_TURN_OFF:
            break;
        default:
            err = -ENOTSUP;
    }

    err = pm_device_action_run(config->bus.bus, PM_DEVICE_ACTION_SUSPEND);
    __ASSERT(err == 0 || err == -EALREADY, "Failed suspend SPI Bus");

    if (err == -EALREADY) {
        err = 0;
    }

    if (err < 0) {
        LOG_ERR("%s: failed to set power mode", dev->name);
    }

    return err;
}

static const struct gc9a01_config gc9a01_config = {
    .bus = SPI_DT_SPEC_INST_GET(0, SPI_OP_MODE_MASTER | SPI_WORD_SET(8), 0),
    .reset_gpio = GPIO_DT_SPEC_INST_GET(0, reset_gpios),
    .dc_gpio = GPIO_DT_SPEC_INST_GET(0, dc_gpios),
    .bl_gpio = GPIO_DT_SPEC_INST_GET(0, bl_gpios),
};

static struct display_driver_api gc9a01_driver_api = {
    .blanking_on = gc9a01_blanking_on,
    .blanking_off = gc9a01_blanking_off,
    .write = gc9a01_write,
    .read = gc9a01_read,
    .get_framebuffer = gc9a01_get_framebuffer,
    .set_brightness = gc9a01_set_brightness,
    .set_contrast = gc9a01_set_contrast,
    .get_capabilities = gc9a01_get_capabilities,
    .set_pixel_format = gc9a01_set_pixel_format,
    .set_orientation = gc9a01_set_orientation,
};

PM_DEVICE_DT_INST_DEFINE(0, gc9a01_pm_action);
DEVICE_DT_INST_DEFINE(0, gc9a01_init, PM_DEVICE_DT_INST_GET(0), NULL, &gc9a01_config, POST_KERNEL,
                      CONFIG_DISPLAY_INIT_PRIORITY, &gc9a01_driver_api);
