#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx_hal.h"

/* Sample times */
#define SAMPLE_TIME_ACC_MS  16
#define SAMPLE_TIME_GYR_MS  10
#define SAMPLE_TIME_MAG_MS  100
#define SAMPLE_TIME_BAR_MS  20
#define SAMPLE_TIME_RC_MS   10
#define SAMPLE_TIME_CTRL_MS 4
#define SAMPLE_TIME_PWM_MS  4
#define SAMPLE_TIME_LED_MS  500

/* Low-pass filters */
#define LPF_ACC_CUTOFF_HZ 10.0f
#define LPF_GYR_CUTOFF_HZ 32.0f
#define LPF_MAG_CUTOFF_HZ  5.0f
#define LPF_BAR_CUTOFF_HZ  2.0f
#define LPF_RC_CUTOFF_HZ   7.5f

/* Pins and ports */
#define LIS3DSH_CS_PORT     GPIOE
#define LIS3DSH_CS_PIN      GPIO_PIN_3
#define LIS3DSH_SCK_PORT    GPIOA
#define LIS3DSH_SCK_PIN     GPIO_PIN_5
#define LIS3DSH_MISO_PORT   GPIOA
#define LIS3DSH_MISO_PIN    GPIO_PIN_6
#define LIS3DSH_MOSI_PORT   GPIOA
#define LIS3DSH_MOSI_PIN    GPIO_PIN_7

void mems_Init(void);

#endif
