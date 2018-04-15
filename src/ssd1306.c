/*
 * Bitloom driver for the SSD1306 display.
 *
 * This module implements a driver for the SSD1306 display chip.
 *
 * Copyright (c) 2016-2018. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include "ssd1306.h"

void ssd1306_init (uint8_t taskid)
{
}


ssd1306_state_t ssd1306_get_state(void)
{
    return ssd1306_idle;
}
