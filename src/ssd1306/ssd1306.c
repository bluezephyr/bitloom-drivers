/*
 * BitLoom driver for the SSD1306 display.
 *
 * This module implements a driver for the SSD1306 display chip.
 *
 * Copyright (c) 2016-2021. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "ssd1306.h"
#include "ssd1306_defines.h"
#include "config/ssd1306_config.h"

/*
 * Com functions - to be moved to separate module
 */
#include "hal/i2c.h"

#define INIT_DELAY_TIME 100u

/*
 * Local function prototypes
 */
static void initDisplay(void);
static void sendData(void);
static void createSingleCommand(uint8_t command, uint8_t length);
static enum ssd1306_request_t createSingleCommandRequest(enum ssd1306_result_t *result);
static void prepareSetMultiPlexRatioCommand(uint8_t ratio);
static void prepareSetDisplayOffset (uint8_t offset);
static void prepareSetContrast(uint8_t level);
static void prepareSetPixelsFromRAM(void);
static void prepareSetPixelsEntireDisplayOn(void);
static void prepareSetNormalDisplay(void);
static void prepareSetInvertedDisplay(void);
static void prepareSetDisplayOn(void);
static void prepareSetDisplaySleep(void);
static void prepareSetAddressingMode(uint8_t mode);
static void prepareSetColumnAddress(void);
static void prepareSetPageAddress(void);
static void prepareSetDisplayStartLine(uint8_t line);
static void prepareSetSegmentRemap_0(void);
static void prepareSetSegmentRemap_127(void);
static void prepareSetComOutputScanDirectionNormal(void);
static void prepareSetComOutputScanDirectionRemapped(void);
static void prepareSetComPinsHardwareConfig(bool useAltComPinConf, bool enableLeftRightRemap);
static void prepareSetDisplayClock(uint8_t divideRatio, uint8_t oscillatorFrequency);
static void prepareEnableChargePump(void);
static void prepareDisableChargePump(void);


/*
 * Current state of the SSD1306 driver
 */
enum ssd1306_state_t
{
    ssd1306_idle_state,
    ssd1306_single_command_state,
    ssd1306_init_display_state,
    ssd1306_send_graphics_data_state
};

enum ssd1306_operation_step_t
{
    ssd1306_none_step,
    ssd1306_init_delay_step,
    ssd1306_init_set_mux_ratio_step,
    ssd1306_init_set_display_offset_step,
    ssd1306_init_set_display_start_line_step,
    ssd1306_init_set_segment_remap_step,
    ssd1306_init_set_com_output_scan_direction_step,
    ssd1306_init_set_com_pins_hardware_configuration_step,
    ssd1306_init_set_contrast_step,
    ssd1306_init_set_use_pixels_from_ram_step,
    ssd1306_init_set_normal_display_step,
    ssd1306_init_set_clock_divider_and_oscillator_step,
    ssd1306_init_charge_pump_setting_step,
    ssd1306_init_set_addressing_mode_step,
    ssd1306_init_display_on_step,
    ssd1306_init_done,
    ssd1306_data_set_col_position_step,
    ssd1306_data_set_page_position_step,
    ssd1306_data_send_graphics_data_step,
    ssd1306_data_send_done
};

enum ssd1306_command_t
{
    no_command,
    single_command,
    send_data_command
};

/*
 * SSD1306 class (singleton)
 */
static struct ssd1306_t
{
    uint8_t taskId;
    enum ssd1306_state_t state;
    enum ssd1306_operation_step_t operationStep;
    enum ssd1306_result_t *operationResult;
    enum ssd1306_command_t commandType;
    bool operationOngoing;
    uint8_t commandBuffer[SSD1306_COMMAND_BUFFER_SIZE];
    uint8_t commandLen;
    uint8_t *graphicsData;
    uint16_t dataLen;
    uint8_t delayTime;
    enum ssd1306_addressing_mode_t addressingMode;
    uint8_t colStart;
    uint8_t colEnd;
    uint8_t pageStart;
    uint8_t pageEnd;
    enum i2c_op_result_t commandResult;
} self;

