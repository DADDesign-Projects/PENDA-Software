/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cIS25LPxxx.h"

#include "cDisplay.h"
#include "QSPI.h"
#include "cEncoder.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")
#include "Penda.h"
#include "Vanilla_Extract_20p.h"
#pragma GCC pop_options

#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DMA2D_HandleTypeDef hdma2d;

QSPI_HandleTypeDef hqspi;

SAI_HandleTypeDef hsai_BlockA1;
SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_a;
DMA_HandleTypeDef hdma_sai1_b;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// ==** DAD **=================================================================

extern "C" void DAD_MPU_Config(void);

//QFlash
Dad::cIS25LPxxx __Flash;

//GFX
DECLARE_DISPLAY(__Display);
DECLARE_LAYER(BackgroundLayer, 320, 240)
DECLARE_LAYER(BallLayer, 16, 16)
DECLARE_LAYER(RacquetLayer, 6, 40)
DECLARE_LAYER(TextLayer, 250, 100)

//QSPI FlasherStorage
DadQSPI::cQSPI_FlasherStorage QFLASH_SECTION __FlashStorage;

//Encoders
DadUI::cEncoder __Encoder0;
DadUI::cEncoder __Encoder1;
DadUI::cEncoder __Encoder2;
DadUI::cEncoder __Encoder3;

int16_t __Encoder0Inc;
int16_t __Encoder1Inc;
int16_t __Encoder2Inc;
int16_t __Encoder3Inc;

// ------------------------------------------------------------------------
// AudioCallback
// ------------------------------------------------------------------------
ITCM void AudioCallback(AudioBuffer *pIn, AudioBuffer *pOut){
	for (size_t i = 0; i < AUDIO_BUFFER_SIZE; i++)
	{
		pOut->Right = pIn->Right;
		pOut->Left = pIn->Left;
		pOut++;
		pIn++;
	}
	__Encoder0.Debounce();
	__Encoder1.Debounce();
	__Encoder2.Debounce();
	__Encoder3.Debounce();
	__Encoder0Inc += __Encoder0.getIncrement();
	__Encoder1Inc += __Encoder1.getIncrement();
	__Encoder2Inc += __Encoder2.getIncrement();
	__Encoder3Inc += __Encoder3.getIncrement();
}

// ==End DAD==================================================================
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
// ==** DAD **=================================================================

#ifdef USE_RAM
	SCB->VTOR = 0x24000000;
#endif

