/*
 * Graphics library for small, monochrome displays (max size 255*255 pixels)
 *
 * The graphics library provides a set of functions to draw graphics on a display.
 * It uses a framebuffer to keep a representation of the display contents in RAM.
 * Calls to the graphics functions will manipulate the contents of the framebuffer
 * but the data will not be sent to the display until the 'show' function is called.
 * When the 'show' function is called, this is an indication to the library to
 * start sending the framebuffer data to the display. The sending is done by the
 * run function (called by the scheduler). During the transmission of the bitmap
 * data to the display, the framebuffer is locked for modifications. As soon as
 * the contents has been sent, new data can be put in the framebuffer again.
 *
 * Copyright (c) 2015-2021. BlueZephyr
 */

#ifndef BITLOOM_GRAPHICS_H
#define BITLOOM_GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Init the graphics library. This function must be called before any of the
 * graphics functions can be used.
 */
void graphics_init(uint8_t taskId);

/*
 * Run function for the task. Called by the scheduler.
 */
void graphics_run (void);

/*
 * Function to indicate that the drawing on the framebuffer is finished and
 * that the updated contents shall be sent to the display. The function will
 * lock the framebuffer for further modifications. When all data has been sent,
 * the framebuffer will be unlocked for further modifications.
 */
void graphics_show (void);

#endif //BITLOOM_GRAPHICS_H
