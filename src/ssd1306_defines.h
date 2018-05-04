#ifndef SSD1306_DEFINES_H
#define SSD1306_DEFINES_H

/*
 * General
 */
#define SSD1306_COMMAND_BUFFER_LEN  3

/*
 * I2C defines
 */
#define SSD1306_I2C_SLAVE_ADDRESS       0x78

// First byte after the I2C address
#define SSD1306_COMMAND_STREAM          0x00
#define SSD1306_COMMAND_SINGLE          0x80
#define SSD1306_DATA_STREAM             0x40
#define SSD1306_DATA_SINGLE             0xC0

/*
 * Fundamental commands
 */
#define SSD1306_SET_CONTRAST            0x81
#define SSD1306_DISPLAY_ON              0xAF
#define SSD1306_DISPLAY_SLEEP           0xAE

#endif  // SSD1306_DEFINES_H
