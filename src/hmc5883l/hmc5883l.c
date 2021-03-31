/*
 * Simple driver for the HMC5883L 3-Axis digital compass IC.
 *
 * Copyright (c) 2016-2020. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include "hmc5883l.h"
#include "hal/i2c.h"

#ifdef DEBUG
#include <core/uart.h>
#endif // DEBUG

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
#define HMC5883L_ID_REG_C       0x12


// Mode register
#define MD0     0
#define MD1     1
#define HS      7

// Status register
#define RDY     0
#define LOCK    1
#define SR2     2
#define SR3     3
#define SR4     4
#define SR5     5
#define SR6     6
#define SR7     7

// At least 6 bytes to hold data output
#define HMC_I2C_BUFFER_LEN 6

// UART Error buffer
#define OUTPUT_LEN 13

// Bitfield for the configurations
#define CONFIG_CRA_REQUESTED    0
#define CONFIG_CRA_SET          1
#define CONFIG_CRB_REQUESTED    2
#define CONFIG_CRB_SET          3

enum hmc_state_t
{
    hmc_state_idle,
    hmc_state_send_cra,
    hmc_state_send_crb,
    hmc_state_wait_send_cra_done,
    hmc_state_wait_send_crb_done,
    hmc_state_send_single_measurement_mode,
    hmc_state_wait_send_single_measurement_mode,
    hmc_state_read_measurement_registers,
    hmc_state_wait_read_measurement_registers,
};

/*
 * Internal variables for the driver
 */
static struct hmc_5883l_t
{
    uint8_t taskId;
    uint8_t cra;
    uint8_t crb;
    uint8_t configurationStatus;
    enum hmc_state_t state;
    enum hmc_op_result_t *opResult;
    enum hmc_driver_status_t driverStatus; // Really needed?
    enum i2c_op_result_t i2cResult;
    uint8_t i2c_buffer[HMC_I2C_BUFFER_LEN];
} self;

/*
 * Public functions
 */
void hmc_init(uint8_t taskId)
{
    self.taskId = taskId;
    self.cra = 0;
    self.crb = 0;
    self.configurationStatus = 0;
    self.state = hmc_state_idle;
    self.driverStatus = hmc_status_idle;
    self.i2cResult = i2c_operation_ok;
}

void hmc_task_run(void)
{
    switch (self.state)
    {
        case hmc_state_idle:
            // Nothing to do - return immediately
            return;
        case hmc_state_send_cra:
            self.i2c_buffer[0] = self.cra;
            if (i2c_masterTransmitRegister(HMC5883L_ADDRESS, HMC5883L_CONFIG_A, self.i2c_buffer,
                                           1, &self.i2cResult) == i2c_request_ok)
            {
                self.state = hmc_state_wait_send_cra_done;
            }
            break;
        case hmc_state_wait_send_cra_done:
            if (self.i2cResult == i2c_operation_ok)
            {
                self.state = hmc_state_send_crb;
            }
            break;
        case hmc_state_send_crb:
            self.i2c_buffer[0] = self.crb;
            if (i2c_masterTransmitRegister(HMC5883L_ADDRESS, HMC5883L_CONFIG_B, self.i2c_buffer,
                                           1, &self.i2cResult) == i2c_request_ok)
            {
                self.state = hmc_state_wait_send_crb_done;
            }
            break;
        case hmc_state_wait_send_crb_done:
            if (self.i2cResult == i2c_operation_ok)
            {
                self.state = hmc_state_send_single_measurement_mode;
            }
            break;
        case hmc_state_send_single_measurement_mode:
            self.i2c_buffer[0] = (1 << MD1);
            if (i2c_masterTransmitRegister(HMC5883L_ADDRESS, HMC5883L_MODE, self.i2c_buffer,
                                           1, &self.i2cResult) == i2c_request_ok)
            {
                self.state = hmc_state_wait_send_single_measurement_mode;
            }
            break;
        case hmc_state_wait_send_single_measurement_mode:
            if (self.i2cResult == i2c_operation_ok)
            {
                self.state = hmc_state_read_measurement_registers;
            }
            break;
        case hmc_state_read_measurement_registers:
            if (i2c_read_register(HMC5883L_ADDRESS, HMC5883L_DATA_X_MSB, self.i2c_buffer,
                                  HMC_I2C_BUFFER_LEN, &self.i2cResult) == i2c_request_ok)
            {
                self.state = hmc_state_wait_read_measurement_registers;
            }
            break;
        case hmc_state_wait_read_measurement_registers:
            if (self.i2cResult == i2c_operation_ok)
            {
                // Measurement finished
                self.state = hmc_state_idle;
                *self.opResult = hmc_operation_ok;
            }
            break;
    }

    if (self.i2cResult > i2c_operation_processing)
    {
        *self.opResult = hmc_operation_error;
        self.state = hmc_state_idle;
    }
}

