/*
 * Unit tests for the Bit Loom SSD 1306 driver.
 *
 * Copyright (c) 2018. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include <string.h>
#include "unity_fixture.h"
#include "ssd1306.h"
#include "ssd1306_defines.h"
#include "ssd1306_mock.h"

/*
 * Tests to be written:
 * - init_chip function
 * - set_update_area
 * - send_data
 * - Make sure that the selected bus (SPI or I2C) is used
 */

/*
 * Defines for the test cases.
 */
#define SSD_TASK_ID                             1
#define SSD_LOW_CONTRAST                     0x20
#define SSD_DISPLAY_LINE_VALUE_OK            0x20
#define SSD_DISPLAY_LINE_VALUE_TOO_HIGH      0x40
#define SSD_MULTIPLEX_RATIO_VALUE_TOO_LOW    0x0F
#define SSD_MULTIPLEX_RATIO_VALUE_TOO_HIGH   0x41
#define SSD_MULTIPLEX_RATIO_VALUE_OK         0x20
#define SSD_DISPLAY_OFFSET_VALUE_OK          0x10
#define SSD_DISPLAY_OFFSET_VALUE_TOO_HIGH    0x40
#define SSD_CONTRAST_COMMAND_LENGTH             3
#define I2C_TASK_BYTES_PER_TICK                 8

TEST_GROUP(ssd1306_i2c);
TEST_GROUP_RUNNER(ssd1306_i2c)
{
    RUN_TEST_CASE(ssd1306_i2c, init);
    RUN_TEST_CASE(ssd1306_i2c, get_state_returns_busy_if_i2c_busy);
    RUN_TEST_CASE(ssd1306_i2c, call_command_changes_state_to_busy);
    RUN_TEST_CASE(ssd1306_i2c, busy_when_not_complete_command_sent_on_i2c);
    RUN_TEST_CASE(ssd1306_i2c, idle_when_complete_command_sent_on_i2c);
    RUN_TEST_CASE(ssd1306_i2c, set_contrast_command);
    RUN_TEST_CASE(ssd1306_i2c, set_display_on);
    RUN_TEST_CASE(ssd1306_i2c, set_display_off);
    RUN_TEST_CASE(ssd1306_i2c, send_graphics_data_over_i2c);
    RUN_TEST_CASE(ssd1306_i2c, set_display_start_line_too_high);
    RUN_TEST_CASE(ssd1306_i2c, set_display_start_line_ok);
    RUN_TEST_CASE(ssd1306_i2c, set_segment_remap_0);
    RUN_TEST_CASE(ssd1306_i2c, set_segment_remap_127);
    RUN_TEST_CASE(ssd1306_i2c, set_com_output_scan_direction_normal);
    RUN_TEST_CASE(ssd1306_i2c, set_com_output_scan_direction_remapped);
    RUN_TEST_CASE(ssd1306_i2c, set_multiplex_ratio_value_too_low);
    RUN_TEST_CASE(ssd1306_i2c, set_multiplex_ratio_value_too_high);
    RUN_TEST_CASE(ssd1306_i2c, set_multiplex_ratio_value_ok);
    RUN_TEST_CASE(ssd1306_i2c, set_display_offset_too_high);
    RUN_TEST_CASE(ssd1306_i2c, set_display_offset_value_ok);
    RUN_TEST_CASE(ssd1306_i2c, init_display);
}

TEST_SETUP(ssd1306_i2c)
{
    ssd1306_mock_create();
}

TEST_TEAR_DOWN(ssd1306_i2c)
{
    ssd1306_mock_destroy();
}

/*
 * Help functions
 */
void i2c_com_send_bytes(uint16_t number_of_bytes)
{
    uint16_t i;

    for (i=0; i<number_of_bytes; i++)
    {
        ssd1306_mock_com_send_byte();
    }
}

