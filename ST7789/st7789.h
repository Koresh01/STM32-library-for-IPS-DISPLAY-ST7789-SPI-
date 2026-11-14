#ifndef __ST7789_H
#define __ST7789_H

#include "main.h"
#include "fonts.h"

/* Размер дисплея */

#define ST7789_WIDTH      240
#define ST7789_HEIGHT     240


/* Цвета RGB565 */

#define ST7789_BLACK      0x0000
#define ST7789_WHITE      0xFFFF
#define ST7789_RED        0xF800
#define ST7789_GREEN      0x07E0
#define ST7789_BLUE       0x001F
#define ST7789_YELLOW     0xFFE0
#define ST7789_CYAN       0x07FF
#define ST7789_MAGENTA    0xF81F

/*=============================
        Команды ST7789
==============================*/

#define ST7789_SWRESET    0x01    // Программный сброс
#define ST7789_SLPOUT     0x11    // Выход из режима сна
#define ST7789_DISPON     0x29    // Включить дисплей

#define ST7789_CASET      0x2A    // Задать область по X
#define ST7789_RASET      0x2B    // Задать область по Y
#define ST7789_RAMWR      0x2C    // Начать запись пикселей

#define ST7789_MADCTL     0x36    // Настроить ориентацию изображения
#define ST7789_COLMOD     0x3A    // Задать формат цвета
#define ST7789_INVON      0x21    // Включить инверсию цветов

/*=============================
      Пользовательские функции
==============================*/

void ST7789_Init(void);
void ST7789_FillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_FillScreen(uint16_t color);
void ST7789_DrawHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void ST7789_DrawVLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ST7789_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ST7789_DrawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);

void ST7789_DrawChar(uint16_t x, uint16_t y, char c, const FontDef *font, uint16_t color);
void ST7789_DrawString(uint16_t x, uint16_t y, const char *str, const FontDef *font, uint16_t color);

void ST7789_DrawCharRUS(uint16_t x, uint16_t y, const char *c, const FontDef *font, uint16_t color);
void ST7789_DrawStringRUS(uint16_t x, uint16_t y, const char *str, const FontDef *font, uint16_t color);

#endif
