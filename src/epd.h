/*
 * MIT License
 *
 * Copyright (c) 2024 permanentnydebet
 */

#ifndef SRC_EPD_H_
#define SRC_EPD_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "src/epd_font.h"

typedef enum {
    kEpdOrientationNormal = 0,
    kEpdOrientation90Deg,
    kEpdOrientation180Deg,
    kEpdOrientation270Deg,
    kEpdOrientationLast
} EpdOrientation;

typedef enum {
    kEpdColorBlack = 0,
    kEpdColorWhite,
    kEpdColorRed,
    kEpdColorLast,
} EpdColor;

typedef struct {
    uint8_t *image_bw;
    uint8_t *image_red;
    size_t image_size;

    EpdOrientation orientation;

    uint16_t resolution_x;
    uint16_t resolution_y;

    uint16_t rows_count;
    uint16_t columns_count;
} EpdImage;

EpdImage *EPD_NewImage(uint16_t resolution_x, uint16_t resolution_y, EpdOrientation orientation, EpdColor color);
bool EPD_DeleteImage(EpdImage *image);

void EPD_Fill(EpdImage *image, EpdColor color);
void EPD_DrawPoint(EpdImage *image, uint16_t x, uint16_t y, EpdColor color);
void EPD_DrawLine(EpdImage *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, EpdColor color);
void EPD_DrawRectangle(EpdImage *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, EpdColor color, bool is_filled);

void EPD_ShowChar(EpdImage *image, uint16_t x, uint16_t y, const char char_to_show, EpdFont font, EpdColor color);
void EPD_ShowString(EpdImage *image, uint16_t x, uint16_t y, const char *string_to_show, EpdFont font, EpdColor color);
void EPD_ShowPicture(EpdImage *image, uint16_t x, uint16_t y, const uint8_t *picture, uint16_t picture_size_x, uint16_t picture_size_y, EpdColor color);

#endif  // SRC_EPD_H_
