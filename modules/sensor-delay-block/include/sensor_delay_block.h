#pragma once

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

void sensor_delay_block_key_pressed(void);
bool sensor_delay_block_is_blocking(void);

#ifdef __cplusplus
}
#endif