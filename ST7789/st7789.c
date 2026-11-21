
#include "st7789.h"
#include <stdlib.h>

/* =========================================
   НИЗКОУРОВНЕВОЕ УПРАВЛЕНИЕ
   ========================================= */

/* Выбрать дисплей */
static void ST7789_Select(ST7789_HandleTypeDef *display)
{
    HAL_GPIO_WritePin(
        display->cs_port,
        display->cs_pin,
        GPIO_PIN_RESET
    );
}

/* Снять выбор */
static void ST7789_Unselect(ST7789_HandleTypeDef *display)
{
    HAL_GPIO_WritePin(
        display->cs_port,
        display->cs_pin,
        GPIO_PIN_SET
    );
}

/* Режим команды */
static void ST7789_DC_Command(ST7789_HandleTypeDef *display)
{
    HAL_GPIO_WritePin(
        display->dc_port,
        display->dc_pin,
        GPIO_PIN_RESET
    );
}

/* Режим данных */
static void ST7789_DC_Data(ST7789_HandleTypeDef *display)
{
    HAL_GPIO_WritePin(
        display->dc_port,
        display->dc_pin,
        GPIO_PIN_SET
    );
}

/* Аппаратный сброс */
static void ST7789_Reset(ST7789_HandleTypeDef *display)
{
    HAL_GPIO_WritePin(
        display->res_port,
        display->res_pin,
        GPIO_PIN_RESET
    );

    HAL_Delay(20);

    HAL_GPIO_WritePin(
        display->res_port,
        display->res_pin,
        GPIO_PIN_SET
    );

    HAL_Delay(120);
}


/* =========================================
   SPI ПЕРЕДАЧА
   Эти функции НЕ переключают CS!
   ========================================= */

/* Отправить команду */
static void ST7789_WriteCommand(
    ST7789_HandleTypeDef *display,
    uint8_t command)
{
    ST7789_DC_Command(display);

    HAL_SPI_Transmit(
        display->spi,
        &command,
        1,
        HAL_MAX_DELAY
    );
}

/* Отправить данные */
static void ST7789_WriteData(
    ST7789_HandleTypeDef *display,
    uint8_t *data,
    uint16_t size)
{
    if (size == 0)
        return;

    ST7789_DC_Data(display);

    HAL_SPI_Transmit(
        display->spi,
        data,
        size,
        HAL_MAX_DELAY
    );
}


/* =========================================
   УПРАВЛЕНИЕ ОБЛАСТЬЮ ПАМЯТИ
   Вызывается при активном CS!
   ========================================= */

static void ST7789_SetAddressWindow(
    ST7789_HandleTypeDef *display,
    uint16_t x0,
    uint16_t y0,
    uint16_t x1,
    uint16_t y1)
{
    uint8_t data[4];

    /* Область X */
    ST7789_WriteCommand(display, ST7789_CASET);

    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;

    ST7789_WriteData(display, data, 4);

    /* Область Y */
    ST7789_WriteCommand(display, ST7789_RASET);

    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;

    ST7789_WriteData(display, data, 4);

    /* Запись пикселей */
    ST7789_WriteCommand(display, ST7789_RAMWR);
}


/* =========================================
   ИНИЦИАЛИЗАЦИЯ
   ========================================= */

void ST7789_Init(ST7789_HandleTypeDef *display)
{
    if (display == NULL || display->spi == NULL)
        return;

    ST7789_Unselect(display);

    ST7789_Reset(display);

    uint8_t data;

    ST7789_Select(display);

    ST7789_WriteCommand(display, ST7789_SWRESET);
    HAL_Delay(150);

    ST7789_WriteCommand(display, ST7789_SLPOUT);
    HAL_Delay(120);

    ST7789_WriteCommand(display, ST7789_COLMOD);

    data = 0x55;
    ST7789_WriteData(display, &data, 1);
    HAL_Delay(10);

    ST7789_WriteCommand(display, ST7789_MADCTL);

    data = 0x00;
    ST7789_WriteData(display, &data, 1);

    ST7789_WriteCommand(display, ST7789_INVON);
    HAL_Delay(10);

    ST7789_WriteCommand(display, 0x13);
    HAL_Delay(10);

    ST7789_WriteCommand(display, ST7789_DISPON);
    HAL_Delay(20);

    ST7789_Unselect(display);
}


/* =========================================
   РИСОВАНИЕ ПИКСЕЛЯ
   Внутренняя функция.
   Вызывается при активном CS!
   ========================================= */

static void ST7789_DrawPixelRaw(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    uint16_t color)
{
    if (x >= display->width || y >= display->height)
        return;

    uint8_t data[2];

    ST7789_SetAddressWindow(display, x, y, x, y);

    data[0] = color >> 8;
    data[1] = color & 0xFF;

    ST7789_WriteData(display, data, 2);
}


/* =========================================
   ПУБЛИЧНЫЕ ФУНКЦИИ РИСОВАНИЯ
   ========================================= */

void ST7789_DrawPixel(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    uint16_t color)
{
    if (display == NULL)
        return;

    ST7789_Select(display);

    ST7789_DrawPixelRaw(display, x, y, color);

    ST7789_Unselect(display);
}


void ST7789_FillRectangle(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    uint16_t width,
    uint16_t height,
    uint16_t color)
{
    if (display == NULL || width == 0 || height == 0)
        return;

    if (x >= display->width || y >= display->height)
        return;

    if (width > display->width - x)
        width = display->width - x;

    if (height > display->height - y)
        height = display->height - y;

    uint8_t data[2];

    data[0] = color >> 8;
    data[1] = color & 0xFF;

    ST7789_Select(display);

    ST7789_SetAddressWindow(
        display,
        x, y,
        x + width - 1,
        y + height - 1
    );

    ST7789_DC_Data(display);

    uint32_t pixels = (uint32_t)width * height;

    while (pixels--)
    {
        HAL_SPI_Transmit(
            display->spi,
            data,
            2,
            HAL_MAX_DELAY
        );
    }

    ST7789_Unselect(display);
}


