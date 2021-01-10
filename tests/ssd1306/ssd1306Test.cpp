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
    #include "hal/i2c.h"
    #include "i2c_mock.h"
}

/*
 * Defines for the test cases.
 */
#define SSD_TASK_ID                                          1
#define SSD1306_I2C_SLAVE_ADDRESS                         0x78

// First byte after the I2C address
#define SSD1306_COMMAND_STREAM                            0x00
#define SSD1306_COMMAND_SINGLE                            0x80
#define SSD1306_DATA_STREAM                               0x40
#define SSD1306_DATA_SINGLE                               0xC0

#define SSD1306_SET_USE_PIXELS_FROM_RAM                   0xA4

TEST_GROUP(ssd1306_i2c)
{
    // Output parameters
    enum ssd1306_result_t ssd1306Result;
    enum i2c_op_result_t i2cOpResult;
    uint8_t buffer[10];

    void setup() override
    {
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/********************************************************************
 * TEST CASES
 ********************************************************************/
TEST(ssd1306_i2c, init)
{
    ssd1306_init(SSD_TASK_ID);
}

TEST(ssd1306_i2c, set_display_pixels_use_pixel_data_from_RAM)
{
    buffer[0] = SSD1306_SET_USE_PIXELS_FROM_RAM;
    i2cOpResult = i2c_operation_processing;
    mock().expectOneCall("i2c_write_register").
            withParameter("address", SSD1306_I2C_SLAVE_ADDRESS).
            withParameter("reg", SSD1306_COMMAND_SINGLE).
            withParameter("length", 1).
            withMemoryBufferParameter("buffer", buffer, 1).
            withOutputParameterReturning("result", &i2cOpResult, sizeof(i2cOpResult));

    ssd1306_setPixelsFromRAM(&ssd1306Result);
    i2c_mock_setI2cOpResult(i2c_operation_ok);
    ssd1306_run();

    CHECK_EQUAL(ssd1306_result_ok, ssd1306Result);
}

/********************************************************************
 * TEST RUNNER
 ********************************************************************/
int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