void ssd1306_init (uint8_t taskId)
{
    self.taskId = taskId;
    self.operationResult = NULL;
    self.state = ssd1306_idle_state;
    self.operationStep = ssd1306_none_step;
    self.commandType = no_command;
    self.operationOngoing = false;
    self.commandResult = i2c_operation_ok;
    self.commandLen = 0;
    self.graphicsData = NULL;
    self.dataLen = 0;
    self.delayTime = 0;
    self.addressingMode = SSD1306_DEFAULT_MEMORY_ADDRESSING_MODE;
    self.colStart = 0;
    self.colEnd = 127;
    self.pageStart = 0;
    self.pageEnd = 7;
}

void ssd1306_run (void)
{
    if (self.operationOngoing)
    {
        // Only I2C at the moment
        if (self.commandResult == i2c_operation_processing)
        {
            // Wait until the operation has finished
            return;
        }
        else
        {
            self.operationOngoing = false;
            self.commandType = no_command;
        }

        if (self.commandResult != i2c_operation_ok)
        {
            // Handle error
        }
    }
    // Check if there is a new command request to handle
    else if (self.commandType == single_command)
    {
        if (i2c_masterTransmit(SSD1306_I2C_SLAVE_ADDRESS, self.commandBuffer,
                               self.commandLen, &self.commandResult) == i2c_request_ok)
        {
            self.operationOngoing = true;
        }
        return;
    }
    else if (self.commandType == send_data_command)
    {
        if (i2c_masterTransmitRegister(SSD1306_I2C_SLAVE_ADDRESS, SSD1306_DATA_SINGLE, self.graphicsData,
                                       self.dataLen, &self.commandResult) == i2c_request_ok)
        {
            self.operationOngoing = true;
        }
        return;
    }

    switch (self.state)
    {
        case ssd1306_idle_state:
            break;
        case ssd1306_init_display_state:
            initDisplay();
            break;
        case ssd1306_single_command_state:
            // Not implemented
            break;
        case ssd1306_send_graphics_data_state:
            sendData();
            break;
    }
}

static void initDisplay(void)
{
    switch(self.operationStep)
    {
        case ssd1306_init_delay_step:
            if (self.delayTime++ == INIT_DELAY_TIME)
            {
                self.operationStep = ssd1306_init_set_mux_ratio_step;
            }
            break;
        case ssd1306_init_set_mux_ratio_step:
            prepareSetMultiPlexRatioCommand(SSD1306_DEFAULT_MUX_VALUE);
            self.operationStep = ssd1306_init_set_display_offset_step;
            break;
        case ssd1306_init_set_display_offset_step:
            prepareSetDisplayOffset(SSD1306_DEFAULT_DISPLAY_OFFSET);
            self.operationStep = ssd1306_init_set_display_start_line_step;
            break;
        case ssd1306_init_set_display_start_line_step:
            prepareSetDisplayStartLine(SSD1306_DEFAULT_DISPLAY_STARTLINE);
            self.operationStep = ssd1306_init_set_segment_remap_step;
            break;
        case ssd1306_init_set_segment_remap_step:
            prepareSetSegmentRemap_127();
            self.operationStep = ssd1306_init_set_com_output_scan_direction_step;
            break;
        case ssd1306_init_set_com_output_scan_direction_step:
            prepareSetComOutputScanDirectionRemapped();
            self.operationStep = ssd1306_init_set_com_pins_hardware_configuration_step;
            break;
        case ssd1306_init_set_com_pins_hardware_configuration_step:
            prepareSetComPinsHardwareConfig(SSD1306_DEFAULT_COM_HW_PIN_USE_ALT_COM_PIN_CONF,
                                            SSD1306_DEFAULT_COM_HW_PIN_EN_LEFT_RIGHT_REMAP);
            self.operationStep = ssd1306_init_set_contrast_step;
            break;
        case ssd1306_init_set_contrast_step:
            prepareSetContrast(SSD1306_DEFAULT_CONTRAST);
            self.operationStep = ssd1306_init_set_use_pixels_from_ram_step;
            break;
        case ssd1306_init_set_use_pixels_from_ram_step:
            prepareSetPixelsFromRAM();
            self.operationStep = ssd1306_init_set_normal_display_step;
            break;
        case ssd1306_init_set_normal_display_step:
            prepareSetNormalDisplay();
            self.operationStep = ssd1306_init_set_clock_divider_and_oscillator_step;
            break;
        case ssd1306_init_set_clock_divider_and_oscillator_step:
            prepareSetDisplayClock(SSD1306_DEFAULT_DISPLAY_CLOCK_DIVIDE_RATIO, SSD1306_DEFAULT_OSCILLATOR_FREQUENCY);
            self.operationStep = ssd1306_init_charge_pump_setting_step;
            break;
        case ssd1306_init_charge_pump_setting_step:
            prepareEnableChargePump();
            self.operationStep = ssd1306_init_set_addressing_mode_step;
            break;
        case ssd1306_init_set_addressing_mode_step:
            prepareSetAddressingMode(self.addressingMode);
            self.operationStep = ssd1306_init_display_on_step;
            break;
        case ssd1306_init_display_on_step:
            prepareSetDisplayOn();
            self.operationStep = ssd1306_init_done;
            break;
        case ssd1306_init_done:
            self.state = ssd1306_idle_state;
            *self.operationResult = ssd1306_result_ok;
            break;
        case ssd1306_none_step:
            // Nothing to do
            break;
        default:
            // Should not happen. Handle error?
            break;
    }
}

