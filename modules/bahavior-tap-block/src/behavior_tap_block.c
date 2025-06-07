


#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/keycode.h>
#include <zmk/hid.h>
#include <zmk/behavior_queue.h>
#include <sensor_delay_block.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int behavior_tap_block_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                                     struct zmk_behavior_binding_event event) {
    uint32_t param = binding->param1;
    sensor_delay_block_key_pressed();
    return zmk_hid_press(param);
}

static int behavior_tap_block_keymap_binding_released(struct zmk_behavior_binding *binding,
                                                      struct zmk_behavior_binding_event event) {
    uint32_t param = binding->param1;
    return zmk_hid_release(param);
}

static const struct behavior_driver_api behavior_tap_block_driver_api = {
    .binding_pressed = behavior_tap_block_keymap_binding_pressed,
    .binding_released = behavior_tap_block_keymap_binding_released,
};

BEHAVIOR_DT_INST_DEFINE(0, behavior_tap_block_driver_api);