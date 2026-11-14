#include "st7789.h"
#include <stdlib.h>

extern SPI_HandleTypeDef hspi1;

/* Аппаратный сброс дисплея */
static void ST7789_Reset(void) {
	HAL_GPIO_WritePin(RES_GPIO_Port, RES_Pin, GPIO_PIN_RESET);

	HAL_Delay(20);

	HAL_GPIO_WritePin(RES_GPIO_Port, RES_Pin, GPIO_PIN_SET);

	HAL_Delay(120);
}

/* Переключить DC в режим команды */
static void ST7789_DC_Command(void) {
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
}

/* Переключить DC в режим данных */
static void ST7789_DC_Data(void) {
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
}

/* Отправить команду */
static void ST7789_WriteCommand(uint8_t command) {
	ST7789_DC_Command();

	HAL_SPI_Transmit(&hspi1, &command, 1, HAL_MAX_DELAY);
}

/* Отправить данные */
static void ST7789_WriteData(uint8_t *data, uint16_t size) {
	ST7789_DC_Data();

	HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
}

/* Задать область рисования */
static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1,
		uint16_t y1) {
	uint8_t data[4];

	// Задать область по X
	ST7789_WriteCommand(ST7789_CASET);

	data[0] = x0 >> 8;
	data[1] = x0 & 0xFF;
	data[2] = x1 >> 8;
	data[3] = x1 & 0xFF;

	ST7789_WriteData(data, 4);

	// Задать область по Y
	ST7789_WriteCommand(ST7789_RASET);

	data[0] = y0 >> 8;
	data[1] = y0 & 0xFF;
	data[2] = y1 >> 8;
	data[3] = y1 & 0xFF;

	ST7789_WriteData(data, 4);

	// Начать запись пикселей
	ST7789_WriteCommand(ST7789_RAMWR);
}

/* Инициализация дисплея */
void ST7789_Init(void) {
	uint8_t data;

	ST7789_Reset();

	// Программный сброс
	ST7789_WriteCommand(ST7789_SWRESET);
	HAL_Delay(150);

	// Выход из режима сна
	ST7789_WriteCommand(ST7789_SLPOUT);
	HAL_Delay(120);

	// Формат RGB565
	ST7789_WriteCommand(ST7789_COLMOD);
	data = 0x55;
	ST7789_WriteData(&data, 1);
	HAL_Delay(10);

	// Ориентация памяти
	ST7789_WriteCommand(ST7789_MADCTL);
	data = 0x00;
	ST7789_WriteData(&data, 1);

	// Включить инверсию цветов
	ST7789_WriteCommand(ST7789_INVON);
	HAL_Delay(10);

	// Нормальный режим
	ST7789_WriteCommand(0x13);
	HAL_Delay(10);

	// Включить дисплей
	ST7789_WriteCommand(ST7789_DISPON);
	HAL_Delay(20);
}

/* Закрасить прямоугольник */
void ST7789_FillRectangle(uint16_t x, uint16_t y, uint16_t width,
		uint16_t height, uint16_t color) {
	if (x + width > ST7789_WIDTH || y + height > ST7789_HEIGHT)
		return;

	uint8_t data[2];

	// Выбрать область
	ST7789_SetAddressWindow(x, y, x + width - 1, y + height - 1);

	data[0] = color >> 8;
	data[1] = color & 0xFF;

	ST7789_DC_Data();

	uint32_t pixels = (uint32_t) width * height;

	// Заполнить область цветом
	while (pixels--) {
		HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
	}
}

void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
	if (x >= ST7789_WIDTH || y >= ST7789_HEIGHT)
		return;

	uint8_t data[2];

	ST7789_SetAddressWindow(x, y, x, y);

	data[0] = color >> 8;
	data[1] = color & 0xFF;

	ST7789_WriteData(data, 2);
}

void ST7789_FillScreen(uint16_t color) {
	ST7789_FillRectangle(0, 0,
	ST7789_WIDTH,
	ST7789_HEIGHT, color);
}

void ST7789_DrawHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
	ST7789_FillRectangle(x, y, length, 1, color);
}

void ST7789_DrawVLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
	ST7789_FillRectangle(x, y, 1, length, color);
}