static void sendData(void)
{
    switch(self.operationStep)
    {
        case ssd1306_data_set_col_position_step:
            prepareSetColumnAddress();
            self.operationStep = ssd1306_data_set_page_position_step;
            break;
        case ssd1306_data_set_page_position_step:
            prepareSetPageAddress();
            self.operationStep = ssd1306_data_send_graphics_data_step;
            break;
        case ssd1306_data_send_graphics_data_step:
            self.commandType = send_data_command;
            self.operationStep = ssd1306_data_send_done;
            break;
        case ssd1306_data_send_done:
            self.state = ssd1306_idle_state;
            *self.operationResult = ssd1306_result_ok;
            break;
        default:
            // Should not happen. Handle error?
            break;
    }
}

/*
 * Help functions to prepare commands
 */
static void createSingleCommand(uint8_t command, uint8_t length)
{
    self.commandType = single_command;
    self.commandBuffer[0] = SSD1306_COMMAND_SINGLE;
    self.commandBuffer[1] = command;
    self.commandLen = length;
}

static enum ssd1306_request_t createSingleCommandRequest(enum ssd1306_result_t *result)
{
    self.state = ssd1306_single_command_state;
    self.operationResult = result;

    // Check errors and return corresponding error code
    return ssd1306_request_ok;
}

/*
 * Fundamental commands
 */
enum ssd1306_request_t ssd1306_initDisplay(enum ssd1306_result_t *result)
{
    if (self.state == ssd1306_idle_state)
    {
        self.state = ssd1306_init_display_state;
        self.operationStep = ssd1306_init_delay_step;
        self.operationResult = result;
        *self.operationResult = ssd1306_result_processing;
        return ssd1306_request_ok;
    }
    return ssd1306_request_busy;
}

enum ssd1306_request_t ssd1306_setContrast(uint8_t level, enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetContrast(level);
    return createSingleCommandRequest(result);
}

static void prepareSetContrast(uint8_t level)
{
    createSingleCommand(SSD1306_SET_CONTRAST, 3);
    self.commandBuffer[2] = level;
}

enum ssd1306_request_t ssd1306_setPixelsFromRAM(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetPixelsFromRAM();
    return createSingleCommandRequest(result);
}

static void prepareSetPixelsFromRAM(void)
{
    createSingleCommand(SSD1306_SET_USE_PIXELS_FROM_RAM, 2);
}

enum ssd1306_request_t ssd1306_setAllPixelsActive(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetPixelsEntireDisplayOn();
    return createSingleCommandRequest(result);
}

static void prepareSetPixelsEntireDisplayOn(void)
{
    createSingleCommand(SSD1306_SET_PIXELS_ENTIRE_DISPLAY_ON, 2);
}

enum ssd1306_request_t ssd1306_setNormalDisplay(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetNormalDisplay();
    return createSingleCommandRequest(result);
}

static void prepareSetNormalDisplay(void)
{
    createSingleCommand(SSD1306_SET_NORMAL_DISPLAY, 2);
}

enum ssd1306_request_t ssd1306_setInvertedDisplay(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetInvertedDisplay();
    return createSingleCommandRequest(result);
}

static void prepareSetInvertedDisplay(void)
{
    createSingleCommand(SSD1306_SET_INVERTED_DISPLAY, 2);
}

enum ssd1306_request_t ssd1306_setDisplayOn(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetDisplayOn();
    return createSingleCommandRequest(result);
}

static void prepareSetDisplayOn(void)
{
    createSingleCommand(SSD1306_DISPLAY_ON, 2);
}

