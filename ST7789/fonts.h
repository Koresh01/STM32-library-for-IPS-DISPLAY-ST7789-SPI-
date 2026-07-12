#ifndef __FONTS_H
#define __FONTS_H

#include "main.h"

typedef struct
{
    uint8_t Width;
    uint8_t Height;
    const uint8_t *Data;
} FontDef;

extern const FontDef Font_5x7;

#endif
