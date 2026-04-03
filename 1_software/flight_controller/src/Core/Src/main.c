#include "stm32f4xx_hal.h"

#include <string.h>
#include <stdio.h>

#include "lis3dsh.h"
#include "lowpassfilter.h"
#include "main.h"

LIS3DSH_HandleTypeDef   lis3dsh;

UART_HandleTypeDef      huart2;
//DMA_HandleTypeDef hdma_usart2_rx;

Lowpassfilter_Handle    accFiltered;
Lowpassfilter_Data      accInput;

char txData[100];

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}


static void MX_SPI1_Init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();

    
    lis3dsh.spiHandler.Instance = SPI1;
    lis3dsh.spiHandler.Init.Mode = SPI_MODE_MASTER;
    lis3dsh.spiHandler.Init.Direction = SPI_DIRECTION_2LINES;
    lis3dsh.spiHandler.Init.DataSize = SPI_DATASIZE_8BIT;
    lis3dsh.spiHandler.Init.CLKPolarity = SPI_POLARITY_LOW;
    lis3dsh.spiHandler.Init.CLKPhase = SPI_PHASE_1EDGE;
    lis3dsh.spiHandler.Init.NSS = SPI_NSS_SOFT;
    lis3dsh.spiHandler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    lis3dsh.spiHandler.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lis3dsh.spiHandler.Init.TIMode = SPI_TIMODE_DISABLE;
    lis3dsh.spiHandler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    
    HAL_SPI_Init(&lis3dsh.spiHandler);
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LED PD12
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // SPI1 Pins: PA5=SCK, PA6=MISO, PA7=MOSI
    GPIO_InitStruct.Pin = LIS3DSH_SCK_PIN | LIS3DSH_MISO_PIN | LIS3DSH_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(LIS3DSH_SCK_PORT, &GPIO_InitStruct);

    // CS Pin PE3
    GPIO_InitStruct.Pin = LIS3DSH_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LIS3DSH_CS_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LIS3DSH_CS_PORT, LIS3DSH_CS_PIN, GPIO_PIN_RESET);
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_UART_Init(&huart2);
}

void mems_Init(void)
{
    lis3dsh.init.dataRate = LIS3DSH_DATARATE_12_5;
    lis3dsh.init.fullScale = LIS3DSH_FULLSCALE_4;
    lis3dsh.init.antiAliasingBW = LIS3DSH_FILTER_BW_50;
    lis3dsh.init.enableAxes = LIS3DSH_XYZ_ENABLE;
    lis3dsh.init.interruptEnable = false;

    lis3dsh.ioBank.csPort = LIS3DSH_CS_PORT;
    lis3dsh.ioBank.csPin = LIS3DSH_CS_PIN;

    LIS3DSH_Init(&lis3dsh);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();

    mems_Init();

    LowpassFilter_Init(&accFiltered, LPF_TYPE_BUTTERWORTH, LPF_ACC_CUTOFF_HZ, SAMPLE_TIME_ACC_MS);

    while(1)
    {
        if(LIS3DSH_DataReady(1000))
        {
            lis3dsh.dataScaled = LIS3DSH_GetScaledData();
            snprintf(txData, sizeof(txData), "[raw] x: %.2f, y: %.2f, z: %.2f\r\n", lis3dsh.dataScaled.x, lis3dsh.dataScaled.y, lis3dsh.dataScaled.z);
            HAL_UART_Transmit(&huart2, (uint8_t *)txData, strlen(txData), 10);

            //float accFilt[3];
		    //for (int n = 0; n < 3; n++) {
		    //    accFilt[n] = LowpassFilter_Update(&lpfAcc[n], lis3dsh.dataScaled.x);
		    //}

            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
        }
    }
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}
