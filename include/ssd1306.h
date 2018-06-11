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

#include <stdint.h>
#include <stdbool.h>

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
void ssd1306_init(uint8_t taskid);

/*
 * Returns the status of the driver.  The function will also check the
 * availability of the needed resourses (i.e., the communication bus).
 */
ssd1306_state_t ssd1306_get_state(void);

/*
 * Function to run the SSD1306 task.  This function must be executes to service
 * the multi step commands of the driver.
 */
void ssd1306_run(void);

/*
 * COMMANDS
 *
 * Note! For all commands, it is the responsibility of the application to make
 * sure that the state is idle before any command functions are used.
 */

/**** MULTI STEP COMMANDS ****
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


/**** SINGLE STEP COMMANDS ****/

/*
 * FUNDAMENTAL COMMANDS
 */

/*
 * Set contrast level. Contrast increases as the value increases.
 * Default value is 0x7F
 */
void ssd1306_set_contrast(uint8_t level);

/*
 * Select if the pixels are set based on the contents of the display's internal
 * RAM or if all pixels on the display shall be turned on regardless on the RAM
 * contents.
 * Default value is to base on RAM
 */
void ssd1306_set_pixels_from_RAM(void);
void ssd1306_set_pixels_entire_display_on(void);

/*
 * Set normal or inverted display.
 * Default value is normal.
 */
void ssd1306_set_normal_display(void);
void ssd1306_set_inverted_display(void);

/*
 * Turn the OLED panel display on or put it in sleep mode.
 * Default value is sleep mode.
 */
void ssd1306_set_display_on(void);
void ssd1306_set_display_sleep(void);

/*
 * SCROLLING COMMANDS
 */

/*
 * ADDRESSING SETTING COMMANDS
 */

/*
 * HARDWARE CONFIGURATION COMMANDS
 */

/*
 * Set display RAM display start line register from 0-63.
 * Default value is 0.
 */
void ssd1306_set_display_start_line(uint8_t line);

/*
 * Set which column address that is mapped to segment 0.
 * Default value is 0.
 */
void ssd1306_set_segment_remap_0(void);
void ssd1306_set_segment_remap_127(void);

/*
 * Set MUX ratio from 16MUX to 64MUX (decimal).
 * Default value is 64.
 */
void ssd1306_set_multiplex_ratio(uint8_t ratio);

/*
 * This command sets the scan direction of the COM output.
 * In remapped mode; scan from COM[N-1] to COM0, where N is the Multiplex ratio.
 * Default value is normal.
 */
void ssd1306_set_com_output_scan_direction_normal(void);
void ssd1306_set_com_output_scan_direction_remapped(void);

/*
 * Set vertical shift from 0-63 (decimal).
 * Default value is 0.
 */
void ssd1306_set_display_offset(uint8_t offset);

/*
 * Specify COM pins hardware configuration.
 *
 * use_alt_com_pin_conf parameter:
 *   Use alternative COM pin configuration (true) or
 *   use sequential COM pin configuration (false)
 *   Default value is true
 *
 * enable_left_right_remap parameter:
 *   Disable COM Left/Right remap (false) or
 *   Enable COM Left/Right remap (true)
 *   Default value is false.
 */
void ssd1306_set_com_pins_hardware_config(bool use_alt_com_pin_conf,
                                          bool enable_left_right_remap);


/*
 * TIMING AND DRIVING SCHEME SETTING COMMANDS
 */

/*
 * Display clock settings.
 *
 * divide_ratio parameter:
 *   Divide ratio is from 1 to 16.
 *   Default value is 1.
 *
 * oscillator_frequency parameter:
 *   The oscillator frewuency is a value between 0 and 15 (decimal)
 *   Higher value on this parameter gives higher frequency
 *   Default value is 8.
 */
void ssd1306_set_display_clock(uint8_t divide_ratio, uint8_t oscillator_frequency);

/*
 * CHARGE PUMP REGULATOR COMMANDS
 */

/*
 * The charge pump must be enabled with the following sequence:
 *  ssd1306_enable_charge_pump();
 *  ssd1306_set_display_on();
 * The charge pump is disabled by default.
 */
void ssd1306_enable_charge_pump(void);
void ssd1306_disable_charge_pump(void);


/**** DATA SEND COMMAND ****/

/*
 * The function will send the specified graphics data to the OLED panel using
 * the communication bus.  The buffer and its contents must not be modified
 * until the state is idle.
 */
void ssd1306_send_graphics_data(uint8_t* buffer, uint16_t len);

#endif // SSD1306_H