// ===End DAD==================================================================

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_QUADSPI_Init();
  MX_FMC_Init();
  MX_SAI1_Init();
  MX_SPI1_Init();
  MX_DMA2D_Init();
  /* USER CODE BEGIN 2 */

  // ==** DAD **=================================================================

  // Performing initializations
	__Flash.Init(&hqspi); 	// Initialize Flash memory
	DAD_MPU_Config();		// Initialize MPU
	SCB_EnableICache(); 	// Enable I-Cache
	SCB_EnableICache(); 	// Enable D-Cache
	INIT_DISPLAY(__Display, &hspi1); // Display Initialization
	// Encoder Initialization
	__Encoder0.Init(Encoder0_A_GPIO_Port, Encoder0_A_Pin, Encoder0_B_GPIO_Port, Encoder0_B_Pin, Encoder0_SW_GPIO_Port, Encoder0_SW_Pin, 20, 100);
	__Encoder1.Init(Encoder1_A_GPIO_Port, Encoder1_A_Pin, Encoder1_B_GPIO_Port, Encoder1_B_Pin, Encoder1_SW_GPIO_Port, Encoder1_SW_Pin, 20, 100);
	__Encoder2.Init(Encoder2_A_GPIO_Port, Encoder2_A_Pin, Encoder2_B_GPIO_Port, Encoder2_B_Pin, Encoder2_SW_GPIO_Port, Encoder2_SW_Pin, 20, 100);
	__Encoder3.Init(Encoder3_A_GPIO_Port, Encoder3_A_Pin, Encoder3_B_GPIO_Port, Encoder3_B_Pin, Encoder3_SW_GPIO_Port, Encoder3_SW_Pin, 20, 100);
	StartAudio();					 // Start audio callback

    // Display demo
	__Display.setOrientation(Rotation::Degre_90);
	DadGFX::cLayer* pBackground = ADD_LAYER(BackgroundLayer, 0, 0, 1);
	pBackground->drawFillRect(0,0,320, 240, DadGFX::sColor(9, 111, 148, 255));
	pBackground->drawFillRect(80, 0, 80, 240, DadGFX::sColor(23, 148, 194, 255));
	pBackground->drawFillRect(240, 0, 240, 240, DadGFX::sColor(23, 148, 194, 255));


	DadGFX::cLayer* pRacquet = ADD_LAYER(RacquetLayer, 0, 0, 11);
	pRacquet->drawFillRect(0,0,6,40, DadGFX::sColor(255, 255, 255, 255));

	DadGFX::cLayer* pBall = ADD_LAYER(BallLayer, 0, 0, 10);
	pBall->drawFillCircle(7,7,7,DadGFX::sColor(255, 255, 255,255));

	DadGFX::cImageLayer* pPenda = __Display.addLayer(Penda_map, 70, 20, 80, 80, 8);
	// If you use flash memory storage
	//DadGFX::cImageLayer* pPenda = __Display.addLayer(__FlashStorage.GetFilePtr("Penda.png"), 70, 20, 80, 80, 8);

	DadGFX::cFont Vanilla(&__Vanilla_Extract_20p);
	// If you use flash memory storage
	//DadGFX::GFXBinFont * pBinFont = (DadGFX::GFXBinFont *)__FlashStorage.GetFilePtr("Vanilla_Extract_20p.bin");
	//DadGFX::cFont Vanilla(pBinFont);
	DadGFX::cLayer* pText = ADD_LAYER(TextLayer, 20, 160, 2);
	pText->setFont(&Vanilla);
	pText->setCursor(0,0);
	pText->setTextFrontColor(DadGFX::sColor(255,255,255,100));
	pText->drawText("Demo");
	pText->setCursor(0,Vanilla.getHeight());
	pText->drawText("PENDA Generic");

	__Display.flush();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  float AvanceX = 7;
  float AvanceY = 5;
  int16_t PosX = 0;
  int16_t PosY = 100;
  int16_t PosRacquetX = 320-30;
  int16_t PosRacquetY = (240/2)-20;

  int16_t XPenda = 20;
  int16_t YPenda = 70;
  uint8_t  Move = 0;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(__Encoder0Inc != 0){
		  XPenda += __Encoder0Inc;
		  __Encoder0Inc = 0;
		  Move = 1;
	  }
	  if(__Encoder1Inc != 0){
		  XPenda += __Encoder1Inc;
		  YPenda += __Encoder1Inc;
		  __Encoder1Inc = 0;
		  Move = 1;
	  }
	  if(__Encoder2Inc != 0){
		  YPenda += __Encoder2Inc;
		  __Encoder2Inc = 0;
		  Move = 1;
	  }
	  if(__Encoder3Inc != 0){
		  XPenda += __Encoder3Inc;
		  YPenda -= __Encoder3Inc;
		  __Encoder3Inc = 0;
		  Move = 1;
	  }

	  if( Move == 1 ){
		  if(XPenda < 0) XPenda = 0;
		  if(XPenda > __Display.getWith()) XPenda=__Display.getWith()-1;
		  if(YPenda <0) YPenda = 0;
		  if(YPenda > __Display.getHeight()) YPenda=__Display.getHeight()-1;

		  Move = 0;
		  pPenda->moveLayer(XPenda, YPenda);
	  }

      PosX += AvanceX;
      if(PosX < 0){
          PosX = 0;
          AvanceX = - AvanceX;
      }
      if(PosX >= TFT_HEIGHT-30){
          PosX = TFT_HEIGHT-30;
          AvanceX = - AvanceX;
      }

      PosY += AvanceY;
      if(PosY < 0){
          PosY = 0;
          AvanceY = - AvanceY;
      }

      if(PosY >= TFT_WIDTH-7){
          PosY = TFT_WIDTH-7;
          AvanceY = - AvanceY;
      }

      int16_t DelatY = (PosRacquetY + 20) - PosY;

      PosRacquetY -= DelatY / (3+((TFT_HEIGHT-PosX)/25));

      if(PosRacquetY < 0){
          PosRacquetY = 0;
      }

      if((PosRacquetY+40) > TFT_WIDTH){
          PosRacquetY = TFT_WIDTH-40;
      }

      pBall->moveLayer(PosX, PosY);
      pRacquet->moveLayer(PosRacquetX, PosRacquetY);
      __Display.flush();
      HAL_Delay(10);
  }
  // ==End DAD=================================================================
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  RCC_OscInitStruct.PLL.PLLR = 8;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
  PeriphClkInitStruct.PLL2.PLL2M = 4;
  PeriphClkInitStruct.PLL2.PLL2N = 147;
  PeriphClkInitStruct.PLL2.PLL2P = 4;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 3750;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 2;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hqspi.Init.FlashSize = 22;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

