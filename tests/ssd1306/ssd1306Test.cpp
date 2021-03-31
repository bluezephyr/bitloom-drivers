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
    enum ssd1306_result_t ssd1306OpResult;
    enum ssd1306_request_t ssd1306RequestResult;
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

    void expectI2CCommandWithNoArgs(uint8_t command, enum i2c_request_t returnValue)
    {
        buffer[0] = SSD1306_COMMAND_SINGLE;
        buffer[1] = command;
        i2cOpResult = i2c_operation_processing;
        mock().expectOneCall("i2c_masterTransmit").
                withParameter("address", SSD1306_I2C_SLAVE_ADDRESS).
                withParameter("length", 2).
                withMemoryBufferParameter("buffer", buffer, 2).
                withOutputParameterReturning("result", &i2cOpResult, sizeof(i2cOpResult)).
                andReturnValue(returnValue);
    }

    void processAndCheckSSD1306ResultOk()
    {
        i2c_mock_updateI2cOpResult(i2c_operation_ok);
        ssd1306_run();
        CHECK_EQUAL(ssd1306_result_ok, ssd1306OpResult);
    }
};

/********************************************************************
 * TEST CASES
 ********************************************************************/
TEST(ssd1306_i2c, set_display_pixels_entire_display_on)
{
    expectI2CCommandWithNoArgs(SSD1306_SET_PIXELS_ENTIRE_DISPLAY_ON, i2c_request_ok);
    ssd1306_setAllPixelsActive(&ssd1306OpResult);
    processAndCheckSSD1306ResultOk();
}

TEST(ssd1306_i2c, set_display_pixels_use_pixel_data_from_RAM)
{
    expectI2CCommandWithNoArgs(SSD1306_SET_USE_PIXELS_FROM_RAM, i2c_request_ok);
    ssd1306_setPixelsFromRAM(&ssd1306OpResult);
    processAndCheckSSD1306ResultOk();
}

TEST(ssd1306_i2c, request_when_driver_is_processing_returns_busy)
{
    expectI2CCommandWithNoArgs(SSD1306_SET_USE_PIXELS_FROM_RAM, i2c_request_ok);
    (void)ssd1306_setPixelsFromRAM(&ssd1306OpResult);
    ssd1306_run();
    CHECK_EQUAL(ssd1306_request_busy, ssd1306_setAllPixelsActive(&ssd1306OpResult));
}

/********************************************************************
 * TEST RUNNER
 ********************************************************************/
int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