void run_i2c_and_ssd_tasks(uint16_t number_of_ticks)
{
    uint16_t i;

    for (i=0; i<number_of_ticks; i++)
    {
        i2c_com_send_bytes(I2C_TASK_BYTES_PER_TICK);
        ssd1306_run();
    }
}

void prepare_and_call_set_contrast_command(void)
{
    uint16_t bufferlen = 3;
    uint8_t buffer[SSD1306_COMMAND_BUFFER_LEN] = {SSD1306_COMMAND_SINGLE,
                                                  SSD1306_SET_CONTRAST,
                                                  SSD_LOW_CONTRAST};

    ssd1306_mock_expect_i2c_master_write(SSD1306_I2C_SLAVE_ADDRESS, buffer, bufferlen);
    ssd1306_set_contrast(SSD_LOW_CONTRAST);
}

void expect_i2c_command(uint8_t command)
{
    uint16_t bufferlen = 2;
    uint8_t buffer[SSD1306_COMMAND_BUFFER_LEN];

    buffer[0] = SSD1306_COMMAND_SINGLE;
    buffer[1] = command;
    ssd1306_mock_expect_i2c_master_write(SSD1306_I2C_SLAVE_ADDRESS, buffer, bufferlen);
}

void expect_i2c_command_one_arg(uint8_t command, uint8_t argument)
{
    uint16_t bufferlen = 3;
    uint8_t buffer[SSD1306_COMMAND_BUFFER_LEN];

    buffer[0] = SSD1306_COMMAND_SINGLE;
    buffer[1] = command;
    buffer[2] = argument;
    ssd1306_mock_expect_i2c_master_write(SSD1306_I2C_SLAVE_ADDRESS, buffer, bufferlen);
}

/*
 * TEST CASES
 */
TEST(ssd1306_i2c, init)
{
    ssd1306_init(SSD_TASK_ID);
    TEST_ASSERT_EQUAL_INT(ssd1306_idle, ssd1306_get_state());
}

TEST(ssd1306_i2c, get_state_returns_busy_if_i2c_busy)
{
    ssd1306_mock_set_com_busy();
    TEST_ASSERT_EQUAL_INT(ssd1306_busy, ssd1306_get_state());
}

TEST(ssd1306_i2c, call_command_changes_state_to_busy)
{
    prepare_and_call_set_contrast_command();
    TEST_ASSERT_EQUAL_INT(ssd1306_busy, ssd1306_get_state());
}