/**
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_MSBJUSTIFIED, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockB1.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_MSBJUSTIFIED, SAI_PROTOCOL_DATASIZE_24BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

}

/* FMC initialization function */
void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 8;
  SdramTiming.WriteRecoveryTime = 3;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 10;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
#define SDRAM_MODEREG_BURST_LENGTH_2 ((1 << 0))
#define SDRAM_MODEREG_BURST_LENGTH_4 ((1 << 1))

#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((0 << 3))

#define SDRAM_MODEREG_CAS_LATENCY_3 ((1 << 4) | (1 << 5))

#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ()

#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((1 << 9))
#define SDRAM_MODEREG_WRITEBURST_MODE_PROG_BURST ((0 << 9))

   FMC_SDRAM_CommandTypeDef Command;

   /* Step 3:  Configure a clock configuration enable command */
   Command.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
   Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber      = 1;
   Command.ModeRegisterDefinition = 0;

   /* Send the command */
   if(HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000) != HAL_OK){
	    Error_Handler();
   }

   /* Step 4: Insert 100 ms delay */
   HAL_Delay(100);

   /* Step 5: Configure a PALL (precharge all) command */
   Command.CommandMode            = FMC_SDRAM_CMD_PALL;
   Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber      = 1;
   Command.ModeRegisterDefinition = 0;

   /* Send the command */
   if(HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000) != HAL_OK){
	    Error_Handler();
   }

   /* Step 6 : Configure a Auto-Refresh command */
   Command.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
   Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber      = 4;
   Command.ModeRegisterDefinition = 0;

   /* Send the command */
   if(HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000) != HAL_OK){
	    Error_Handler();
   }

   /* Step 7: Program the external memory mode register */
   uint32_t tmpmrd = 0;
   tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4
            | SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL | SDRAM_MODEREG_CAS_LATENCY_3
            | SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

   Command.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
   Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
   Command.AutoRefreshNumber      = 1;
   Command.ModeRegisterDefinition = tmpmrd;

   /* Send the command */
   if(HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000) != HAL_OK){
	    Error_Handler();
   }

   if(HAL_SDRAM_ProgramRefreshRate(&hsdram1, 0x81A - 20) != HAL_OK){
	    Error_Handler();
   }

  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_Pin|TFT_Reset_Pin|TFT_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RESET_CODEC_GPIO_Port, RESET_CODEC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Encoder3_A_Pin Encoder3_B_Pin Encoder1_SW_Pin */
  GPIO_InitStruct.Pin = Encoder3_A_Pin|Encoder3_B_Pin|Encoder1_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Encoder3_SW_Pin FootSwitch2_Pin */
  GPIO_InitStruct.Pin = Encoder3_SW_Pin|FootSwitch2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin TFT_Reset_Pin */
  GPIO_InitStruct.Pin = LED_Pin|TFT_Reset_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : Encoder2_SW_Pin */
  GPIO_InitStruct.Pin = Encoder2_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Encoder2_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Encoder0_SW_Pin Encoder0_A_Pin FootSwitch2A2_Pin Encoder1_B_Pin
                           Encoder2_A_Pin Encoder2_B_Pin Encoder1_A_Pin */
  GPIO_InitStruct.Pin = Encoder0_SW_Pin|Encoder0_A_Pin|FootSwitch2A2_Pin|Encoder1_B_Pin
                          |Encoder2_A_Pin|Encoder2_B_Pin|Encoder1_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : TFT_DC_Pin */
  GPIO_InitStruct.Pin = TFT_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TFT_DC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Encoder0_B_Pin */
  GPIO_InitStruct.Pin = Encoder0_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Encoder0_B_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RESET_CODEC_Pin */
  GPIO_InitStruct.Pin = RESET_CODEC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RESET_CODEC_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
