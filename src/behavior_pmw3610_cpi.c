#define DT_DRV_COMPAT zmk_behavior_pmw3610_cpi

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/input/input_pmw3610.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>
#include <dt-bindings/zmk/pmw3610-cpi.h>
#include <zmk/behavior.h>

#define PMW3610_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pixart_pmw3610)
#define PMW3610_CPI_STEP 200
#define PMW3610_CPI_MIN 200
#define PMW3610_CPI_MAX 3200
#define PMW3610_CPI_SETTINGS_SUBTREE "pmw3610_cpi"
#define PMW3610_CPI_SETTINGS_KEY "resolution"
#define PMW3610_CPI_SETTINGS_PATH PMW3610_CPI_SETTINGS_SUBTREE "/" PMW3610_CPI_SETTINGS_KEY

static const uint16_t default_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);
static uint16_t current_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);

static bool pmw3610_cpi_is_valid(uint16_t cpi) {
    return cpi >= PMW3610_CPI_MIN && cpi <= PMW3610_CPI_MAX && cpi % PMW3610_CPI_STEP == 0;
}

#if IS_ENABLED(CONFIG_SETTINGS)
static int pmw3610_cpi_settings_set(const char *name, size_t len, settings_read_cb read_cb,
                                    void *cb_arg) {
    ssize_t read_len;
    uint16_t saved_cpi;

    if (!settings_name_steq(name, PMW3610_CPI_SETTINGS_KEY, NULL)) {
        return 0;
    }

    if (len != sizeof(saved_cpi)) {
        return -EINVAL;
    }

    read_len = read_cb(cb_arg, &saved_cpi, sizeof(saved_cpi));
    if (read_len != sizeof(saved_cpi) || !pmw3610_cpi_is_valid(saved_cpi)) {
        return -EINVAL;
    }

    current_cpi = saved_cpi;
    return 0;
}

static int pmw3610_cpi_settings_commit(void) {
    const struct device *sensor = DEVICE_DT_GET(PMW3610_NODE);

    if (!device_is_ready(sensor)) {
        return -ENODEV;
    }

    return pmw3610_set_resolution(sensor, current_cpi);
}

SETTINGS_STATIC_HANDLER_DEFINE(pmw3610_cpi, PMW3610_CPI_SETTINGS_SUBTREE, NULL,
                               pmw3610_cpi_settings_set, pmw3610_cpi_settings_commit, NULL);
#endif

static int behavior_pmw3610_cpi_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    const struct device *sensor = DEVICE_DT_GET(PMW3610_NODE);
    int next_cpi;
    int err;

    (void)event;

    if (!device_is_ready(sensor)) {
        return -ENODEV;
    }

    switch (binding->param1) {
    case PMW3610_CPI_INCREASE:
        next_cpi = MIN(current_cpi + PMW3610_CPI_STEP, PMW3610_CPI_MAX);
        break;
    case PMW3610_CPI_DECREASE:
        next_cpi = MAX(current_cpi - PMW3610_CPI_STEP, PMW3610_CPI_MIN);
        break;
    case PMW3610_CPI_RESET:
        next_cpi = default_cpi;
        break;
    default:
        return -EINVAL;
    }

    err = pmw3610_set_resolution(sensor, next_cpi);
    if (err == 0) {
        current_cpi = next_cpi;
#if IS_ENABLED(CONFIG_SETTINGS)
        err = settings_save_one(PMW3610_CPI_SETTINGS_PATH, &current_cpi, sizeof(current_cpi));
#endif
    }

    return err;
}

static int behavior_pmw3610_cpi_released(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    return 0;
}

static const struct behavior_driver_api behavior_pmw3610_cpi_driver_api = {
    .binding_pressed = behavior_pmw3610_cpi_pressed,
    .binding_released = behavior_pmw3610_cpi_released,
};

#define PMW3610_CPI_INST(n)                                                                       \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                              \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                                 \
                            &behavior_pmw3610_cpi_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PMW3610_CPI_INST)
