/*
 * Unit tests for the HMC5883l BitLoom driver.
 *
 * Copyright (c) 2020. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>
#include <cstring>

extern "C"
{
    #include "hmc5883l.h"
}

#define HMC5883L_ADDRESS        0x3c
#define HMC5883L_CONFIG_A       0x00
#define HMC5883L_CONFIG_B       0x01
#define HMC5883L_MODE           0x02
#define HMC5883L_DATA_X_MSB     0x03
#define HMC5883L_DATA_X_LSB     0x04
#define HMC5883L_DATA_Z_MSB     0x05
#define HMC5883L_DATA_Z_LSB     0x06
#define HMC5883L_DATA_Y_MSB     0x07
#define HMC5883L_DATA_Y_LSB     0x08
#define HMC5883L_STATUS         0x09
#define HMC5883L_ID_REG_A       0x10
#define HMC5883L_ID_REG_B       0x11

#define MEASUREMENT_BUFFER_SIZE 6u


TEST_GROUP(hmc5883l)
{
    // Variables for output parameters
    enum hmc_op_result_t hmcOpResult;
    enum i2c_op_result_t operation_result_cra;
    enum i2c_op_result_t operation_result_crb;
    enum i2c_op_result_t operation_result_mode;
    uint8_t outputBuffer[MEASUREMENT_BUFFER_SIZE];

    void setup() override
    {
        hmcOpResult = hmc_operation_ok;
        hmc_init(0);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expectConfigARegisterCalls(const int cra)
    {
        operation_result_cra = i2c_operation_ok;

        // TODO: Check also value of CRA in buffer
        hmc_set_config_reg_A(cra);
        mock().expectOneCall("i2c_masterTransmitRegister")
                .withParameter("address", HMC5883L_ADDRESS)
                .withParameter("reg", HMC5883L_CONFIG_A)
                .withParameter("length", 1)
                .withOutputParameterReturning("result", &operation_result_cra, sizeof(operation_result_cra))
                .ignoreOtherParameters()
                .andReturnValue(i2c_request_ok);
    }

    void expectConfigBRegisterCalls(const int crb)
    {
        operation_result_crb = i2c_operation_ok;

        // TODO: Check also value of CRB in buffer
        hmc_set_config_reg_B(crb);
        mock().expectOneCall("i2c_masterTransmitRegister")
                .withParameter("address", HMC5883L_ADDRESS)
                .withParameter("reg", HMC5883L_CONFIG_B)
                .withParameter("length", 1)
                .withOutputParameterReturning("result", &operation_result_crb, sizeof(operation_result_crb))
                .ignoreOtherParameters()
                .andReturnValue(i2c_request_ok);
    }

    void expectModeRequestRegisterCall()
    {
        operation_result_mode = i2c_operation_ok;

        // TODO: Check also value of mode in buffer
        mock().expectOneCall("i2c_masterTransmitRegister")
                .withParameter("address", HMC5883L_ADDRESS)
                .withParameter("reg", HMC5883L_MODE)
                .withParameter("length", 1)
                .withOutputParameterReturning("result", &operation_result_mode, sizeof(operation_result_mode))
                .ignoreOtherParameters()
                .andReturnValue(i2c_request_ok);
    }

    void expectReadMeasurementRegisters(uint8_t output[])
    {
        operation_result_mode = i2c_operation_ok;

        memcpy(outputBuffer, output, sizeof(outputBuffer));
        mock().expectOneCall("i2c_read_register")
                .withParameter("address", HMC5883L_ADDRESS)
                .withParameter("read_register", HMC5883L_DATA_X_MSB)
                .withOutputParameterReturning("buffer", outputBuffer, sizeof(outputBuffer))
                .withParameter("length", MEASUREMENT_BUFFER_SIZE)
                .withOutputParameterReturning("result", &operation_result_mode, sizeof(operation_result_mode))
                .andReturnValue(i2c_request_ok);
    }

    static void executeRunTimes(int times)
    {
        for(int i=0; i<times; i++)
        {
            hmc_task_run();
        }
    }
};

/********************************************************************
 * TEST CASES
 ********************************************************************/
