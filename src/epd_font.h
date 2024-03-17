/*
 * MIT License
 *
 * Copyright (c) 2024 permanentnydebet
 */

#ifndef SRC_EPD_FONT_H_
#define SRC_EPD_FONT_H_

#include <stdint.h>

typedef struct {
    const uint8_t *table;
    uint16_t width;
    uint16_t hight;
    uint16_t distance_size;
} EpdFont;

#endif  // SRC_EPD_FONT_H_
