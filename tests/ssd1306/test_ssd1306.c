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

/*
 * Tests to be written:
 * - Call command without run to check that state is changed
 * - init_chip function
 * - set_update_area
 * - send_data
 * - Make sure that the selected bus (SPI or I2C) is used
 */

/*
 * Defines for the test cases.
 */
#define SSD_TASK_ID     1

TEST_GROUP(ssd1306);
TEST_GROUP_RUNNER(ssd1306)
{
    RUN_TEST_CASE(ssd1306, init);
    //RUN_TEST_CASE(ssd1306, call_command_changes_state_to_busy);
}

TEST_SETUP(ssd1306)
{
}

TEST_TEAR_DOWN(ssd1306)
{
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

}