enum ssd1306_request_t ssd1306_setDisplaySleep(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetDisplaySleep();
    return createSingleCommandRequest(result);
}

static void prepareSetDisplaySleep(void)
{
    createSingleCommand(SSD1306_DISPLAY_SLEEP, 2);
}

/*
 * Scrolling commands
 */

/*
 * Addressing setting commands
 */
void ssd1306_setMemoryAddressingMode(enum ssd1306_addressing_mode_t mode)
{
    if((ssd1306_addressing_horizontal <= mode) && (mode <= ssd1306_addressing_page))
    {
        self.addressingMode = mode;
    }
}

static void prepareSetAddressingMode(uint8_t mode)
{
    createSingleCommand(SSD1306_SET_MEMORY_ADDRESSING_MODE, 3);
    self.commandBuffer[2] = mode;
}

void ssd1306_setColumnAddress(uint8_t startAddress, uint8_t endAddress)
{
    if (startAddress < 128)
    {
        self.colStart = startAddress;
    }
    if (endAddress < 128)
    {
        self.colEnd = endAddress;
    }
}

static void prepareSetColumnAddress(void)
{
    if (self.addressingMode == SSD1306_HORIZONTAL_ADDRESSING_MODE ||
        self.addressingMode == SSD1306_VERTICAL_ADDRESSING_MODE)
    {
        createSingleCommand(SSD1306_SET_COLUMN_ADDRESS, 4);
        self.commandBuffer[2] = self.colStart;
        self.commandBuffer[3] = self.colEnd;
    }
    else
    {
        // Handle page addressing mode
    }
}

void ssd1306_setPageAddress(uint8_t startAddress, uint8_t endAddress)
{
    if (startAddress < 8)
    {
        self.pageStart = startAddress;
    }
    if (endAddress < 8)
    {
        self.pageEnd = endAddress;
    }
}

static void prepareSetPageAddress(void)
{
    if (self.addressingMode == SSD1306_HORIZONTAL_ADDRESSING_MODE ||
        self.addressingMode == SSD1306_VERTICAL_ADDRESSING_MODE)
    {
        createSingleCommand(SSD1306_SET_PAGE_ADDRESS, 4);
        self.commandBuffer[2] = self.pageStart;
        self.commandBuffer[3] = self.pageEnd;
    }
    else
    {
        // Handle page addressing mode
    }
}

/*
 * Hardware configuration commands
 */
enum ssd1306_request_t ssd1306_setDisplayStartLine(uint8_t line, enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetDisplayStartLine(line);
    return createSingleCommandRequest(result);
}

static void prepareSetDisplayStartLine(uint8_t line)
{
    if (line <= SSD1306_DISPLAY_START_LINE_MAX)
    {
        createSingleCommand(SSD1306_SET_DISPLAY_START_LINE | line, 2);
    }
    else
    {
        // Handle error
        // Set error code and let the error handler abort the operation and return to idle
    }
}

enum ssd1306_request_t ssd1306_setSegmentRemap_0(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetSegmentRemap_0();
    return createSingleCommandRequest(result);
}

static void prepareSetSegmentRemap_0(void)
{
    createSingleCommand(SSD1306_SEGMENT_REMAP_0, 2);
}

enum ssd1306_request_t ssd1306_setSegmentRemap_127(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetSegmentRemap_127();
    return createSingleCommandRequest(result);
}

static void prepareSetSegmentRemap_127(void)
{
    createSingleCommand(SSD1306_SEGMENT_REMAP_127, 2);
}

enum ssd1306_request_t ssd1306_setMultiplexRatio(uint8_t ratio, enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetMultiPlexRatioCommand(ratio);
    return createSingleCommandRequest(result);
}

static void prepareSetMultiPlexRatioCommand(uint8_t ratio)
{
    if((ratio>=SSD1306_MUX_MIN_VALUE) && (ratio<=SSD1306_MUX_MAX_VALUE))
    {
        createSingleCommand(SSD1306_SET_MULTIPLEX_RATIO, 3);
        self.commandBuffer[2] = ratio - 1;
    }
    else
    {
        // Handle error
        // Set error code and let the error handler abort the operation and return to idle
    }
}

enum ssd1306_request_t ssd1306_setComOutputScanDirectionNormal(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetComOutputScanDirectionNormal();
    return createSingleCommandRequest(result);
}

