/*
 * Framebuffer library for small, monochrome displays (max size 255*255 pixels)
 *
 * Copyright (c) 2015-2021. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */

#include "framebuffer.h"


/*
 * Defines
 */
#define POS_UNDEFINED   0xFFFF
#define FRAMEBUFFER_MAX_X (FRAMEBUFFER_X_PIXELS - 1)
#define FRAMEBUFFER_MAX_Y (FRAMEBUFFER_Y_PIXELS - 1)
#define FRAMEBUFFER_MAX_Y_SEG (FRAMEBUFFER_MAX_Y / 8)
#define FRAMEBUFFER_MIN_X 0
#define FRAMEBUFFER_MIN_Y 0

/*
 * Internal variables for the framebuffer.
 */
static struct framebuffer_t
{
    uint8_t dataSegments[FRAMEBUFFER_SIZE];
    uint8_t dirtySegX1;   // Top left segment in the dirty table
    uint8_t dirtySegY1;   // Top left segment in the dirty table
    uint8_t dirtySegX2;   // Bottom right dirty segment
    uint8_t dirtySegY2;   // Bottom right dirty segment
    uint16_t dataPos;    // Next byte to be copied to the display
    uint8_t error;
    bool isLocked;
    bool isDirty;
} self;

/*
 * Local function prototypes
 */
static void updateDirtyArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/*
 * Init function for the framebuffer. The main data element is the
 * data_segments array that holds the actual framebuffer data. The array size
 * is in segments, i.e., the len of the array shall be FRAMEBUFFER_X_PIXELS*y_max/8.
 */
void framebuffer_init (void)
{
    self.dirtySegX1 = 0;
    self.dirtySegY1 = 0;
    self.dirtySegX2 = FRAMEBUFFER_MAX_X;
    self.dirtySegY2 = FRAMEBUFFER_MAX_Y_SEG;
    self.dataPos = POS_UNDEFINED;
    self.error = 0;
    self.isDirty = true;
    self.isLocked = false;

    // Clear the framebuffer
    for (uint16_t i=0; i<FRAMEBUFFER_SIZE; i++)
    {
        self.dataSegments[i] = 0;
    }
}

uint8_t* framebuffer_getDirtyAreaBuffer (uint8_t *yStartSeg, uint16_t *bufferLen)
{
    uint8_t *dirtyBuffer;
    uint16_t lastDirtySegmentPos;
    uint16_t firstDirtySegmentPos;

    // Get line number of first dirty segment
    uint8_t firstDirtyLine = self.dirtySegY1;

    // Calculate the position for the first segment of the line that contains
    // the topmost part of the dirty area.
    firstDirtySegmentPos = firstDirtyLine * FRAMEBUFFER_X_PIXELS;

    // Find the position of the last dirty segment
    lastDirtySegmentPos = self.dirtySegY2 * FRAMEBUFFER_X_PIXELS + self.dirtySegX2;

    // Calculate the length of the dirty buffer including the prepended segments
    *bufferLen = lastDirtySegmentPos - firstDirtySegmentPos + 1;

    // Set the yStartSeg output parameter
    *yStartSeg = firstDirtyLine;

    // Set the buffer pointer to the first segment of the line that contains
    dirtyBuffer = self.dataSegments + firstDirtySegmentPos;

    // Clear dirty area
    self.isDirty = false;

    return dirtyBuffer;
}

bool framebuffer_isLocked (void)
{
    return self.isLocked;
}

bool framebuffer_isDirty(void)
{
    return self.isDirty;
}

void framebuffer_getDirtyArea (uint8_t* xStartSeg, uint8_t* xEndSeg,
                               uint8_t* yStartSeg, uint8_t* yEndSeg)
{
    *xStartSeg = self.dirtySegX1;
    *xEndSeg = self.dirtySegX2;
    *yStartSeg = self.dirtySegY1;
    *yEndSeg = self.dirtySegY2;
}

