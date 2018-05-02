/*
 * Unit tests for the Bit Loom SSD 1306 driver.
 *
 * Copyright (c) 2018. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include "unity_fixture.h"
#include "ssd1306.h"
#include "ssd1306_defines.h"
#include "ssd1306_mock.h"

/*
 * Tests to be written:
 * - Call command without run to check that state is changed
 * - init_chip function
 * - set_update_area
 * - send_data
 * - Make sure that the selected bus (SPI or I2C) is used
 * - Check that busy is returned if the bus is currently used by another
 *   application.
 */

/*
 * Defines for the test cases.
 */
#define SSD_TASK_ID         1
#define SSD_LOW_CONTRAST    0x20
#define SSD_CONTRAST_COMMAND_LENGTH 2

TEST_GROUP(ssd1306);
TEST_GROUP_RUNNER(ssd1306)
{
    RUN_TEST_CASE(ssd1306, init);
    RUN_TEST_CASE(ssd1306, call_command_changes_state_to_busy);
    RUN_TEST_CASE(ssd1306, busy_when_not_complete_command_sent_on_i2c);
    RUN_TEST_CASE(ssd1306, idle_when_complete_command_sent_on_i2c);
    RUN_TEST_CASE(ssd1306, command_set_contrast_using_i2c);
}

TEST_SETUP(ssd1306)
{
    ssd1306_mock_create();
}

TEST_TEAR_DOWN(ssd1306)
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

void prepare_and_call_set_contrast_command(void)
{
    uint8_t buffer[SSD1306_COMMAND_BUFFER_LEN] = {SSD1306_SET_CONTRAST, SSD_LOW_CONTRAST};
    uint16_t bufferlen = 2;

    ssd1306_mock_expect_i2c_master_write(SSD1306_I2C_SLAVE_ADDRESS, buffer, bufferlen);
    ssd1306_set_contrast(SSD_LOW_CONTRAST);
}

/*
 * TEST CASES
 */
TEST(ssd1306, init)
{
    ssd1306_init(SSD_TASK_ID);
    TEST_ASSERT_EQUAL_INT(ssd1306_idle, ssd1306_get_state());
}

TEST(ssd1306, call_command_changes_state_to_busy)
{
    prepare_and_call_set_contrast_command();
    TEST_ASSERT_EQUAL_INT(ssd1306_busy, ssd1306_get_state());
}

TEST(ssd1306, busy_when_not_complete_command_sent_on_i2c)
{
    prepare_and_call_set_contrast_command();
    i2c_com_send_bytes(SSD_CONTRAST_COMMAND_LENGTH-1);

    TEST_ASSERT_EQUAL_INT(ssd1306_busy, ssd1306_get_state());
    ssd1306_mock_verify_complete();
}

TEST(ssd1306, idle_when_complete_command_sent_on_i2c)
{
    prepare_and_call_set_contrast_command();
    i2c_com_send_bytes(SSD_CONTRAST_COMMAND_LENGTH);

    TEST_ASSERT_EQUAL_INT(ssd1306_idle, ssd1306_get_state());
    ssd1306_mock_verify_complete();
}

TEST(ssd1306, command_set_contrast_using_i2c)
{
    prepare_and_call_set_contrast_command();
    ssd1306_mock_verify_complete();
}
