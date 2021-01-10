/*
 * Unit tests for the SSD1306 BitLoom driver.
 *
 * Copyright (c) 2021. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

extern "C"
{
    #include "ssd1306.h"
    #include "ssd1306_defines.h"
    #include "hal/i2c.h"
    #include "i2c_mock.h"
}

/*
 * Defines for the test cases.
 */
#define SSD_TASK_ID                                          1


TEST_GROUP(ssd1306_i2c)
{
    // Output parameters
    enum ssd1306_result_t ssd1306Result;
    enum ssd1306_result_t ssd1306SecondRequestResult;
    enum i2c_op_result_t i2cOpResult;
    uint8_t buffer[10];

    void setup() override
    {
        ssd1306_init(SSD_TASK_ID);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expect_i2c_command_with_no_args (uint8_t command)
    {
        buffer[0] = command;
        i2cOpResult = i2c_operation_processing;
        mock().expectOneCall("i2c_write_register").
                withParameter("address", SSD1306_I2C_SLAVE_ADDRESS).
                withParameter("reg", SSD1306_COMMAND_SINGLE).
                withParameter("length", 1).
                withMemoryBufferParameter("buffer", buffer, 1).
                withOutputParameterReturning("result", &i2cOpResult, sizeof(i2cOpResult));
    }

    void process_and_check_ssd1306_result_ok ()
    {
        i2c_mock_setI2cOpResult(i2c_operation_ok);
        ssd1306_run();
        CHECK_EQUAL(ssd1306_result_ok, ssd1306Result);
    }
};

/********************************************************************
 * TEST CASES
 ********************************************************************/
TEST(ssd1306_i2c, set_display_pixels_entire_display_on)
{
    expect_i2c_command_with_no_args(SSD1306_SET_PIXELS_ENTIRE_DISPLAY_ON);
    ssd1306_setAllPixelsActive(&ssd1306Result);
    process_and_check_ssd1306_result_ok();
}

TEST(ssd1306_i2c, set_display_pixels_use_pixel_data_from_RAM)
{
    expect_i2c_command_with_no_args(SSD1306_SET_USE_PIXELS_FROM_RAM);
    ssd1306_setPixelsFromRAM(&ssd1306Result);
    process_and_check_ssd1306_result_ok();
}

TEST(ssd1306_i2c, request_when_driver_is_processing_returns_busy)
{
    expect_i2c_command_with_no_args(SSD1306_SET_USE_PIXELS_FROM_RAM);
    ssd1306_setPixelsFromRAM(&ssd1306Result);
    ssd1306_setAllPixelsActive(&ssd1306SecondRequestResult);
    CHECK_EQUAL(ssd1306_result_busy, ssd1306SecondRequestResult);
}

/********************************************************************
 * TEST RUNNER
 ********************************************************************/
int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
