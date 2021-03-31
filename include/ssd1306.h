/*
 * BitLoom driver for the SSD1306 display.
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
 * This driver provides a set of functions to control an SSD1306 display in an
 * asynchronous manner. All commands will return immediately with a response code
 * for the requested commandType. If the returned value is "ssd1306_request_ok", the
 * the commandType will be processed and the result will be published in the "result"
 * out parameter of the function.
 */

enum ssd1306_request_t
{
    ssd1306_request_ok,
    ssd1306_request_busy
};

enum ssd1306_result_t
{
    ssd1306_result_ok,
    ssd1306_result_processing
};

/*
 * The init function must be called before any other function in the driver is
 * called.
 */
void ssd1306_init(uint8_t taskId);

/*
 * Function to run the SSD1306 task. Called by the scheduler.
 */
void ssd1306_run(void);

/*
 * COMMANDS
 */

/*
 * Function to initialize the OLED display. The function uses the values in the
 * config file to configure the display.
 */
enum ssd1306_request_t ssd1306_initDisplay(enum ssd1306_result_t *result);

/*
 * FUNDAMENTAL COMMANDS
 */

/*
 * Set contrast level. Contrast increases as the value increases.
 * Default value is 0x7F
 */
enum ssd1306_request_t ssd1306_setContrast(uint8_t level, enum ssd1306_result_t *result);

/*
 * Select if the pixels are set based on the contents of the display's internal
 * RAM or if all pixels on the display shall be turned on regardless on the RAM
 * contents.
 * Default value is to base on RAM
 */
enum ssd1306_request_t ssd1306_setPixelsFromRAM(enum ssd1306_result_t *result);
enum ssd1306_request_t ssd1306_setAllPixelsActive(enum ssd1306_result_t *result);

/*
 * Set normal or inverted display.
 * Default value is normal.
 */
enum ssd1306_request_t ssd1306_setNormalDisplay(enum ssd1306_result_t *result);
enum ssd1306_request_t ssd1306_setInvertedDisplay(enum ssd1306_result_t *result);

/*
 * Turn the OLED panel display on or put it in sleep mode.
 * Default value is sleep mode.
 */
enum ssd1306_request_t ssd1306_setDisplayOn(enum ssd1306_result_t *result);
enum ssd1306_request_t ssd1306_setDisplaySleep(enum ssd1306_result_t *result);

/*
 * SCROLLING COMMANDS
 */

/*
 * ADDRESSING SETTING COMMANDS
 */

/*
 * Set the memory addressing mode. The value will be used when data is sent to
 * the display. Page addressing mode is default.
 */
enum ssd1306_addressing_mode_t
{
    ssd1306_addressing_horizontal,
    ssd1306_addressing_vertical,
    ssd1306_addressing_page
};
void ssd1306_setMemoryAddressingMode(enum ssd1306_addressing_mode_t mode);

/*
 * Set the column start and end addresses. The values will be used when data is
 * sent to the display. Note that wrapping mechanisms differ depending on which
 * memory addressing mode that is used. Consult the data sheet for details.
 *
 * Column range: 0-127 (default: start=0, end=127)
 * Page range: 0-7 (default: start=0, end=7)
 */
void ssd1306_setColumnAddress(uint8_t startAddress, uint8_t endAddress);
void ssd1306_setPageAddress(uint8_t startAddress, uint8_t endAddress);

/*
 * HARDWARE CONFIGURATION COMMANDS
 */

/*
 * Set display RAM display start line register from 0-63.
 * Default value is 0.
 */
enum ssd1306_request_t ssd1306_setDisplayStartLine(uint8_t line, enum ssd1306_result_t *result);

/*
 * Set which column address that is mapped to segment 0.
 * Default value is 0.
 */
enum ssd1306_request_t ssd1306_setSegmentRemap_0(enum ssd1306_result_t *result);
enum ssd1306_request_t ssd1306_setSegmentRemap_127(enum ssd1306_result_t *result);

/*
 * Set MUX ratio from 16MUX to 64MUX (decimal).
 * Default value is 64.
 */
enum ssd1306_request_t ssd1306_setMultiplexRatio(uint8_t ratio, enum ssd1306_result_t *result);

/*
 * This commandType sets the scan direction of the COM output.
 * In remapped mode; scan from COM[N-1] to COM0, where N is the Multiplex ratio.
 * Default value is normal.
 */
enum ssd1306_request_t ssd1306_setComOutputScanDirectionNormal(enum ssd1306_result_t *result);
enum ssd1306_request_t ssd1306_setComOutputScanDirectionRemapped(enum ssd1306_result_t *result);

/*
 * Set vertical shift from 0-63 (decimal).
 * Default value is 0.
 */
enum ssd1306_request_t ssd1306_setDisplayOffset(uint8_t offset, enum ssd1306_result_t *result);

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
enum ssd1306_request_t ssd1306_setComPinsHardwareConfig(bool useAltComPinConf, bool enableLeftRightRemap, enum ssd1306_result_t *result);


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
 *   The oscillator frequency is a value between 0 and 15 (decimal)
 *   Higher value on this parameter gives higher frequency
 *   Default value is 8.
 */
enum ssd1306_request_t ssd1306_setDisplayClock(uint8_t divideRatio, uint8_t oscillatorFrequency, enum ssd1306_result_t *result);

/*
 * CHARGE PUMP REGULATOR COMMANDS
 */

/*
 * The charge pump must be enabled with the following sequence:
 *  ssd1306_enableChargePump();
 *  ssd1306_setDisplayOn();
 * The charge pump is disabled by default.
 */
enum ssd1306_request_t ssd1306_enableChargePump(enum ssd1306_result_t *result);
enum ssd1306_request_t ssd1306_disableChargePump(enum ssd1306_result_t *result);


/**** DATA SEND COMMAND ****/
/*
 * The function will set the starting page and column and then send the specified
 * graphics data to the OLED panel using the communication bus. The commandBuffer and
 * its contents must not be modified until the result output parameter is set to
 * ssd1306_result_ok.
 *
 * Note that the function will use the configured addressing mode including start/end
 * page and start/end column.
 */
enum ssd1306_request_t ssd1306_sendGraphicsData(uint8_t *buffer, uint16_t len, enum ssd1306_result_t *result);

#endif // SSD1306_H
