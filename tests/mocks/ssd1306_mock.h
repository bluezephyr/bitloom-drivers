/*
 * Mock to test the behviour of the SSD 1306 driver.
 *
 * Copyright (c) 2018 BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#ifndef SSD1306_MOCK_H
#define SSD1306_MOCK_H

#include "ssd1306.h"

void ssd1306_mock_create(void);
void ssd1306_mock_destroy(void);
void ssd1306_mock_verify_complete(void);

void ssd1306_mock_expect_i2c_master_write(uint8_t address, uint8_t* data, uint16_t len);
void ssd1306_mock_expect_i2c_master_write_register(uint8_t address, uint8_t reg,
                                                   uint8_t* data, uint16_t len);

/*
 * Com functions
 */

/*
 * Function to simulate that one byte has been transferred on the communication
 * link.
 */
void ssd1306_mock_com_send_byte(void);
void ssd1306_mock_set_com_busy(void);

#endif // SSD1306_MOCK_H
