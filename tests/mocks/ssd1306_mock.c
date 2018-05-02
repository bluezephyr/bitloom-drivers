/*
 * Implementation of the SSD 1306 mock module for the unit tests.
 * The module implements a mock task that can be controlled by the test cases
 * to verify that the SSD 1306 driver calls the correct underlying operations and
 * that they are called in the correct order.
 *
 * Copyright (c) 2018. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ssd1306_mock.h"
#include "unity_fixture.h"
#include "i2c_master.h"

#define MAX_EXPECTATIONS 100
#define MAX_DATA_LEN 100

typedef enum
{
    i2c_write_op
} operation_t;

typedef struct
{
    operation_t operation;
    uint8_t data[MAX_DATA_LEN];
    uint16_t datalen;
} action_t;

/*
 * Mock class to hold state and expected operations (singleton)
 */
typedef struct
{
    action_t* expectations;
    uint16_t stored_expectations;
    uint16_t used_expectations;
    action_t current_action;
    uint16_t remaining_bytes_to_send;
} mock_t;

/*
 * Mock object
 */
static mock_t self;

/*
 * Local function prototypes.
 */
static void store_expectation(operation_t op, uint8_t* data, uint16_t len);
static void store_current_action(operation_t op, uint8_t* data, uint16_t len);
static void fail_when_operation_is_not_expected(void);
static void fail_when_datalen_is_not_correct(void);
static void fail_when_data_is_not_correct(void);
static void fail_when(int condition, const char* message);
static int too_many_operations(void);
static int not_all_operations_used(void);
static void get_data_hexstring(char* message, uint8_t* data, uint16_t datalen);
static const char* get_string(operation_t operation);

/*
 * Strings
 */
static const char* string_operation_i2c_write      = "I2C write";
//static const char* string_expected                 = "Expected";
//static const char* string_actual                   = "Actual";
static const char* string_operation_unknown        = "unknown string";
static const char* string_too_many_operations      = "Too many operations";
static const char* string_not_all_operations_used  = "Not all operations used";

/*
 * Functions to control the mock.
 */
void ssd1306_mock_create(void)
{
    self.expectations = calloc(MAX_EXPECTATIONS, sizeof(action_t));
    self.stored_expectations = 0;
    self.used_expectations = 0;
    self.current_action = (action_t) {0};
    self.remaining_bytes_to_send = 0;
}

void ssd1306_mock_destroy(void)
{
    if (self.expectations)
    {
        free(self.expectations);
    }
    self.expectations = NULL;
}

void ssd1306_mock_verify_complete(void)
{
    fail_when(not_all_operations_used(), string_not_all_operations_used);
}

void ssd1306_mock_expect_i2c_master_write(uint8_t address, uint8_t* data, uint16_t len)
{
    store_expectation(i2c_write_op, data, len);
}

static void store_expectation(operation_t op, uint8_t* data, uint16_t len)
{
    action_t expectation;

    expectation.operation = op;
    memcpy(expectation.data, data, len);
    expectation.datalen = len;
    self.expectations[self.stored_expectations] = expectation;
    self.stored_expectations++;
}

/*
 * Implementation of the mock.
 */
void i2c_master_write (uint8_t address, uint8_t* buffer, uint16_t len)
{
    fail_when(too_many_operations(), string_too_many_operations);
    store_current_action(i2c_write_op, buffer, len);
    fail_when_operation_is_not_expected();
    fail_when_datalen_is_not_correct();
    fail_when_data_is_not_correct();
    self.used_expectations++;
}

i2c_master_state_t i2c_master_get_state (void)
{
    if (self.remaining_bytes_to_send == 0)
    {
        return i2c_idle;
    }
    else
    {
        return i2c_busy;
    }
}


/*
 * Help functions
 */
static void store_current_action(operation_t op, uint8_t* data, uint16_t len)
{
    self.current_action.operation = op;
    memcpy(self.current_action.data, data, len);
    self.current_action.datalen = len;
    self.remaining_bytes_to_send += len;
}

static void fail_when(int condition, const char* message)
{
    if (condition)
    {
        TEST_FAIL_MESSAGE(message);
    }
}

static void fail_when_operation_is_not_expected(void)
{
    if (self.current_action.operation != self.expectations[self.used_expectations].operation)
    {
        char message[100];
        const char* expected;
        const char* actual;

        actual = get_string(self.current_action.operation);
        expected = get_string(self.expectations[self.used_expectations].operation);
        sprintf(message, "Operation failure in step %i. Expected: %s, Actual: %s",
                self.used_expectations+1, expected, actual);
        TEST_FAIL_MESSAGE(message);
    }
}

static void fail_when_datalen_is_not_correct(void)
{
    char message[100];
    uint16_t expected;
    uint16_t actual;

    if (self.current_action.datalen != self.expectations[self.used_expectations].datalen)
    {
        actual = self.current_action.datalen;
        expected = self.expectations[self.used_expectations].datalen;
        sprintf(message, "Operation failure in step %i. datalen: Expected: %i, Actual: %i",
                self.used_expectations+1, expected, actual);
        TEST_FAIL_MESSAGE(message);
    }
}

static void fail_when_data_is_not_correct(void)
{
    char message[100+2*MAX_DATA_LEN];
    char expected[MAX_DATA_LEN];
    char actual[MAX_DATA_LEN];

    if (memcmp(self.current_action.data, self.expectations[self.used_expectations].data,
               self.current_action.datalen) != 0)
    {
        get_data_hexstring(expected, self.expectations[self.used_expectations].data,
                           self.expectations[self.used_expectations].datalen);
        get_data_hexstring(actual, self.current_action.data, self.current_action.datalen);
        sprintf(message, "Operation failure in step %i. Data mismatch. Expected: [%s] Actual: [%s]",
                self.used_expectations+1, expected, actual);
        TEST_FAIL_MESSAGE(message);
    }
}

static void get_data_hexstring(char* output_string, uint8_t* data, uint16_t datalen)
{
    uint16_t i;

    for(i=0; i<datalen; i++)
    {
        sprintf(output_string, "%02x", data[i]);
        output_string+=2;
    }
}

static const char* get_string(operation_t operation)
{
    switch (operation)
    {
        case i2c_write_op:
            return string_operation_i2c_write;
    }
    return string_operation_unknown;
}

static int too_many_operations(void)
{
    return (self.used_expectations == self.stored_expectations);
}

static int not_all_operations_used(void)
{
    return (self.used_expectations < self.stored_expectations);
}

/*
 * Com functions
 */
void ssd1306_mock_com_send_byte(void)
{
    self.remaining_bytes_to_send--;
}
