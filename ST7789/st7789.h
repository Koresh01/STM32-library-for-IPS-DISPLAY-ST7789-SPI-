
#ifndef ST7789_H
#define ST7789_H

#include "stm32f1xx_hal.h"
#include "fonts.h"
#include <stdint.h>

/* Структура экземпляра дисплея */
typedef struct
{
    SPI_HandleTypeDef *spi;

    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    GPIO_TypeDef *dc_port;
    uint16_t dc_pin;

    GPIO_TypeDef *res_port;
    uint16_t res_pin;

    uint16_t width;
    uint16_t height;

} ST7789_HandleTypeDef;


/* Цвета RGB565 */

#define ST7789_BLACK      0x0000
#define ST7789_WHITE      0xFFFF
#define ST7789_RED        0xF800
#define ST7789_GREEN      0x07E0
#define ST7789_BLUE       0x001F
#define ST7789_YELLOW     0xFFE0
#define ST7789_CYAN       0x07FF
#define ST7789_MAGENTA    0xF81F


/* Команды ST7789 */

#define ST7789_SWRESET    0x01
#define ST7789_SLPOUT     0x11
#define ST7789_DISPON     0x29

#define ST7789_CASET      0x2A
#define ST7789_RASET      0x2B
#define ST7789_RAMWR      0x2C

#define ST7789_MADCTL     0x36
#define ST7789_COLMOD     0x3A
#define ST7789_INVON      0x21


/* Инициализация */

void ST7789_Init(ST7789_HandleTypeDef *display);


/* Графические функции */

void ST7789_FillRectangle(ST7789_HandleTypeDef *display,
                          uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height,
                          uint16_t color);

void ST7789_DrawPixel(ST7789_HandleTypeDef *display,
                      uint16_t x, uint16_t y,
                      uint16_t color);

void ST7789_FillScreen(ST7789_HandleTypeDef *display,
                       uint16_t color);

void ST7789_DrawHLine(ST7789_HandleTypeDef *display,
                      uint16_t x, uint16_t y,
                      uint16_t length, uint16_t color);

void ST7789_DrawVLine(ST7789_HandleTypeDef *display,
                      uint16_t x, uint16_t y,
                      uint16_t length, uint16_t color);

void ST7789_DrawLine(ST7789_HandleTypeDef *display,
                     uint16_t x0, uint16_t y0,
                     uint16_t x1, uint16_t y1,
                     uint16_t color);

void ST7789_DrawRectangle(ST7789_HandleTypeDef *display,
                          uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height,
                          uint16_t color);

void ST7789_DrawTriangle(ST7789_HandleTypeDef *display,
                         uint16_t x0, uint16_t y0,
                         uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2,
                         uint16_t color);

void ST7789_DrawCircle(ST7789_HandleTypeDef *display,
                       uint16_t x0, uint16_t y0,
                       uint16_t radius, uint16_t color);


/* Работа со шрифтами */

void ST7789_DrawChar(ST7789_HandleTypeDef *display,
                     uint16_t x, uint16_t y,
                     char c, const FontDef *font,
                     uint16_t color);

void ST7789_DrawString(ST7789_HandleTypeDef *display,
                       uint16_t x, uint16_t y,
                       const char *str,
                       const FontDef *font,
                       uint16_t color);

void ST7789_DrawCharRUS(ST7789_HandleTypeDef *display,
                        uint16_t x, uint16_t y,
                        const char *c,
                        const FontDef *font,
                        uint16_t color);

void ST7789_DrawStringRUS(ST7789_HandleTypeDef *display,
                          uint16_t x, uint16_t y,
                          const char *str,
                          const FontDef *font,
                          uint16_t color);

#endif
