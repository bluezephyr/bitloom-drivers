/*
 * Implementation of the i2c mock module for the unit tests.
 * The module implements a mock task that can be controlled by the test cases
 * to verify that the i2c master driver calls the correct i2c operations and
 * that they are called in the correct order.
 *
 * The implementation uses the CppUMock framework
 *
 * Copyright (c) 2020. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include <CppUTestExt/MockSupport.h>

extern "C"
{
    // This module mocks the following interface
    #include "hal/i2c.h"
}

void i2c_init (void)
{
    mock().actualCall("i2c_init");
}

enum i2c_request_t i2c_write_register(uint8_t address, uint8_t reg, uint8_t *buffer,
                                      uint8_t length, enum i2c_op_result_t *result)
{
    return static_cast<i2c_request_t>(mock()
            .actualCall("i2c_write_register")
            .withParameter("address", address)
            .withParameter("reg", reg)
            .withParameter("buffer", buffer)
            .withParameter("length", length)
            .withOutputParameter("result", result)
            .returnUnsignedIntValue());
}

enum i2c_request_t i2c_read_register(uint8_t address, uint8_t read_register, uint8_t *buffer,
                                     uint8_t length, enum i2c_op_result_t *result)
{
    return static_cast<i2c_request_t>(mock()
            .actualCall("i2c_read_register")
            .withParameter("address", address)
            .withParameter("read_register", read_register)
            .withOutputParameter("buffer", buffer)
            .withParameter("length", length)
            .withOutputParameter("result", result)
            .returnUnsignedIntValue());
}

uint8_t i2c_get_error_code(void)
{
    return 0;
}
