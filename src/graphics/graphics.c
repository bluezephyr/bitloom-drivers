/*
 * Graphics library for small, monochrome displays (max size 255*255 pixels)
 *
 * Copyright (c) 2015-2021. BlueZephyr
 */

#include <ssd1306.h>
#include <framebuffer.h>
#include "graphics.h"

enum graphics_state_t
{
    state_init,
    state_wait_for_show_request,
    state_clear_display,
    state_data_sent
};

/*
 * Internal variables for the graphics library
 */
static struct graphics_t
{
    enum graphics_state_t state;
    enum ssd1306_result_t displayResult;
    bool showRequested;
    bool operationOngoing;
} self;

void graphics_init(uint8_t taskId)
{
    self.state = state_init;
    self.displayResult = ssd1306_result_ok;
    self.showRequested = false;
    self.operationOngoing = false;
}

void graphics_run (void)
{
    uint8_t *dirtyBuffer;
    uint16_t dirtyBufferLen;
    uint8_t dirtyBufferFirstLine;

    if (self.operationOngoing)
    {
        if (self.displayResult == ssd1306_result_processing)
        {
            // Wait until the operation has finished
            return;
        }
        else
        {
            self.operationOngoing = false;
        }
    }

    switch (self.state)
    {
        case state_init:
            if (ssd1306_initDisplay(&self.displayResult) == ssd1306_request_ok)
            {
                ssd1306_setMemoryAddressingMode(ssd1306_addressing_horizontal);
                self.operationOngoing = true;
                self.state = state_clear_display;
            }
            break;
        case state_clear_display:
            // Clear framebuffer and send to display
            dirtyBuffer = framebuffer_getDirtyAreaBuffer(&dirtyBufferFirstLine, &dirtyBufferLen);

            // Set start position
            ssd1306_setPageAddress(0, 7);
            ssd1306_setColumnAddress(0, 127);

            if (ssd1306_sendGraphicsData(dirtyBuffer, dirtyBufferLen, &self.displayResult) == ssd1306_request_ok)
            {
                self.operationOngoing = true;
                self.state = state_wait_for_show_request;
            }
            break;
        case state_wait_for_show_request:
            if (self.showRequested)
            {
                dirtyBuffer = framebuffer_getDirtyAreaBuffer(&dirtyBufferFirstLine, &dirtyBufferLen);

                // Set start position
                ssd1306_setPageAddress(dirtyBufferFirstLine, 7);
                ssd1306_setColumnAddress(0, 127);

                // Send data
                if (ssd1306_sendGraphicsData(dirtyBuffer, dirtyBufferLen, &self.displayResult) == ssd1306_request_ok)
                {
                    self.operationOngoing = true;
                    self.state = state_data_sent;
                }
            }
            break;
        case state_data_sent:
            framebuffer_unlock();
            self.state = state_wait_for_show_request;
            self.showRequested = false;
            break;
    }
}

void graphics_show (void)
{
    framebuffer_lock();
    self.showRequested = true;
}