void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	int dx, dy, sx, sy, err;

	dx = abs(x1 - x0);
	dy = abs(y1 - y0);

	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;

	err = (dx >= dy) ? dx / 2 : dy / 2;

	while (abs(x1 - x0) > 0 || abs(y1 - y0) > 0) {
		ST7789_DrawPixel(x0, y0, color);

		if (dx >= dy) {
			err += dy;
			x0 += sx;

			if (err >= dx) {
				err -= dx;
				y0 += sy;
			}
		} else {
			err += dx;
			y0 += sy;

			if (err >= dy) {
				err -= dy;
				x0 += sx;
			}
		}
	}

	ST7789_DrawPixel(x1, y1, color);
}


void ST7789_DrawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
	if (width == 0 || height == 0)
		return;

	ST7789_DrawHLine(x, y, width, color);
	ST7789_DrawHLine(x, y + height - 1, width, color);

	ST7789_DrawVLine(x, y, height, color);
	ST7789_DrawVLine(x + width - 1, y, height, color);
}

/* Нарисовать треугольник */
void ST7789_DrawTriangle(uint16_t x0, uint16_t y0,
                         uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2,
                         uint16_t color)
{
	ST7789_DrawLine(x0, y0, x1, y1, color);
	ST7789_DrawLine(x1, y1, x2, y2, color);
	ST7789_DrawLine(x2, y2, x0, y0, color);
}

/* Нарисовать восемь симметричных точек окружности */
static void ST7789_DrawCirclePoints(uint16_t x0, uint16_t y0,
                                    int16_t x, int16_t y,
                                    uint16_t color)
{
	ST7789_DrawPixel(x0 + x, y0 + y, color);
	ST7789_DrawPixel(x0 - x, y0 + y, color);
	ST7789_DrawPixel(x0 + x, y0 - y, color);
	ST7789_DrawPixel(x0 - x, y0 - y, color);

	ST7789_DrawPixel(x0 + y, y0 + x, color);
	ST7789_DrawPixel(x0 - y, y0 + x, color);
	ST7789_DrawPixel(x0 + y, y0 - x, color);
	ST7789_DrawPixel(x0 - y, y0 - x, color);
}

/* Нарисовать окружность */
void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
{
	int16_t x = 0;
	int16_t y = radius;

	int16_t d = 1 - radius;

	while (x <= y)
	{
		ST7789_DrawCirclePoints(x0, y0, x, y, color);

		if (d < 0)
		{
			d += 2 * x + 3;
		}
		else
		{
			d += 2 * (x - y) + 5;
			y--;
		}

		x++;
	}
}

/* Нарисовать символ */
void ST7789_DrawChar(uint16_t x, uint16_t y, char c, const FontDef *font, uint16_t color)
{
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
				ST7789_DrawPixel(x + column, y + row, color);
			}
		}
	}
}


/* Нарисовать строку */
void ST7789_DrawString(uint16_t x, uint16_t y, const char *str, const FontDef *font, uint16_t color)
{
	while (*str != '\0')
	{
		ST7789_DrawChar(x, y, *str, font, color);

		x += font->Width + 1;	// Переходим к следующему символу

		str++;					// Следующий символ строки
	}
}

// Преобразование Unicode-кода русской буквы
// в индекс символа в RusFont5x7_Data[].
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


// Отрисовка одного русского символа UTF-8
void ST7789_DrawCharRUS(uint16_t x, uint16_t y, const char *c, const FontDef *font, uint16_t color)
{
    if (c == NULL || font == NULL)
        return;

    uint8_t byte1 = (uint8_t)c[0];

    // ASCII: английские буквы, цифры, знаки
    if (byte1 < 0x80)
    {
        ST7789_DrawChar(x, y, (char)byte1, &Font_5x7, color);
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

    // Для D1 корректируем старшую часть
    if (byte1 == 0xD1)
        code += 0x40;

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
                ST7789_DrawPixel(x + column, y + row, color);
            }
        }
    }
}


// Отрисовка строки с поддержкой UTF-8
void ST7789_DrawStringRUS(uint16_t x, uint16_t y, const char *str, const FontDef *font, uint16_t color)
{
    if (str == NULL || font == NULL)
        return;

    while (*str != '\0')
    {
        uint8_t byte1 = (uint8_t)*str;

        ST7789_DrawCharRUS(x, y, str, font, color);

        // Русская буква в UTF-8 занимает 2 байта
        if (byte1 == 0xD0 || byte1 == 0xD1)
            str += 2;
        else
            str++;

        x += font->Width + 1;
    }
}
