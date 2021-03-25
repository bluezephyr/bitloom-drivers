/*
 * Framebuffer library for small, monochrome displays (max size 255*255 pixels)
 *
 * Config:
 * FRAMEBUFFER_SIZE - Specifies the size of the framebuffer in segments, i.e.,
 *                    the size of the array shall be x_max*y_max/8.
 *
 * Copyright (c) 2015-2021. BlueZephyr
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "config/framebuffer_config.h"

/*
 * Init the framebuffer. This function must be called before any of the
 * framebuffer functions are used.
 */
void framebuffer_init (void);

/*
 * The function will return a pointer to a buffer that contains all the segments
 * that are within the dirty area. The start of the buffer is aligned to whole lines,
 * i.e., the first segment of the line that contains the top-left pixel in the
 * dirty area is the first item in the buffer. The items in the buffer is a sequence
 * of horizontal segments. The yStartSeg is set to the line number of the first
 * segment and the umber of bytes in the buffer is written in the bufferLen output
 * parameter. Note that the last segment in the buffer does not need to be the last
 * segment on a line.
 */
uint8_t* framebuffer_getDirtyAreaBuffer (uint8_t *yStartSeg, uint16_t *bufferLen);

/*
 * Function to lock and unlock the framebuffer.
 */
void framebuffer_lock(void);
void framebuffer_unlock(void);

/*
 * Function to query the framebuffer if a show request has been made. If so,
 * it is not available for modifications. Calls to to functions that modifies
 * the framebuffer when the show request is being processed are not allowed.
 */
bool framebuffer_isLocked (void);

/*
 * Function to check if the framebuffer is "dirty", i.e., it has modifications
 * that have not been sent to the display.
 */
bool framebuffer_isDirty (void);

/*
 * Function to get the actual area to be updated. The values are only valid if
 * the framebuffer is "dirty". Note that the values are in segments.
 */
void framebuffer_getDirtyArea (uint8_t* xStartSeg, uint8_t* xEndSeg,
                               uint8_t* yStartSeg, uint8_t* yEndSeg);

/*
 * Function to extract the data on the framebuffer that shall be sent to the
 * display. Note that only modified parts of the display are considered; other
 * parts are expected to be kept in their current status. Thus, the display
 * driver should not clear the display or modify anything outside the specified
 * coordinates.
 *
 * The caller provides commandBuffer where the output is to be copied. Each byte in
 * the commandBuffer is a 8-bit vertical segment where each bit represents a pixel.
 * The function will fill the commandBuffer with as many bytes as possible. The
 * function returns the number of bytes that have been put in the commandBuffer. It
 * shall be called repeatedly until it returns zero. During this time it is not
 * possible to write new data to the framebuffer.
 *
 * When all updated data in the framebuffer has been sent to the display, the
 * framebuffer is unlocked and available for modifications again.
 */
uint16_t framebuffer_copyDirtyArea (uint8_t* buffer, uint16_t bufferLen);


/*
 * Functions to set or clear a pixel in the framebuffer. The position is
 * specified with the xPos and yPos parameters (in pixels). The functions
 * will update the framebuffer data and modify the "dirty" state of the
 * framebuffer accordingly. The framebuffer_getPixel function will return the state of
 * the specified pixel without making any modifications on the framebuffer.
 */
void framebuffer_setPixel (uint8_t xPos, uint8_t yPos);
void framebuffer_clearPixel (uint8_t xPos, uint8_t yPos);
uint8_t framebuffer_getPixel (uint8_t xPos, uint8_t yPos);


/*
 * Blit function
 *
 * Sizes and position in pixels. Data in segments.
 */
void framebuffer_blit (int8_t x, int8_t y, uint8_t width, uint8_t height, const uint8_t* data);

#endif // FRAMEBUFFER_H
