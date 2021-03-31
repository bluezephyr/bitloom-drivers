/*
 * Implementation of the i2c mock module for the unit tests.
 * The module implements a mock task that can be controlled by the test cases
 * to verify that the i2c master driver calls the correct i2c operations and
 * that they are called in the correct order.
 *
 * The implementation uses the CppUMock framework
 *
 * Copyright (c) 2021. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#ifndef BITLOOM_DRIVERS_I2C_MOCK_H
#define BITLOOM_DRIVERS_I2C_MOCK_H

#include "hal/i2c.h"

/*
 * This function is used to update the result of the I2C operation.
 * Needed since the result is stored in a memory address that is updated
 * without being explicitly requested.
 *
 * Note that the function must not be called before the mock
 */
void i2c_mock_updateI2cOpResult(enum i2c_op_result_t i2cOpResult);

#endif //BITLOOM_DRIVERS_I2C_MOCK_H
