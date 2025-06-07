#include <zephyr/kernel.h>
#include "sensor_delay_block.h"

static struct k_work_delayable unblock_work;
static bool block_active = false;

#define SENSOR_DELAY_BLOCK_TIMEOUT_MS 250

static void unblock_fn(struct k_work *work) {
    block_active = false;
}

void sensor_delay_block_key_pressed(void) {
    block_active = true;
    k_work_schedule(&unblock_work, K_MSEC(SENSOR_DELAY_BLOCK_TIMEOUT_MS));
}

bool sensor_delay_block_is_blocking(void) {
    return block_active;
}

K_WORK_DELAYABLE_DEFINE(unblock_work, unblock_fn);