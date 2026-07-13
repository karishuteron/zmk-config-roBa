#include <errno.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/input/input_pmw3610.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>

#include <zmk/behavior.h>

#define DT_DRV_COMPAT zmk_behavior_pmw3610_cpi

#if DT_HAS_COMPAT_STATUS_OKAY(pixart_pmw3610)

#define PMW3610_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pixart_pmw3610)
#define CPI_SETTINGS_NAME "pmw3610_cpi/value"
#define CPI_SAVE_DELAY_MS 2000

static const uint16_t default_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);
static uint16_t current_cpi = DT_PROP_OR(PMW3610_NODE, res_cpi, 400);

static void save_cpi(struct k_work *work) {
    (void)work;
    settings_save_one(CPI_SETTINGS_NAME, &current_cpi, sizeof(current_cpi));
}

K_WORK_DELAYABLE_DEFINE(cpi_save_work, save_cpi);

static int cpi_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    if (strcmp(name, "value") != 0 || len != sizeof(current_cpi)) {
        return -EINVAL;
    }

    ssize_t read_len = read_cb(cb_arg, &current_cpi, sizeof(current_cpi));
    if (read_len != (ssize_t)sizeof(current_cpi) || current_cpi < 200 || current_cpi > 3200) {
        return -EINVAL;
    }

    return 0;
}

static int cpi_settings_commit(void) {
    return pmw3610_set_resolution(DEVICE_DT_GET(PMW3610_NODE), current_cpi);
}

SETTINGS_STATIC_HANDLER_DEFINE(pmw3610_cpi, "pmw3610_cpi", NULL, cpi_settings_set,
                               cpi_settings_commit, NULL);

static int behavior_pmw3610_cpi_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    (void)event;
    int32_t adjustment = binding->param1;
    int next_cpi = adjustment == 0 ? default_cpi
                                   : CLAMP((int)current_cpi + adjustment, 200, 3200);
    int err = pmw3610_set_resolution(DEVICE_DT_GET(PMW3610_NODE), next_cpi);

    if (err == 0) {
        current_cpi = next_cpi;
        k_work_reschedule(&cpi_save_work, K_MSEC(CPI_SAVE_DELAY_MS));
    }

    return err;
}

#else

static int behavior_pmw3610_cpi_pressed(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    return 0;
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
