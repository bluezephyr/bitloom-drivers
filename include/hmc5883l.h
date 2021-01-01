/*
 * Simple driver for the HMC5883L 3-Axis digital compass IC.
 *
 * Note that the driver does not have its own task. All functions are carried
 * out by calls to the underlying I2C driver. This means that, after each
 * function call, the application has to wait for the driver to become ready
 * (use the hmc_driver_status function) before the next function is called.
 *
 * Datasheet can be found here:
 * http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/HMC5883L_3-Axis_Digital_Compass_IC.pdf
 *
 * Copyright (c) 2016-2020. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#ifndef HMC5883L_H
#define HMC5883L_H

#include "hal/i2c.h"

enum hmc_op_result_t
{
    hmc_operation_ok = 0,
    hmc_operation_processing,
    hmc_operation_error
};

enum hmc_driver_status_t
{
    hmc_status_idle,
    hmc_status_processing,
    hmc_status_error
};

enum hmc_mode_t
{
    hmc_idle_mode,
    hmc_single_mode,
    hmc_continuous_mode
};

struct hmc_measurement_t
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
};

//enum hmc_status_reg_t
//{
//    hmc_status_unknown,
//    hmc_status_ready,   // (RDY)
//    hmc_status_lock     // (LOCK)
//};

// Configuration register A (CRA_REG)
#define HMC_CRA_MS0     0
#define HMC_CRA_MS1     1
#define HMC_CRA_DO0     2
#define HMC_CRA_DO1     3
#define HMC_CRA_DO2     4
#define HMC_CRA_MA0     5
#define HMC_CRA_MA1     6
#define HMC_CRA_CRA7    7

// Configuration register B (CRB_REG)
#define HMC_CRB_CRB0    0
#define HMC_CRB_CRB1    1
#define HMC_CRB_CRB2    2
#define HMC_CRB_CRB3    3
#define HMC_CRB_CRB4    4
#define HMC_CRB_GN0     5
#define HMC_CRB_GN1     6
#define HMC_CRB_GN2     7
/*
 * Initialize the driver. This function must be called once at startup and before
 * any other function in the interface is called.
 */
void hmc_init(uint8_t taskId);

/*
 * Run function for the HMC driver task.  Called by the scheduler.
 */
void hmc_task_run (void);

/*
 * Set configuration registers.  The registers must be set before any measurements
 * are started.  The values of the registers can be found in the datasheet.

 Example values:
    CRA (00) 8-average, 15 Hz default or any other rate, normal measurement
    uint8_t CRA_REG = (1 << HMC_CRA_MA0) | (1 << HMC_CRA_MA1) | (1 << HMC_CRA_DO2)
    uint8_t CRB_REG = (1 << HMC_CRB_GN0) | (1 << HMC_CRB_GN2)
 */
void hmc_set_config_reg_A (uint8_t cra);
void hmc_set_config_reg_B (uint8_t crb);

/*
 * Start a single measurement sequence.  Before the sequence is started, the config
 * registers A and B must be set.  The measurement is executed using calls to the run
 * function (by the scheduler).  The result of the operation will eventually be published
 * in the result out-parameter.  During the measurement sequence, the value is
 * hmc_operation_processing.
 */
void hmc_start_single_measurement(enum hmc_op_result_t *result);
void hmc_get_measurement_data (struct hmc_measurement_t* measurement);

/*
 * Print the state of the HMC driver.
 */
void hmc_print_i2c_status (void);
void hmc_print_status (void);


/*
 * Request the driver to read out the status register of the chip. When the
 * driver is back in ready state, the status value can be read using the
 * hmc_get_status_register function.
 *
 * TODO: Note! Not implemented. Is this info needed outside the driver?
 */
void hmc_status_register_req (void);
//enum hmc_status_reg_t hmc_get_status_register (void);

#endif