TEST(ssd1306_i2c, busy_when_not_complete_command_sent_on_i2c)
{
    prepare_and_call_set_contrast_command();
    i2c_com_send_bytes(SSD_CONTRAST_COMMAND_LENGTH-1);

    TEST_ASSERT_EQUAL_INT(ssd1306_busy, ssd1306_get_state());
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, idle_when_complete_command_sent_on_i2c)
{
    prepare_and_call_set_contrast_command();
    i2c_com_send_bytes(SSD_CONTRAST_COMMAND_LENGTH);

    TEST_ASSERT_EQUAL_INT(ssd1306_idle, ssd1306_get_state());
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_contrast_command)
{
    prepare_and_call_set_contrast_command();
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_display_on)
{
    expect_i2c_command(SSD1306_DISPLAY_ON);
    ssd1306_set_display_on();
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_display_off)
{
    expect_i2c_command(SSD1306_DISPLAY_SLEEP);
    ssd1306_set_display_sleep();
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_display_start_line_too_high)
{
    ssd1306_set_display_start_line(SSD_DISPLAY_LINE_VALUE_TOO_HIGH);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_display_start_line_ok)
{
    expect_i2c_command(SSD1306_SET_DISPLAY_START_LINE | SSD_DISPLAY_LINE_VALUE_OK);
    ssd1306_set_display_start_line(SSD_DISPLAY_LINE_VALUE_OK);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_segment_remap_0)
{
    expect_i2c_command(SSD1306_SEGMENT_REMAP_0);
    ssd1306_set_segment_remap_0();
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_segment_remap_127)
{
    expect_i2c_command(SSD1306_SEGMENT_REMAP_127);
    ssd1306_set_segment_remap_127();
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_com_output_scan_direction_normal)
{
    expect_i2c_command(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL);
    ssd1306_set_com_output_scan_direction_normal();
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_com_output_scan_direction_remapped)
{
    expect_i2c_command(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED);
    ssd1306_set_com_output_scan_direction_remapped();
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_multiplex_ratio_value_too_low)
{
    ssd1306_set_multiplex_ratio(SSD_MULTIPLEX_RATIO_VALUE_TOO_LOW);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_multiplex_ratio_value_too_high)
{
    ssd1306_set_multiplex_ratio(SSD_MULTIPLEX_RATIO_VALUE_TOO_HIGH);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_multiplex_ratio_value_ok)
{
    /* The value sent to the device is ratio-1 */
    expect_i2c_command_one_arg(SSD1306_SET_MULTIPLEX_RATIO, SSD_MULTIPLEX_RATIO_VALUE_OK-1);
    ssd1306_set_multiplex_ratio(SSD_MULTIPLEX_RATIO_VALUE_OK);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_display_offset_too_high)
{
    ssd1306_set_display_offset(SSD_DISPLAY_OFFSET_VALUE_TOO_HIGH);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, set_display_offset_value_ok)
{
    expect_i2c_command_one_arg(SSD1306_SET_DISPLAY_OFFSET, SSD_DISPLAY_OFFSET_VALUE_OK);
    ssd1306_set_display_offset(SSD_DISPLAY_OFFSET_VALUE_OK);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, send_graphics_data_over_i2c)
{
    uint16_t bufferlen = 8;
    uint8_t expected_buffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    uint8_t buffer[8];

    memcpy(buffer, expected_buffer, bufferlen);
    ssd1306_mock_expect_i2c_master_write_register(SSD1306_I2C_SLAVE_ADDRESS,
            SSD1306_DATA_STREAM, expected_buffer, bufferlen);
    ssd1306_send_graphics_data(buffer, bufferlen);
    ssd1306_mock_verify_complete();
}

TEST(ssd1306_i2c, init_display)
{
    TEST_IGNORE_MESSAGE ("Test not ready");
    expect_i2c_command_one_arg(SSD1306_SET_MULTIPLEX_RATIO, SSD1306_DEFAULT_MUX_VALUE-1);
    expect_i2c_command_one_arg(SSD1306_SET_DISPLAY_OFFSET, SSD1306_DEFAULT_DISPLAY_OFFSET);
    expect_i2c_command(SSD1306_SET_DISPLAY_START_LINE | SSD1306_DEFAULT_DISPLAY_STARTLINE);
    expect_i2c_command(SSD1306_SEGMENT_REMAP_0);
    expect_i2c_command(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL);
    expect_i2c_command_one_arg(SSD1306_SET_COM_PINS_HARDWARE_CONFIGURATION,
                               SSD1306_DEFAULT_COM_PINS_HARDWARE_CONFIGURATION);
    expect_i2c_command_one_arg(SSD1306_SET_CONTRAST, SSD1306_DEFAULT_CONTRAST);
    expect_i2c_command(SSD1306_DISPLAY_SLEEP);
    expect_i2c_command(SSD1306_SET_NORMAL_DISPLAY);
    expect_i2c_command_one_arg(SSD1306_SET_CLOCK_DIVIDER_AND_OSCILLATOR,
                               SSD1306_DEFAULT_OSCILLATOR_FREQUENCY << 4 |
                               SSD1306_DEFAULT_DISPLAY_CLOCK_DIVIDE_RATIO);
    expect_i2c_command_one_arg(SSD1306_CHARGE_PUMP_SETTING, SSD1306_CHARGE_PUMP_ENABLE);
    expect_i2c_command(SSD1306_DISPLAY_ON);

    ssd1306_init_display();
    run_i2c_and_ssd_tasks(12);
    ssd1306_mock_verify_complete();
}