uint16_t framebuffer_copyDirtyArea (uint8_t* buffer, uint16_t bufferLen)
{
    uint8_t x_len = self.dirtySegX2 - self.dirtySegX1 + 1;
    uint16_t last_dirty_seg_pos = self.dirtySegY2 * FRAMEBUFFER_X_PIXELS + self.dirtySegX2;
    uint16_t copied = 0;

    if(self.isDirty)
    {
        if (self.dataPos == POS_UNDEFINED)
        {
            // Set the position to the first dirty segment
            self.dataPos = self.dirtySegY1*FRAMEBUFFER_X_PIXELS + self.dirtySegX1;
        }

        // Copy as many bytes as possible to the commandBuffer
        for (uint16_t i=0; i < bufferLen; i++)
        {
            if (self.dataPos > FRAMEBUFFER_SIZE)
            {
                self.error = 1;
            }
            buffer[i] = self.dataSegments[self.dataPos];
            copied++;

            // Get the next segment to copy
            if (self.dataPos % FRAMEBUFFER_X_PIXELS == self.dirtySegX2)
            {
                // Next row
                self.dataPos += (FRAMEBUFFER_X_PIXELS - x_len);
            }
            self.dataPos++;

            // Check if the new position is outside the dirty table
            if (self.dataPos > last_dirty_seg_pos)
            {
                // Done
                self.isDirty = 0;
                self.isLocked = 0;
                self.dataPos = POS_UNDEFINED;
                break;
            }
        }
    }

    // Return the number of copied bytes
    return copied;
}

void framebuffer_show(void)
{
    // Only applicable if there is something to update
    if (self.isDirty)
    {
        // Lock the framebuffer for writing
        self.isLocked = 1;
    }
}

/*
 * Update the dirty area. Note that the input coordinates are in segments.
 */
static void updateDirtyArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    if((x2 > FRAMEBUFFER_MAX_X) || (y2 > FRAMEBUFFER_MAX_Y_SEG))
    {
        // Outside the framebuffer
        self.error = 1;
        return;
    }
    if(!self.isDirty)
    {
        self.dirtySegX1 = x1;
        self.dirtySegY1 = y1;
        self.dirtySegX2 = x2;
        self.dirtySegY2 = y2;
    }
    if(self.dirtySegX1 > x1)
        self.dirtySegX1 = x1;
    if(self.dirtySegY1 > y1)
        self.dirtySegY1 = y1;
    if(self.dirtySegX2 < x2)
        self.dirtySegX2 = x2;
    if(self.dirtySegY2 < y2)
        self.dirtySegY2 = y2;

    // Set the fb dirty flag
    self.isDirty = 1;
}

/*
 * Pixel functions
 */
void framebuffer_setPixel(uint8_t xPos, uint8_t yPos)
{
    if (xPos<FRAMEBUFFER_X_PIXELS && yPos<FRAMEBUFFER_Y_PIXELS)
    {
        uint16_t dataPos;
        uint8_t segment_y = yPos / 8;

        // Find the correct segment row
        dataPos = segment_y * FRAMEBUFFER_X_PIXELS;

        // Add the x-position to find the correct position
        dataPos += xPos;

        // Set the pixel and keep the old value for the other pixels
        self.dataSegments[dataPos] = self.dataSegments[dataPos] | (1 << (yPos % 8));

        updateDirtyArea(xPos, segment_y, xPos, segment_y);
    }
}

void framebuffer_clearPixel(uint8_t xPos, uint8_t yPos)
{
    if (xPos<FRAMEBUFFER_X_PIXELS && yPos<FRAMEBUFFER_Y_PIXELS)
    {
        uint16_t data_pos = 0;
        uint8_t segment_y = yPos / 8;

        // Find the correct segment row
        data_pos = segment_y * FRAMEBUFFER_X_PIXELS;

        // Add the x-position to find the correct segment
        data_pos += xPos;

        // Clear the pixel and keep the old value for the other pixels
        self.dataSegments[data_pos] = self.dataSegments[data_pos] & ~(1 << (yPos % 8));

        updateDirtyArea(xPos, segment_y, xPos, segment_y);
    }
}

uint8_t framebuffer_getPixel(uint8_t xPos, uint8_t yPos)
{
    if (xPos<FRAMEBUFFER_X_PIXELS && yPos<FRAMEBUFFER_Y_PIXELS)
    {
        uint16_t data_pos;
        uint8_t segment_y = yPos / 8;

        // Find the correct segment row
        data_pos = segment_y * FRAMEBUFFER_X_PIXELS;

        // Add the x-position to find the correct position
        data_pos += xPos;

        // Return the value of the specified pixel
        return self.dataSegments[data_pos] & (1 << (yPos % 8));
    }
}

