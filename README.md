# STM32-library-for-IPS-DISPLAY-ST7789-SPI-


## Возможности библиотеки.
Должен сказать что модуль этого экранчика распаян производителем под управление через SPI. Кроме того, **отсутствует пин Cheap Select**. Поэтому мой экран имеет всего лишь 7 пинов. А код библиотеки не имеет логики Cheap Select. Будьте внимательны, если вы подключаете по SPI несколько периферийных устройств!

<img width="1145" height="859" alt="IMG_7059" src="https://github.com/user-attachments/assets/b67b990b-e540-4b48-b44f-c5a1e5579052" />

## Структура папок.

В корневой папке проекта STM32 Cube IDE создать папку `Libraries` (см.рис.1)

<img width="474" height="375" alt="image" src="https://github.com/user-attachments/assets/10dec2c5-6c14-45b8-8de4-125a5c65b116" />

_рис.1 Папка Libraries_

Далее в этой папке создадим папку `ST7789` (см.рис.2). Здесь будут храниться файлы нашей библиотеки.

<img width="476" height="394" alt="image" src="https://github.com/user-attachments/assets/aa690030-925b-4cf6-9b5b-5525dac295dc" />

_рис.2 Папка ST7789_


Набор файлов Libraries/ST7789 (см.рис.3)

<img width="475" height="495" alt="image" src="https://github.com/user-attachments/assets/0a2c87f2-c4dc-4e51-9744-a727fe516d49" />

_рис.3 Содержимое папки ST7789_

## Как подключить библиотеку.

`project -> properties -> C/C++ General -> Includes -> Добавляем путь до папки ST7789`
<img width="855" height="556" alt="image" src="https://github.com/user-attachments/assets/33ec119a-5190-4ba9-a6c3-56ee540c08a1" />

`project -> properties -> C/C++ General -> Source Location -> Добавляем путь до папки Libraries`
<img width="850" height="546" alt="image" src="https://github.com/user-attachments/assets/94101aa6-c65f-48f9-9ffb-203e903664eb" />

## Пример использования из main.c

```C

#include "main.h"
#include "st7789.h"
SPI_HandleTypeDef hspi1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

int main(void) {

	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_SPI1_Init();
	
  ST7789_Init();
  ST7789_DrawChar(20, 20, 'A', &Font_5x7, ST7789_WHITE);
  ST7789_DrawChar(30, 20, 'B', &Font_5x7, ST7789_RED);
  ST7789_DrawChar(40, 20, 'C', &Font_5x7, ST7789_GREEN);
  ST7789_DrawChar(40, 20, '@', &Font_5x7, ST7789_MAGENTA);

  ST7789_DrawString(50,50, "Hello, WORLD !!!", &Font_5x7, ST7789_CYAN);
	 
	while (1) {
		
	}

}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

static void MX_SPI1_Init(void) {

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, RES_Pin | DC_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : RES_Pin DC_Pin */
	GPIO_InitStruct.Pin = RES_Pin | DC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

```
