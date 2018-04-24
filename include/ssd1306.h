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
#ifndef SSD1306_H
#define SSD1306_H

#include "ssd1306_config.h"

/*
 * Current state of the SSD1306 driver
 *
 * idle:    The driver is available and ready to be used.
 * busy:    The driver is currently performing an operation.
 * error:   The driver has encountered an error and stopped.
 */
typedef enum
{
    ssd1306_idle,
    ssd1306_busy,
    ssd1306_error
} ssd1306_state_t;

/*
 * The init function must be called before any other function in the driver is
 * called.
 */
void ssd1306_init (uint8_t taskid);

/*
 * Returns the status of the driver.  The driver will only accept new commands
 * when idle.  The function will also check the availability of the needed
 * resourses (i.e., the communication bus).
 */
ssd1306_state_t ssd1306_get_state(void);

void ssd1306_run (void);


/*
 * Fundamental commands
 */

/*
 * Set contrast level. Contrast increases as the value increases.
 * Default value is 0x7F
 */
void ssd1306_set_contrast(uint8_t level);

#endif // SSD1306_H