void ST7789_FillScreen(
    ST7789_HandleTypeDef *display,
    uint16_t color)
{
    if (display == NULL)
        return;

    ST7789_FillRectangle(
        display,
        0,
        0,
        display->width,
        display->height,
        color
    );
}


/* Горизонтальная линия */

void ST7789_DrawHLine(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    uint16_t length,
    uint16_t color)
{
    ST7789_FillRectangle(
        display, x, y, length, 1, color
    );
}


/* Вертикальная линия */

void ST7789_DrawVLine(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    uint16_t length,
    uint16_t color)
{
    ST7789_FillRectangle(
        display, x, y, 1, length, color
    );
}

// Преобразование Unicode-кода русской буквы
// в индекс символа в массиве шрифта.
static int16_t ST7789_RUS_GetCharIndex(uint16_t code)
{
    // Заглавные А-Е
    if (code >= 0x0410 && code <= 0x0415)
        return code - 0x0410;

    // Заглавная Ё
    if (code == 0x0401)
        return 6;

    // Заглавные Ж-Я
    if (code >= 0x0416 && code <= 0x042F)
        return code - 0x0410 + 1;

    // Строчные а-е
    if (code >= 0x0430 && code <= 0x0435)
        return 33 + (code - 0x0430);

    // Строчная ё
    if (code == 0x0451)
        return 39;

    // Строчные ж-я
    if (code >= 0x0436 && code <= 0x044F)
        return 33 + (code - 0x0430) + 1;

    return -1;
}

static void ST7789_DrawCharRaw(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    char c,
    const FontDef *font,
    uint16_t color)
{
    if (display == NULL || font == NULL)
        return;

    if (c < 32 || c > 126)
        return;

    uint16_t index = (c - 32) * font->Width;

    for (uint8_t column = 0; column < font->Width; column++)
    {
        uint8_t line = font->Data[index + column];

        for (uint8_t row = 0; row < font->Height; row++)
        {
            if (line & (1 << row))
            {
                ST7789_DrawPixelRaw(
                    display,
                    x + column,
                    y + row,
                    color
                );
            }
        }
    }
}

void ST7789_DrawChar(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    char c,
    const FontDef *font,
    uint16_t color)
{
    if (display == NULL || font == NULL)
        return;

    ST7789_Select(display);

    ST7789_DrawCharRaw(display, x, y, c, font, color);

    ST7789_Unselect(display);
}


void ST7789_DrawString(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    const char *str,
    const FontDef *font,
    uint16_t color)
{
    if (display == NULL || str == NULL || font == NULL)
        return;

    ST7789_Select(display);

    while (*str != '\0')
    {
        ST7789_DrawCharRaw(
            display,
            x,
            y,
            *str,
            font,
            color
        );

        x += font->Width + 1;
        str++;
    }

    ST7789_Unselect(display);
}


static void ST7789_DrawCharRUSRaw(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    const char *c,
    const FontDef *font,
    uint16_t color)
{
    if (display == NULL || c == NULL || font == NULL)
        return;

    uint8_t byte1 = (uint8_t)c[0];

    // ASCII: английские буквы, цифры, знаки
    if (byte1 < 0x80)
    {
        ST7789_DrawCharRaw(
            display,
            x,
            y,
            (char)byte1,
            &Font_5x7,
            color
        );

        return;
    }

    // Русские символы UTF-8 начинаются с D0 или D1
    if ((byte1 != 0xD0 && byte1 != 0xD1) || c[1] == '\0')
        return;

    uint8_t byte2 = (uint8_t)c[1];

    if ((byte2 & 0xC0) != 0x80)
        return;

    // Собираем Unicode-код из двух байтов UTF-8
    uint16_t code = 0x0400 |
                    ((byte1 & 0x0F) << 6) |
                    (byte2 & 0x3F);

    // Получаем индекс символа в шрифте
    int16_t index = ST7789_RUS_GetCharIndex(code);

    if (index < 0)
        return;

    uint16_t data_index = index * font->Width;

    // Рисуем символ из массива шрифта
    for (uint8_t column = 0; column < font->Width; column++)
    {
        uint8_t line = font->Data[data_index + column];

        for (uint8_t row = 0; row < font->Height; row++)
        {
            if (line & (1 << row))
            {
                ST7789_DrawPixelRaw(
                    display,
                    x + column,
                    y + row,
                    color
                );
            }
        }
    }
}


void ST7789_DrawCharRUS(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    const char *c,
    const FontDef *font,
    uint16_t color)
{
    if (display == NULL || c == NULL || font == NULL)
        return;

    ST7789_Select(display);

    ST7789_DrawCharRUSRaw(
        display,
        x,
        y,
        c,
        font,
        color
    );

    ST7789_Unselect(display);
}



void ST7789_DrawStringRUS(
    ST7789_HandleTypeDef *display,
    uint16_t x,
    uint16_t y,
    const char *str,
    const FontDef *font,
    uint16_t color)
{
    if (display == NULL || str == NULL || font == NULL)
        return;

    ST7789_Select(display);

    while (*str != '\0')
    {
        uint8_t byte1 = (uint8_t)*str;

        ST7789_DrawCharRUSRaw(
            display,
            x,
            y,
            str,
            font,
            color
        );

        // Русская буква UTF-8 занимает 2 байта
        if (byte1 == 0xD0 || byte1 == 0xD1)
            str += 2;
        else
            str++;

        x += font->Width + 1;
    }

    ST7789_Unselect(display);
}