void hmc_start_single_measurement(enum hmc_op_result_t *result)
{
    self.opResult = result;
    if (self.configurationStatus == (1 << CONFIG_CRA_REQUESTED | 1 << CONFIG_CRB_REQUESTED))
    {
        if (!(self.configurationStatus & (1 << CONFIG_CRA_SET)))
        {
            // CRA is not set
            self.state = hmc_state_send_cra;
        }
        else if (!(self.configurationStatus & (1 << CONFIG_CRB_SET)))
        {
            // CRB is not set
            self.state = hmc_state_send_crb;
        }
        else
        {
            // Configuration done already - start measuring
            self.state = hmc_state_send_single_measurement_mode;
        }
    }
    else
    {
        self.driverStatus = hmc_status_error;
    }
    *self.opResult = hmc_operation_processing;
}

void hmc_get_measurement_data (struct hmc_measurement_t* measurement)
{
    uint16_t value = 0;

    value = self.i2c_buffer[0] << 8;
    value |= self.i2c_buffer[1];
    measurement->x = value;

    value = self.i2c_buffer[2] << 8;
    value |= self.i2c_buffer[3];
    measurement->z = value;

    value = self.i2c_buffer[4] << 8;
    value |= self.i2c_buffer[5];
    measurement->y = value;
}

void hmc_set_config_reg_A (uint8_t cra)
{
    self.cra = cra;
    self.configurationStatus |= (1 << CONFIG_CRA_REQUESTED);
    self.configurationStatus &= ~(1 << CONFIG_CRA_SET);
}

void hmc_set_config_reg_B (uint8_t crb)
{
    self.crb = crb;
    self.configurationStatus |= (1 << CONFIG_CRB_REQUESTED);
    self.configurationStatus &= ~(1 << CONFIG_CRB_SET);
}

#ifdef DEBUG
void hmc_print_status (void)
{
    char data_values[OUTPUT_LEN] = "            ";

    switch (self.state)
    {
        case hmc_state_idle:
            memcpy(data_values, "HMC IDLE    ", OUTPUT_LEN);
            break;
        case hmc_state_send_cra:
            memcpy(data_values, "SEND CRA    ", OUTPUT_LEN);
            break;
        case hmc_state_send_crb:
            memcpy(data_values, "SEND CRB    ", OUTPUT_LEN);
            break;
        case hmc_state_wait_send_cra_done:
            memcpy(data_values, "WAIT CRA    ", OUTPUT_LEN);
            break;
        case hmc_state_wait_send_crb_done:
            memcpy(data_values, "WAIT CRB    ", OUTPUT_LEN);
            break;
        case hmc_state_send_single_measurement_mode:
            memcpy(data_values, "SEND SINGLE ", OUTPUT_LEN);
            break;
        case hmc_state_wait_send_single_measurement_mode:
            memcpy(data_values, "WAIT SINGLE ", OUTPUT_LEN);
            break;
        case hmc_state_read_measurement_registers:
            memcpy(data_values, "READ MEASUR ", OUTPUT_LEN);
            break;
        case hmc_state_wait_read_measurement_registers:
            memcpy(data_values, "WAIT MEASUR ", OUTPUT_LEN);
            break;
    }
    uart_write((uint8_t*)data_values, OUTPUT_LEN);
    hmc_print_i2c_status();
}

void hmc_print_i2c_status (void)
{
    char data_values[OUTPUT_LEN] = "            ";

    switch (self.i2cResult)
    {
        case i2c_operation_ok:
            memcpy(data_values, "I2C OP OK   ", OUTPUT_LEN);
            break;
        case i2c_operation_processing:
            memcpy(data_values, "PROCESSING  ", OUTPUT_LEN);
            break;
        case i2c_operation_start_error:
            memcpy(data_values, "START ERR   ", OUTPUT_LEN);
            break;
        case i2c_operation_repeated_start_error:
            memcpy(data_values, "REP START E ", OUTPUT_LEN);
            break;
        case i2c_operation_sla_error:
            memcpy(data_values, "SLA ERR     ", OUTPUT_LEN);
            break;
        case i2c_operation_write_error:
            memcpy(data_values, "WRITE ERR   ", OUTPUT_LEN);
            break;
        case i2c_operation_read_error:
            memcpy(data_values, "READ ERR    ", OUTPUT_LEN);
            break;
        case i2c_operation_bus_error:
            memcpy(data_values, "BUS ERR     ", OUTPUT_LEN);
            break;
        case i2c_operation_error:
            memcpy(data_values, "OP ERR      ", OUTPUT_LEN);
            break;
    }
    uart_write((uint8_t*)data_values, OUTPUT_LEN);
}
#endif // DEBUG

void hmc_status_register_req (void)
{
    // TODO: Implement
}

//enum hmc_status_reg_t hmc_get_status_register (void)
//{
//    // TODO: Implement
//    return hmc_status_unknown;
//}
