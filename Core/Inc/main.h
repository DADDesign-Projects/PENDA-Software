/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Encoder3_A_Pin GPIO_PIN_8
#define Encoder3_A_GPIO_Port GPIOB
#define Encoder3_B_Pin GPIO_PIN_9
#define Encoder3_B_GPIO_Port GPIOB
#define Encoder3_SW_Pin GPIO_PIN_10
#define Encoder3_SW_GPIO_Port GPIOG
#define FootSwitch2_Pin GPIO_PIN_9
#define FootSwitch2_GPIO_Port GPIOG
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_Port GPIOC
#define Encoder2_SW_Pin GPIO_PIN_0
#define Encoder2_SW_GPIO_Port GPIOC
#define TFT_Reset_Pin GPIO_PIN_1
#define TFT_Reset_GPIO_Port GPIOC
#define Encoder0_SW_Pin GPIO_PIN_1
#define Encoder0_SW_GPIO_Port GPIOA
#define Encoder0_A_Pin GPIO_PIN_0
#define Encoder0_A_GPIO_Port GPIOA
#define TFT_DC_Pin GPIO_PIN_4
#define TFT_DC_GPIO_Port GPIOC
#define Encoder0_B_Pin GPIO_PIN_11
#define Encoder0_B_GPIO_Port GPIOD
#define FootSwitch2A2_Pin GPIO_PIN_2
#define FootSwitch2A2_GPIO_Port GPIOA
#define Encoder1_B_Pin GPIO_PIN_6
#define Encoder1_B_GPIO_Port GPIOA
#define Encoder2_A_Pin GPIO_PIN_5
#define Encoder2_A_GPIO_Port GPIOA
#define Encoder2_B_Pin GPIO_PIN_3
#define Encoder2_B_GPIO_Port GPIOA
#define Encoder1_A_Pin GPIO_PIN_7
#define Encoder1_A_GPIO_Port GPIOA
#define Encoder1_SW_Pin GPIO_PIN_1
#define Encoder1_SW_GPIO_Port GPIOB
#define RESET_CODEC_Pin GPIO_PIN_11
#define RESET_CODEC_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */


/* Sections ---------------------------------------------------------*/
#define SDRAM_SECTION __attribute__((section(".SDRAM_Section")))
#define QFLASH_SECTION __attribute__((section(".QFLASH_Section")))
#define NO_CACHE_RAM __attribute__((section(".NO_CACHE_RAM_Section")))
#define ITCM __attribute__((section(".RAM_ITCM0_Section")))


/* Audio ---------------------------------------------------------*/
#define AUDIO_BUFFER_SIZE 4
struct AudioBuffer{
	float Right;
	float Left;
};
extern HAL_StatusTypeDef StartAudio();
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
