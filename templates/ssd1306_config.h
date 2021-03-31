#ifndef SSD1306_CONFIG_H
#define SSD1306_CONFIG_H

#include <stdbool.h>

/*
 * Default values that are used in the init display function.
 * See the datasheet (application note section) for more information.
 */
#define SSD1306_DEFAULT_MUX_VALUE                           0x3Fu
#define SSD1306_DEFAULT_DISPLAY_STARTLINE                   0x00u
#define SSD1306_DEFAULT_DISPLAY_OFFSET                      0x00u
#define SSD1306_DEFAULT_COM_HW_PIN_USE_ALT_COM_PIN_CONF     true
#define SSD1306_DEFAULT_COM_HW_PIN_EN_LEFT_RIGHT_REMAP      false
#define SSD1306_DEFAULT_CONTRAST                            0x7Fu
#define SSD1306_DEFAULT_MEMORY_ADDRESSING_MODE              SSD1306_PAGE_ADDRESSING_MODE

#define SSD1306_DEFAULT_DISPLAY_CLOCK_DIVIDE_RATIO          0x01u
#define SSD1306_DEFAULT_OSCILLATOR_FREQUENCY                0x08u

#endif  // SSD1306_CONFIG_H