TEST(hmc5883l, driver_in_idle_state_after_init)
{
    // hmc_init function called in the setup
    //CHECK_EQUAL(hmc_status_idle, hmc_driver_status());
}

TEST(hmc5883l, expect_no_action_when_in_idle_state)
{
    hmc_task_run();
}

TEST(hmc5883l, error_if_single_measurement_done_without_configuration)
{
    hmc_start_single_measurement(&hmcOpResult);
//    CHECK_EQUAL(hmc_status_error, hmc_driver_status());
}

TEST(hmc5883l, config_registers_A_sent_over_i2c_at_first_single_measurement)
{
    expectConfigARegisterCalls(1 << HMC_CRA_MA0 | (1 << HMC_CRA_MA1) | (1 << HMC_CRA_DO2));
    // Prepare values for CRB as well (will not be used in this test case)
    hmc_set_config_reg_B((1 << HMC_CRB_GN0) | (1 << HMC_CRB_GN2));

    hmc_start_single_measurement(&hmcOpResult);
    hmc_task_run();
    CHECK_EQUAL(hmc_operation_processing, hmcOpResult);
}

TEST(hmc5883l, config_registers_B_sent_after_A_at_first_single_measurement)
{
    expectConfigARegisterCalls(1 << HMC_CRA_MA0 | (1 << HMC_CRA_MA1) | (1 << HMC_CRA_DO2));
    expectConfigBRegisterCalls(1 << HMC_CRB_GN0 | (1 << HMC_CRB_GN2));

    hmc_start_single_measurement(&hmcOpResult);
    executeRunTimes(3);
    CHECK_EQUAL(hmc_operation_processing, hmcOpResult);
}

TEST(hmc5883l, expect_single_mode_register_request_after_crb_when_single_measurement_started)
{
    expectConfigARegisterCalls((1 << HMC_CRA_MA0) | (1 << HMC_CRA_MA1) | (1 << HMC_CRA_DO2));
    expectConfigBRegisterCalls(1 << HMC_CRB_GN0 | (1 << HMC_CRB_GN2));
    expectModeRequestRegisterCall();

    hmc_start_single_measurement(&hmcOpResult);
    executeRunTimes(5);
    CHECK_EQUAL(hmc_operation_processing, hmcOpResult);
}

TEST(hmc5883l, expect_read_measurement_after_single_mode_request_when_single_measurement_started)
{
    expectConfigARegisterCalls((1 << HMC_CRA_MA0) | (1 << HMC_CRA_MA1) | (1 << HMC_CRA_DO2));
    expectConfigBRegisterCalls(1 << HMC_CRB_GN0 | (1 << HMC_CRB_GN2));
    expectModeRequestRegisterCall();
    expectReadMeasurementRegisters(outputBuffer); // Ignore buffer contents

    hmc_start_single_measurement(&hmcOpResult);
    executeRunTimes(8);
    CHECK_EQUAL(hmc_operation_ok, hmcOpResult);
}

TEST(hmc5883l, read_data_after_single_measurement)
{
    /*
     * Buffer data as sent from the IC
     * Data Output X MSB Register
     * Data Output X LSB Register
     * Data Output Z MSB Register
     * Data Output Z LSB Register
     * Data Output Y MSB Register
     * Data Output Y LSB Register
     */
    struct hmc_measurement_t measurement = {0, 0, 0};
    uint8_t output[MEASUREMENT_BUFFER_SIZE] = {0x1, 0x2, 0x5, 0x6, 0x3, 0x4};
    expectConfigARegisterCalls((1 << HMC_CRA_MA0) | (1 << HMC_CRA_MA1) | (1 << HMC_CRA_DO2));
    expectConfigBRegisterCalls(1 << HMC_CRB_GN0 | (1 << HMC_CRB_GN2));
    expectModeRequestRegisterCall();
    expectReadMeasurementRegisters(output);

    hmc_start_single_measurement(&hmcOpResult);
    executeRunTimes(8);
    hmc_get_measurement_data(&measurement);
    CHECK_EQUAL(0x102, measurement.x);
    CHECK_EQUAL(0x304, measurement.y);
    CHECK_EQUAL(0x506, measurement.z);
}



/********************************************************************
 * TEST RUNNER
 ********************************************************************/
int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
