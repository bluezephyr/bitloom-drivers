#ifndef SSD1306_DEFINES_H
#define SSD1306_DEFINES_H

/*
 * General
 */
#define SSD1306_COMMAND_BUFFER_SIZE  4u

/*
 * I2C defines
 */
#define SSD1306_I2C_SLAVE_ADDRESS                         0x78u

// First byte after the I2C address
#define SSD1306_COMMAND_SINGLE                            0x00u
#define SSD1306_COMMAND_STREAM                            0x80u
#define SSD1306_DATA_SINGLE                               0x40u
#define SSD1306_DATA_STREAM                               0xC0u

/*
 * Fundamental commands
 */
#define SSD1306_SET_CONTRAST                              0x81u
#define SSD1306_SET_USE_PIXELS_FROM_RAM                   0xA4u
#define SSD1306_SET_PIXELS_ENTIRE_DISPLAY_ON              0xA5u
#define SSD1306_SET_NORMAL_DISPLAY                        0xA6u
#define SSD1306_SET_INVERTED_DISPLAY                      0xA7u
#define SSD1306_DISPLAY_ON                                0xAFu
#define SSD1306_DISPLAY_SLEEP                             0xAEu

/*
 * Scrolling commands
 */

/*
 * Addressing settings commands
 */
#define SSD1306_SET_MEMORY_ADDRESSING_MODE                0x20u
#define SSD1306_HORIZONTAL_ADDRESSING_MODE                0x00u
#define SSD1306_VERTICAL_ADDRESSING_MODE                  0x01u
#define SSD1306_PAGE_ADDRESSING_MODE                      0x02u
#define SSD1306_SET_COLUMN_ADDRESS                        0x21u
#define SSD1306_SET_PAGE_ADDRESS                          0x22u

/*
 * Hardware configuration commands
 */
#define SSD1306_SET_DISPLAY_START_LINE                    0x40u
#define SSD1306_DISPLAY_START_LINE_MAX                    0x3Fu
#define SSD1306_SEGMENT_REMAP_0                           0xA0u
#define SSD1306_SEGMENT_REMAP_127                         0xA1u
#define SSD1306_SET_MULTIPLEX_RATIO                       0xA8u
#define SSD1306_MUX_MIN_VALUE                             0x10u
#define SSD1306_MUX_MAX_VALUE                             0x40u
#define SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL      0xC0u
#define SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED    0xC8u
#define SSD1306_SET_DISPLAY_OFFSET                        0xD3u
#define SSD1306_DISPLAY_OFFSET_MAX_VALUE                  0x3Fu
#define SSD1306_SET_COM_PINS_HARDWARE_CONFIGURATION       0xDAu
#define SSD1306_COM_PINS_HARDWARE_BASE_VALUE              0x02u

/*
 * Timing and driving scheme settings commands
 */
#define SSD1306_SET_CLOCK_DIVIDER_AND_OSCILLATOR          0xD5u
#define SSD1306_CLOCK_DIVIDER_MIN_VALUE                   0x01u
#define SSD1306_CLOCK_DIVIDER_MAX_VALUE                   0x10u
#define SSD1306_OSCILLATOR_FREQUENCY_MAX_VALUE            0x0Fu

/*
 * Charge pump commands
 *
 * The charge pump must be enabled with the following commandType sequence
 * - Charge pump setting
 * - Charge pump enable
 * - Display on
 */
#define SSD1306_CHARGE_PUMP_SETTING                       0x8Du
#define SSD1306_CHARGE_PUMP_ENABLE                        0x14u
#define SSD1306_CHARGE_PUMP_DISABLE                       0x10u

#endif  // SSD1306_DEFINES_H
