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
 * Returns the status of the driver.  The function will also check the
 * availability of the needed resourses (i.e., the communication bus).
 */
ssd1306_state_t ssd1306_get_state(void);

/*
 * Function to run the SSD1306 task.  This function must be executes to service
 * the multi step commands of the driver.
 */
void ssd1306_run (void);

/*
 * COMMANDS
 *
 * Note! For all commands, it is the responsibility of the application to make
 * sure that the state is idle before any command functions are used.
 */

/*
 * MULTI STEP COMMANDS
 *
 * The following commands require that the run function is executed in order
 * for the command to be processed.
 */

/*
 * Function to initalize the OLED display.  The function is multi step and
 * requires the run function to be executed until the get_state function
 * returns idle.
 *
 * The function uses the values in the config file to configure the diplay.
 */
void ssd1306_init_display(void);

/*
 * SIMPLE COMMANDS
 *
 */

/*
 * Fundamental commands
 */

/*
 * Set contrast level. Contrast increases as the value increases.
 * Default value is 0x7F
 */
void ssd1306_set_contrast(uint8_t level);

/*
 * Turn the OLED panel display on or put it in sleep mode (default value).
 */
void ssd1306_set_display_on (void);
void ssd1306_set_display_sleep (void);

/*
 * Scrolling commands
 */

/*
 * Addressing setting commands
 */

/*
 * Hardware configuration commands
 */
void ssd1306_set_multiplex_ratio (uint8_t value);

/*
 * Timing and Driving Scheme Setting commands
 */

/*
 * DATA SEND
 *
 * The function will send the specified graphics data to the OLED panel using
 * the communication bus.  The buffer and its contents must not be modified
 * until the state is idle.
 */
void ssd1306_send_graphics_data(uint8_t* buffer, uint16_t len);

#endif // SSD1306_H
