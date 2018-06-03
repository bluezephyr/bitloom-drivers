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
#include <stdio.h>

/*
 * Com functions - to be moved to separate module
 */
#include "i2c_master.h"
void ssd1306_i2c_write(uint8_t* buffer, uint8_t len);
void ssd1306_i2c_write_register(uint8_t reg, uint8_t* buffer, uint8_t len);
/* End Com functions */

/*
 * Local function prototypes
 */
static void ssd1306_process_init_display(void);

/*
 * Processing tables are used to manage multi step commands.
 */
typedef enum
{
    ssd1306_table_none,
    ssd1306_table_init_display,
} ssd1306_processing_table_t;

typedef enum
{
    ssd1306_init_display_set_set_mux,
    ssd1306_init_display_set_display_offset,
    ssd1306_init_display_set_display_start_line,
    ssd1306_init_display_set_segment_remap_0,
    ssd1306_init_display_set_com_output_scan_direction_normal,
    ssd1306_init_display_set_com_pins_hardware_configuration,
    ssd1306_init_display_set_contrast,
    ssd1306_init_display_display_sleep,
    ssd1306_init_display_set_clock_divider_and_oscillator,
    ssd1306_init_display_charge_pump_setting,
    ssd1306_init_display_display_on
} ssd1306_init_display_processing_steps_t;

/*
 * SSD1306 class (singleton)
 */
typedef struct
{
    ssd1306_processing_table_t processing_table;
    uint8_t processing_step;
    uint8_t buffer[SSD1306_COMMAND_BUFFER_LEN];
} ssd1306_t;

static ssd1306_t self;

void ssd1306_init (uint8_t taskid)
{
    self.processing_table = ssd1306_table_none;
    self.processing_step = 0;
}

ssd1306_state_t ssd1306_get_state(void)
{
    ssd1306_state_t state = ssd1306_error;

    switch (i2c_master_get_state())
    {
        case i2c_idle:
            switch (self.processing_table)
            {
                case ssd1306_table_none:
                    state = ssd1306_idle;
                    break;
                case ssd1306_table_init_display:
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

void ssd1306_run (void)
{
    if (i2c_master_get_state() != i2c_idle)
    {
        return;
    }

    switch(self.processing_table)
    {
        case ssd1306_table_none:
            // Nothing to do
            break;

        case ssd1306_table_init_display:
            ssd1306_process_init_display();
            break;
    }
}

static void ssd1306_process_init_display(void)
{
    switch(self.processing_step)
    {
        case ssd1306_init_display_set_set_mux:
            ssd1306_set_multiplex_ratio(SSD1306_DEFAULT_MUX_VALUE);
            self.processing_step = ssd1306_init_display_set_display_offset;
            break;
        case ssd1306_init_display_set_display_offset:
            ssd1306_set_display_offset(SSD1306_DEFAULT_DISPLAY_OFFSET);
            self.processing_step = ssd1306_init_display_set_display_start_line;
            break;
        case ssd1306_init_display_set_display_start_line:
            ssd1306_set_display_start_line(SSD1306_DEFAULT_DISPLAY_STARTLINE);
            self.processing_step = ssd1306_init_display_set_segment_remap_0;
            break;
        case ssd1306_init_display_set_segment_remap_0:
            ssd1306_set_segment_remap_0();
            self.processing_step = ssd1306_init_display_set_com_output_scan_direction_normal;
            break;
        case ssd1306_init_display_set_com_output_scan_direction_normal:
            ssd1306_set_com_output_scan_direction_normal();
            self.processing_step = ssd1306_init_display_set_com_pins_hardware_configuration;
            break;
        case ssd1306_init_display_set_com_pins_hardware_configuration:
            break;
        case ssd1306_init_display_set_contrast:
            break;
        case ssd1306_init_display_display_sleep:
            break;
        case ssd1306_init_display_set_clock_divider_and_oscillator:
            break;
        case ssd1306_init_display_charge_pump_setting:
            break;
        case ssd1306_init_display_display_on:
            break;
    }
}

void ssd1306_init_display(void)
{
    self.processing_table = ssd1306_table_init_display;
    self.processing_step = ssd1306_init_display_set_set_mux;
}

/*
 * Fundamental commands
 */
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

/*
 * Scrolling commands
 */

/*
 * Addressing setting commands
 */

/*
 * Hardware configuration commands
 */
void ssd1306_set_display_start_line(uint8_t line)
{
    if (line <= SSD1306_DISPLAY_START_LINE_MAX)
    {
        self.buffer[0] = SSD1306_COMMAND_SINGLE;
        self.buffer[1] = SSD1306_SET_DISPLAY_START_LINE | line;
        ssd1306_i2c_write (self.buffer, 2);
    }
}

void ssd1306_set_segment_remap_0(void)
{
    self.buffer[0] = SSD1306_COMMAND_SINGLE;
    self.buffer[1] = SSD1306_SEGMENT_REMAP_0;
    ssd1306_i2c_write (self.buffer, 2);
}

void ssd1306_set_segment_remap_127(void)
{
    self.buffer[0] = SSD1306_COMMAND_SINGLE;
    self.buffer[1] = SSD1306_SEGMENT_REMAP_127;
    ssd1306_i2c_write (self.buffer, 2);
}

void ssd1306_set_multiplex_ratio(uint8_t ratio)
{
    if((ratio>=SSD1306_MUX_MIN_VALUE) && (ratio<=SSD1306_MUX_MAX_VALUE))
    {
        self.buffer[0] = SSD1306_COMMAND_SINGLE;
        self.buffer[1] = SSD1306_SET_MULTIPLEX_RATIO;
        self.buffer[2] = ratio - 1;
        ssd1306_i2c_write (self.buffer, 3);
    }
}

void ssd1306_set_com_output_scan_direction_normal(void)
{
    self.buffer[0] = SSD1306_COMMAND_SINGLE;
    self.buffer[1] = SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL;
    ssd1306_i2c_write (self.buffer, 2);
}

void ssd1306_set_com_output_scan_direction_remapped(void)
{
    self.buffer[0] = SSD1306_COMMAND_SINGLE;
    self.buffer[1] = SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED;
    ssd1306_i2c_write (self.buffer, 2);
}

void ssd1306_set_display_offset (uint8_t offset)
{
    if (offset < 64)
    {
        self.buffer[0] = SSD1306_COMMAND_SINGLE;
        self.buffer[1] = SSD1306_SET_DISPLAY_OFFSET;
        self.buffer[2] = offset;
        ssd1306_i2c_write (self.buffer, 3);
    }
}

/*
 * Timing and Driving Scheme Setting commands
 */


/*
 * DATA SEND
 */
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
