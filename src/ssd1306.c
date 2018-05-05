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
#include "ssd1306.h"
#include "ssd1306_defines.h"

/*
 * Com functions - to be moved to separate module
 */
#include "i2c_master.h"
void ssd1306_i2c_write(uint8_t* buffer, uint8_t len);
void ssd1306_i2c_write_register(uint8_t reg, uint8_t* buffer, uint8_t len);
/* End Com functions */

/*
 * Operation step of the ssd1306 driver
 */
typedef enum
{
    ssd1306_step_idle,
    ssd1306_step_busy
} ssd1306_step_t;

/*
 * SSD1306 class (singleton)
 */
typedef struct
{
    ssd1306_step_t step;
    uint8_t buffer[SSD1306_COMMAND_BUFFER_LEN];
} ssd1306_t;

static ssd1306_t self;


void ssd1306_init (uint8_t taskid)
{
    self.step = ssd1306_step_idle;
}

ssd1306_state_t ssd1306_get_state(void)
{
    ssd1306_state_t state = ssd1306_error;

    switch (i2c_master_get_state())
    {
        case i2c_idle:
            switch (self.step)
            {
                case ssd1306_step_idle:
                    state = ssd1306_idle;
                    break;
                case ssd1306_step_busy:
                    state = ssd1306_busy;
                    break;
            }
            break;

        case i2c_busy:
            state = ssd1306_busy;
            break;

        case i2c_error:
            state = ssd1306_error;
            break;
    }
    return state;
}

void ssd1306_set_contrast(uint8_t level)
{
    self.buffer[0] = SSD1306_COMMAND_SINGLE;
    self.buffer[1] = SSD1306_SET_CONTRAST;
    self.buffer[2] = level;
    ssd1306_i2c_write (self.buffer, 3);
}

void ssd1306_set_display_on(void)
{
    self.buffer[0] = SSD1306_COMMAND_SINGLE;
    self.buffer[1] = SSD1306_DISPLAY_ON;
    ssd1306_i2c_write (self.buffer, 2);
}

void ssd1306_set_display_sleep(void)
{
    self.buffer[0] = SSD1306_COMMAND_SINGLE;
    self.buffer[1] = SSD1306_DISPLAY_SLEEP;
    ssd1306_i2c_write (self.buffer, 2);
}

void ssd1306_send_graphics_data(uint8_t* buffer, uint16_t len)
{
    ssd1306_i2c_write_register(SSD1306_DATA_STREAM, buffer, len);
}

/*
 * Com functions for I2C
 */
void ssd1306_i2c_write(uint8_t* buffer, uint8_t len)
{
    i2c_master_write(SSD1306_I2C_SLAVE_ADDRESS, buffer, len);
}

void ssd1306_i2c_write_register(uint8_t reg, uint8_t* buffer, uint8_t len)
{
    i2c_master_write_register(SSD1306_I2C_SLAVE_ADDRESS, reg, buffer, len);
}
