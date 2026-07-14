#define DT_DRV_COMPAT zmk_behavior_pmw3610_cpi

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/input/input_pmw3610.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>
#include <dt-bindings/zmk/pmw3610-cpi.h>
#include <zmk/behavior.h>

#define PMW3610_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pixart_pmw3610)
#define PMW3610_CPI_STEP 200
#define PMW3610_CPI_MIN 200
#define PMW3610_CPI_MAX 3200

static const uint16_t default_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);
static uint16_t current_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);

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
