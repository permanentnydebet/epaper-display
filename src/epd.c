/*
 * MIT License
 *
 * Copyright (c) 2024 permanentnydebet
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "src/epd.h"

EpdImage *EPD_NewImage(uint16_t resolution_x, uint16_t resolution_y, EpdOrientation orientation, EpdColor color) {
    if (orientation == kEpdOrientationLast || color == kEpdColorLast)
        return NULL;

    EpdImage image = {
        .resolution_x = resolution_x,
        .resolution_y = resolution_y,
        .orientation = orientation
    };

    switch (orientation) {
        case kEpdOrientation90Deg:
        case kEpdOrientation270Deg:
            image.rows_count = resolution_x;
            image.columns_count = (resolution_y + 7) / 8;
            break;
        case kEpdOrientationNormal:
        case kEpdOrientation180Deg:
            image.rows_count = resolution_y;
            image.columns_count = (resolution_x + 7) / 8;
            break;
        default:
            goto error;
    }

    image.image_size = image.rows_count * image.columns_count;

    image.image_bw = calloc(sizeof(uint8_t), image.image_size);
    if (image.image_bw == NULL)
        goto error;

    image.image_red = calloc(sizeof(uint8_t), image.image_size);
    if (image.image_red == NULL)
        goto free_image_bw;

    EpdImage *new_image = malloc(sizeof(EpdImage));
    if (new_image == NULL)
        goto free_image_red;

    memset(image.image_bw, (color & 0x1) * 0xFF, image.image_size);
    memset(image.image_red, ((color >> 1) & 0x1) * 0xFF, image.image_size);

    memcpy(new_image, &image, sizeof(EpdImage));
    return new_image;

free_image_red:
    free(image.image_red);
free_image_bw:
    free(image.image_bw);
error:
    return NULL;
}

bool EPD_DeleteImage(EpdImage *image) {
    if (image == NULL)
        return false;

    free(image->image_bw);
    free(image->image_red);
    free(image);
    return true;
}

void EPD_Fill(EpdImage *image, EpdColor color) {
    if (image == NULL || color == kEpdColorLast)
        return;

    memset(image->image_bw, (color & 0x1) * 0xFF, image->image_size);
    memset(image->image_red, ((color >> 1) & 0x1) * 0xFF, image->image_size);
}

static void SetPixel(EpdImage *image, uint16_t x, uint16_t y, EpdColor color) {
    unsigned idx = 0;
    unsigned offset = 0;

    switch (image->orientation) {
        case kEpdOrientation90Deg:
        case kEpdOrientation270Deg:
            idx = (y >> 3);
            offset = 7 - (y - (idx << 3));
            idx = idx * image->rows_count + x;
            break;
        case kEpdOrientationNormal:
        case kEpdOrientation180Deg:
            idx = (x >> 3) + y * image->columns_count;
            offset = 7 - (x % 8);
            break;
        default:
            return;
    }

    switch (color) {
        case kEpdColorBlack:
          image->image_bw[idx] &= ~(1 << offset);
          image->image_red[idx] &= ~(1 << offset);
          break;

        case kEpdColorWhite:
          image->image_bw[idx] |= (1 << offset);
          image->image_red[idx] &= ~(1 << offset);
          break;

        case kEpdColorRed:
          image->image_red[idx] |= (1 << offset);
          break;

        default:
          break;
    };
}

void EPD_DrawPoint(EpdImage *image, uint16_t x, uint16_t y, EpdColor color) {
    if (color == kEpdColorLast)
        return;
    SetPixel(image, x, y, color);
}

void EPD_DrawLine(EpdImage *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, EpdColor color) {
    uint16_t Xpoint = x1;
    uint16_t Ypoint = y1;
    int32_t dx = (int32_t)x2 - (int32_t)x1 >= 0 ? x2 - x1 : x1 - x2;
    int32_t dy = (int32_t)y2 - (int32_t)y1 <= 0 ? y2 - y1 : y1 - y2;
    int32_t XAddway = x1 < x2 ? 1 : -1;
    int32_t YAddway = y1 < y2 ? 1 : -1;
    int32_t Esp = dx + dy;

    for (;;) {
        EPD_DrawPoint(image, Xpoint, Ypoint, color);
        if (2 * Esp >= dy) {
            if (Xpoint == x2)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }

        if (2 * Esp <= dx) {
            if (Ypoint == y2)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

void EPD_DrawRectangle(EpdImage *image, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, EpdColor color, bool is_filled) {
    if (is_filled) {
        for (uint16_t y = y1; y < y2; y++)
            EPD_DrawLine(image, x1, y, x2, y, color);
    } else {
        EPD_DrawLine(image, x1, y1, x2, y1, color);
        EPD_DrawLine(image, x1, y1, x1, y2, color);
        EPD_DrawLine(image, x2, y2, x2, y1, color);
        EPD_DrawLine(image, x2, y2, x1, y2, color);
    }
}

void EPD_ShowChar(EpdImage *image, uint16_t x, uint16_t y, const char char_to_show, EpdFont font, EpdColor color) {
    uint8_t char_number = (uint8_t)(char_to_show - ' ');
    uint8_t bytes_per_width = font.width / 8 + ((font.width % 8) ? 1 : 0);
    unsigned char_offset = bytes_per_width * font.hight * char_number;

    EPD_ShowPicture(image, x, y, font.table + char_offset, font.width, font.hight, color);
}

void EPD_ShowString(EpdImage *image, uint16_t x, uint16_t y, const char *string_to_show, EpdFont font, EpdColor color) {
    if (string_to_show == NULL)
        return;

    for (unsigned i = 0; i < strlen(string_to_show); i++) {
        EPD_ShowChar(image, x, y, string_to_show[i], font, color);
        x += font.distance_size;
    }
}

void EPD_ShowPicture(EpdImage *image, uint16_t x, uint16_t y, const uint8_t *picture, uint16_t picture_size_x, uint16_t picture_size_y, EpdColor color) {
    uint16_t start_x = x;
    uint16_t batch_number = 0;

    picture_size_x = picture_size_x / 8 + ((picture_size_x % 8) ? 1 : 0);

    for (uint16_t picture_y = 0; picture_y < picture_size_y; picture_y++) {
        for (uint16_t picture_x = 0; picture_x < picture_size_x; picture_x++) {
            uint8_t batch_of_pixels = picture[batch_number];

            for (uint8_t pixel = 8; pixel > 0; pixel--) {
                bool is_pixel_on = batch_of_pixels & (1 << (pixel - 1));
                if (x >= image->resolution_x || y >= image->resolution_y)
                    break;

                if (is_pixel_on)
                    EPD_DrawPoint(image, x, y, color);
                x++;
            }
            batch_number++;
        }
        x = start_x;
        y++;
    }
}