void framebuffer_blit (int8_t x, int8_t y, uint8_t width, uint8_t height, const uint8_t* data)
{
    uint8_t i, j;
    uint8_t obj_start_x;
    uint8_t obj_end_x;
    int8_t obj_current_row;
    uint8_t obj_last_row;
    uint8_t fb_start_x;
    uint8_t fb_start_row;
    uint8_t fb_rows;
    uint8_t y_shift;
    uint16_t cur_fb_seg;
    const uint8_t* obj_data_top_row;
    const uint8_t* obj_data_bottom_row;

    // Check if part of the object is outside the framebuffer
    // If so - truncate
    if ((x + width < FRAMEBUFFER_MIN_X) || (x > FRAMEBUFFER_MAX_X) ||
        (y + height < FRAMEBUFFER_MIN_Y) || (y > FRAMEBUFFER_MAX_Y))
    {
        // Completely outside
        return;
    }

    // Calculate the parts of the blit object on the x-axis that are visible.
    // Truncate parts that are outside the framebuffer.
    if (x < FRAMEBUFFER_MIN_X)
    {
        obj_start_x = -x;
        fb_start_x = FRAMEBUFFER_MIN_X;
    }
    else
    {
        obj_start_x = 0;
        fb_start_x = x;
    }

    if (x + width > FRAMEBUFFER_X_PIXELS)
    {
        obj_end_x = FRAMEBUFFER_X_PIXELS - x;
    }
    else
    {
        obj_end_x = width;
    }

    // For the y axis, we have segments of 8 pixels. Therefore, we need to 
    // calculate the shift value. This is the number of bits that the lower row
    // of the object is shifted down on the framebuffer (i.e. left shift). The
    // upper row will be shifted 8-shift bits up (i.e. right shift) to make a
    // complete row.
    if (y >= 0)
    {
        y_shift = y % 8;
    }
    else
    {
        y_shift = 8-(-y % 8);
    }

    // Calculate which segments of the blit object that have parts that are
    // visible. Set the obj_current_row accordingly. Calculate framebuffer
    // start row. Calculate the number of rows on the framebuffer that the
    // blit object will affect. Calculate framebuffer start row.
    if (y >= FRAMEBUFFER_MIN_Y)
    {
        // This is the case where a zero top row is needed
        obj_current_row = -1;

        //  Each row is 8 pixels: divide with 8
        fb_start_row = y >> 3;
    }
    else
    {
        // abs(-y/8)
        obj_current_row = ((uint8_t)(-y)) >> 3;
        fb_start_row = FRAMEBUFFER_MIN_Y;
    }

    if (y + height >= FRAMEBUFFER_Y_PIXELS)
    {
        obj_last_row = (FRAMEBUFFER_Y_PIXELS - y) >> 3;
        fb_rows = ((FRAMEBUFFER_MAX_Y) >> 3) - fb_start_row + 1;
    }
    else
    {
        obj_last_row = height >> 3;
        fb_rows = ((y + height - 1) >> 3) - fb_start_row + 1;
    }

    // Iterate over all visible rows and copy relevant data to the framebuffer.
    for (i=fb_start_row; i<fb_start_row + fb_rows; i++)
    {
        cur_fb_seg = i*FRAMEBUFFER_X_PIXELS;

        if (obj_current_row < 0)
        {
            // Use only bottom row. Shift normally
            for (j=0; j<(obj_end_x-obj_start_x); j++)
            {
                self.dataSegments[cur_fb_seg+fb_start_x+j] |=
                        data[obj_start_x+j] << (y_shift);
            }
        }
        else if (obj_current_row == obj_last_row)
        {
            // Use only top row. Shift normally
            obj_data_top_row = data + obj_current_row * width;

            for (j=0; j<(obj_end_x-obj_start_x); j++)
            {
                self.dataSegments[cur_fb_seg+fb_start_x+j] |=
                        obj_data_top_row[obj_start_x+j] >> (8-y_shift);
            }
        }
        else
        {
            // Use top and bottom rows
            obj_data_top_row = data + obj_current_row * width;
            obj_data_bottom_row = obj_data_top_row + width;

            for (j=0; j<(obj_end_x-obj_start_x); j++)
            {
                self.dataSegments[cur_fb_seg+fb_start_x+j] |=
                    (obj_data_top_row[obj_start_x+j] >> (8-y_shift)) |
                    (obj_data_bottom_row[obj_start_x+j] << (y_shift));
            }
        }

        obj_current_row++;
    }

    updateDirtyArea(fb_start_x, fb_start_row,
                    fb_start_x + obj_end_x - 1, fb_start_row + fb_rows - 1);
}

void framebuffer_lock(void)
{
    self.isLocked = true;
}

void framebuffer_unlock(void)
{
    self.isLocked = false;
}
