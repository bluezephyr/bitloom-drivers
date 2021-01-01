#ifndef SSD1306_DEFINES_H
#define SSD1306_DEFINES_H

/*
 * General
 */
#define SSD1306_COMMAND_BUFFER_LEN  3

/*
 * I2C defines
 */
#define SSD1306_I2C_SLAVE_ADDRESS                         0x78

// First byte after the I2C address
#define SSD1306_COMMAND_STREAM                            0x00
#define SSD1306_COMMAND_SINGLE                            0x80
#define SSD1306_DATA_STREAM                               0x40
#define SSD1306_DATA_SINGLE                               0xC0

/*
 * Fundamental commands
 */
#define SSD1306_SET_CONTRAST                              0x81
#define SSD1306_SET_USE_PIXELS_FROM_RAM                   0xA4
#define SSD1306_SET_PIXELS_ENTIRE_DISPLAY_ON              0xA5
#define SSD1306_SET_NORMAL_DISPLAY                        0xA6
#define SSD1306_SET_INVERTED_DISPLAY                      0xA7
#define SSD1306_DISPLAY_ON                                0xAF
#define SSD1306_DISPLAY_SLEEP                             0xAE

/*
 * Scrolling commands
 */

/*
 * Addressing settings commands
 */

/*
 * Hardware configuration commands
 */
#define SSD1306_SET_DISPLAY_START_LINE                    0x40
#define SSD1306_DISPLAY_START_LINE_MAX                    0x3F
#define SSD1306_SEGMENT_REMAP_0                           0xA0
#define SSD1306_SEGMENT_REMAP_127                         0xA1
#define SSD1306_SET_MULTIPLEX_RATIO                       0xA8
#define SSD1306_MUX_MIN_VALUE                             0x10
#define SSD1306_MUX_MAX_VALUE                             0x40
#define SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL      0xC0
#define SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED    0xC8
#define SSD1306_SET_DISPLAY_OFFSET                        0xD3
#define SSD1306_DISPLAY_OFFSET_MAX_VALUE                  0x3F
#define SSD1306_SET_COM_PINS_HARDWARE_CONFIGURATION       0xDA
#define SSD1306_COM_PINS_HARDWARE_BASE_VALUE              0x02

/*
 * Timing and driving scheme settings commands
 */
#define SSD1306_SET_CLOCK_DIVIDER_AND_OSCILLATOR          0xD5
#define SSD1306_CLOCK_DIVIDER_MIN_VALUE                   0x01
#define SSD1306_CLOCK_DIVIDER_MAX_VALUE                   0x10
#define SSD1306_OSCILLATOR_FREQUENCY_MAX_VALUE            0x0F

/*
 * Charge pump commands
 *
 * The charge pump must be enabled with the following command sequence
 * - Charge pump setting
 * - Charge pump enable
 * - Display on
 */
#define SSD1306_CHARGE_PUMP_SETTING                       0x8D
#define SSD1306_CHARGE_PUMP_ENABLE                        0x14
#define SSD1306_CHARGE_PUMP_DISABLE                       0x10

#endif  // SSD1306_DEFINES_H