static void prepareSetComOutputScanDirectionNormal(void)
{
    createSingleCommand(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL, 2);
}

enum ssd1306_request_t ssd1306_setComOutputScanDirectionRemapped(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetComOutputScanDirectionRemapped();
    return createSingleCommandRequest(result);
}

static void prepareSetComOutputScanDirectionRemapped(void)
{
    createSingleCommand(SSD1306_SET_COM_OUTPUT_SCAN_DIRECTION_REMAPPED, 2);
}

enum ssd1306_request_t ssd1306_setDisplayOffset (uint8_t offset, enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetDisplayOffset(offset);
    return createSingleCommandRequest(result);
}

static void prepareSetDisplayOffset (uint8_t offset)
{
    if (offset <= SSD1306_DISPLAY_OFFSET_MAX_VALUE)
    {
        createSingleCommand(SSD1306_SET_DISPLAY_OFFSET, 3);
        self.commandBuffer[2] = offset;
    }
    else
    {
        // Handle error
        // Set error code and let the error handler abort the operation and return to idle
    }
}

enum ssd1306_request_t ssd1306_setComPinsHardwareConfig(bool useAltComPinConf, bool enableLeftRightRemap, enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetComPinsHardwareConfig(useAltComPinConf, enableLeftRightRemap);
    return createSingleCommandRequest(result);
}

static void prepareSetComPinsHardwareConfig(bool useAltComPinConf, bool enableLeftRightRemap)
{
    createSingleCommand(SSD1306_SET_COM_PINS_HARDWARE_CONFIGURATION, 3);
    self.commandBuffer[2] = SSD1306_COM_PINS_HARDWARE_BASE_VALUE |
                            ((uint8_t)useAltComPinConf) << 4 |
                            ((uint8_t)enableLeftRightRemap) << 5;
}

/*
 * Timing and Driving Scheme Setting commands
 */
enum ssd1306_request_t ssd1306_setDisplayClock(uint8_t divideRatio, uint8_t oscillatorFrequency, enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareSetDisplayClock(divideRatio, oscillatorFrequency);
    return createSingleCommandRequest(result);
}

static void prepareSetDisplayClock(uint8_t divideRatio, uint8_t oscillatorFrequency)
{
    if ((divideRatio >= SSD1306_CLOCK_DIVIDER_MIN_VALUE) &&
        (divideRatio <= SSD1306_CLOCK_DIVIDER_MAX_VALUE) &&
        (oscillatorFrequency <= SSD1306_OSCILLATOR_FREQUENCY_MAX_VALUE))
    {
        createSingleCommand(SSD1306_SET_CLOCK_DIVIDER_AND_OSCILLATOR, 3);
        self.commandBuffer[2] = (divideRatio - 1) | (oscillatorFrequency << 4);
    }
    else
    {
        // Handle error
        // Set error code and let the error handler abort the operation and return to idle
    }
}

/*
 * CHARGE PUMP REGULATOR COMMANDS
 */
enum ssd1306_request_t ssd1306_enableChargePump(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareEnableChargePump();
    return createSingleCommandRequest(result);
}

static void prepareEnableChargePump(void)
{
    createSingleCommand(SSD1306_CHARGE_PUMP_SETTING, 3);
    self.commandBuffer[2] = SSD1306_CHARGE_PUMP_ENABLE;
}

enum ssd1306_request_t ssd1306_disableChargePump(enum ssd1306_result_t *result)
{
    if (self.state != ssd1306_idle_state)
    {
        return ssd1306_request_busy;
    }
    prepareDisableChargePump();
    return createSingleCommandRequest(result);
}

static void prepareDisableChargePump(void)
{
    createSingleCommand(SSD1306_CHARGE_PUMP_SETTING, 3);
    self.commandBuffer[2] = SSD1306_CHARGE_PUMP_DISABLE;
}

/*
 * DATA SEND
 */
enum ssd1306_request_t ssd1306_sendGraphicsData(uint8_t *buffer, uint16_t len, enum ssd1306_result_t *result)
{
    if (self.state == ssd1306_idle_state)
    {
        self.graphicsData = buffer;
        self.dataLen = len;
        self.state = ssd1306_send_graphics_data_state;
        self.operationResult = result;
        self.operationStep = ssd1306_data_set_col_position_step;
        return ssd1306_request_ok;
    }
    return ssd1306_request_busy;
}
