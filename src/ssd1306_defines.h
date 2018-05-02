#ifndef SSD1306_DEFINES_H
#define SSD1306_DEFINES_H

/*
 * General
 */
#define SSD1306_COMMAND_BUFFER_LEN  2

/*
 * I2C defines
 */
#define SSD1306_I2C_SLAVE_ADDRESS       0x78

/*
 * Fundamental commands
 */
#define SSD1306_SET_CONTRAST        0x81
#define SSD1306_DISPLAY_ON          0xAF
#define SSD1306_DISPLAY_SLEEP       0xAE

#endif  // SSD1306_DEFINES_H
