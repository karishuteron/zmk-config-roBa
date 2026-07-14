#define DT_DRV_COMPAT zmk_behavior_pmw3610_cpi

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/input/input_pmw3610.h>
#include <zephyr/sys/util.h>

#include <zmk/behavior.h>

#if DT_HAS_COMPAT_STATUS_OKAY(pixart_pmw3610)

#define PMW3610_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pixart_pmw3610)

static const uint16_t default_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);
static uint16_t current_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);

static int behavior_pmw3610_cpi_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    (void)event;

    int32_t adjustment = binding->param1;
    int next_cpi = adjustment == 0 ? default_cpi
                                   : CLAMP((int)current_cpi + adjustment, 200, 3200);
    int err = pmw3610_set_resolution(DEVICE_DT_GET(PMW3610_NODE), next_cpi);

    if (err == 0) {
        current_cpi = next_cpi;
    }

    return err;
}

#else

static int behavior_pmw3610_cpi_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    return -ENOTSUP;
}

#endif

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

BEHAVIOR_DT_INST_DEFINE(0, behavior_pmw3610_cpi_driver_api);
