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
#include "cDisplay.h"

#include "cEncoder.h"
#include "cIS25LPxxx.h"
#include "cMemory.h"
#include "QSPI.h"
#include "PendaUI.h"
#include "UIComponent.h"
#include "Parameter.h"
#include "cDCO.h"

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

// =====** DAD **=================================================================

extern "C" void DAD_MPU_Config(void);

//QFlash
DadQSPI::cIS25LPxxx __Flash;

//GFX
DECLARE_DISPLAY(__Display);
DECLARE_LAYER(Back, TFT_HEIGHT , TFT_WIDTH);

//QSPI Storage
QFLASH_SECTION DadQSPI::cQSPI_FlasherStorage  __FlashStorage;
DadQSPI::cQSPI_PersistentStorage 			  __PersistentStorage;

// LFO
DadDSP::cDCO __LFO;

// UI Object Manager
DadUI::cUIObjectManager __UIObjManager;

// Main params
DadUI::cParameter __ParamDepth;

// LFO params
DadUI::cParameter __ParamShape;
DadUI::cParameter __ParamSpeed;
DadUI::cParameter __ParamRatio;


// ------------------------------------------------------------------------
// Parameter callback
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// SpeedChange
void SpeedChange(float Value){
	__LFO.setFreq(Value);
}

// ------------------------------------------------------------------------
// Ratio change
void RatioChange(float Value){
	__LFO.setNormalizedDutyCycle(__ParamRatio.getNormalizedValue());
}

// ------------------------------------------------------------------------
// AudioCallback - Processes audio in real-time (called by the audio engine)
// ------------------------------------------------------------------------
bool __MemOnOff = false; 	// Global state variable: Tracks whether audio processing is active
uint32_t __CT=0; 			// Cycle counter
							// - Used in main loop to blink an activity LED
 	 	 	 	 	 	 	// - The LED blink rate indicates proper callback execution

// ITCM: Optimized for fast execution (placed in Instruction Tightly Coupled Memory)
ITCM void AudioCallback(AudioBuffer *pIn, AudioBuffer *pOut) {
    // Get the current ON/OFF state from the UI (real-time safe)
    bool OnOff = DadUI::cPendaUI::RTProcess();

    // Process each sample in the audio buffer
    for (size_t i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        // Detect state change only when audio is near silence (avoid clicks)
        if ((OnOff != __MemOnOff) && (fabs(pIn->Right + pIn->Left) < 0.001f)) {
            __MemOnOff = OnOff; // Update state if crossing threshold
        }

        // Apply processing if active
        if (__MemOnOff) {
            __LFO.Step(); // Advance LFO to next step

            float LFOVal = 0;
            // Select LFO waveform based on parameter
            switch ((uint16_t)__ParamShape) {
                case 0: // Sine wave
                    LFOVal = __LFO.getTriangleModValue();
                    break;
                case 1: // Square wave
                    LFOVal = __LFO.getSquareModValue();
                    break;
            }

            // Apply modulated gain to audio (LFO * depth control)
            pOut->Right = pIn->Right * LFOVal * std::pow(10.0f,(1 - __ParamDepth.getNormalizedValue()) * -12 / 20.0f);
            pOut->Left = pIn->Left * LFOVal * std::pow(10.0f, (1-__ParamDepth.getNormalizedValue()) *  -12/ 20.0f);
        } else {
            // Bypass processing (pass-through)
            pOut->Right = pIn->Right;
            pOut->Left = pIn->Left;
        }

        // Advance buffer pointers (post-increment)
        pOut++;
        pIn++;
    }

    // Increment cycle counter for visual feedback:
    __CT++;
}

// ===** End DAD **==================================================================
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
// =====** DAD **=================================================================

#ifdef USE_RAM
	SCB->VTOR = 0x24000000;
#endif

// ===** End DAD **==================================================================

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

// =====** DAD **=================================================================

  // Performing initializations -----------------------
  DAD_MPU_Config();				// Initialize MPU
  __Flash.Init(&hqspi); 		// Initialize Flash memory

  SCB_EnableICache(); 			// Enable I-Cache
  SCB_EnableICache(); 			// Enable D-Cache

  // Display Initializations
  INIT_DISPLAY(__Display, &hspi1);
  __Display.setOrientation(Rotation::Degre_90);
  DadGFX::cLayer *pBack = ADD_LAYER(Back, 0,0,1);
  DadGFX::cFont Font(FONTL);

  // PersistentStorage initialization
  if(__PersistentStorage.Init()){
		pBack->eraseLayer(SPLASHSCREEN_BACK_COLOR);

	    const uint16_t TextCentre = 320/2;
	    pBack->setTextFrontColor(SPLASHSCREEN_TEXT_COLOR);
	    pBack->setFont(&Font);

	    const char *pText1 = "Please wait (~60s)";
	    uint16_t TextWidth = pBack->getTextWidth(pText1);
	    pBack->setCursor(TextCentre - (TextWidth/2), 40);
	    pBack->drawText(pText1);

	    const char *pText2 = "Flash memory";
	    TextWidth = pBack->getTextWidth(pText2);
	    pBack->setCursor(TextCentre - (TextWidth/2), 80);
	    pBack->drawText(pText2);

	    const char *pText3 = "initialization";
	    TextWidth = pBack->getTextWidth(pText3 );
	    pBack->setCursor(TextCentre - (TextWidth/2), 120);
	    pBack->drawText(pText3 );

	    const char *pText4 = "in progress.";
	    TextWidth = pBack->getTextWidth(pText4);
	    pBack->setCursor(TextCentre - (TextWidth/2), 160);
	    pBack->drawText(pText4);

	    __Display.flush();
	    __PersistentStorage.InitializeMemory();
  }
  pBack->eraseLayer(DadGFX::sColor(0,0,0,255));

  // GUI Initializations
  DadUI::cPendaUI::Init("Demo", "Tremolo", "Version 1.0");

  __ParamDepth.Init(50.0f, 0.0f, 100.0f,    // float InitValue, float Min, float Max,
			  5.0f, 1.0f,     		 	 	// float RapidIncrement, float SlowIncrement,
			  nullptr,       				// CallbackType Callback = nullptr,
			  0.2f * UI_RT_SAMPLING_RATE);	// float Slope = 0.1s);

  __ParamShape.Init(0.0f, 0.0f, 1.0f,   	// float InitValue, float Min, float Max,
			  1.0f, 1.0f, 	 				// float RapidIncrement, float SlowIncrement,
			  nullptr,   	 				// CallbackType Callback = nullptr,
			  0);		 	 				// float Slope = 0);

  __ParamSpeed.Init(5.0f, 1.0, 10.0f,  		// float InitValue, float Min, float Max,
			  0.5f, 0.05f, 	 			    // float RapidIncrement, float SlowIncrement,
			  SpeedChange,	 			    // CallbackType Callback = nullptr,
			  0);		 	 			    // float Slope = 0, float SamplingRate=SAMPLING_RATE);

  __ParamRatio.Init(0, -100, +100,			// float InitValue, float Min, float Max,
			  5.0f, 1.0f, 			        // float RapidIncrement, float SlowIncrement,
			  RatioChange,     				// CallbackType Callback = nullptr,
			  0);		     				// float Slope = 0, float SamplingRate=SAMPLING_RATE);

  DadUI::cParameterNumNormalView 		DeepView;
  DadUI::cParameterDiscretView	 		ShapeView;
  DadUI::cParameterNumNormalView		SpeedView;
  DadUI::cParameterNumLeftRightView		RatioView;

  DeepView.Init(&__ParamDepth, "Depth", "Depth",
  	  	  	  	  "%", "%");

  SpeedView.Init(&__ParamSpeed, "Freq.", "Frequency",
  	  	  	  	  "Hz", "Hz");

  RatioView.Init(&__ParamRatio, "Ratio", "Duty cycle ratio",
	  	  	  	  "%", "%");

  ShapeView.Init(&__ParamShape, "Shape", "Shape");
  ShapeView.AddDiscreteValue("triang.", "Triangular"); //const std::string& ShortDiscretValue, const std::string& LongDiscretValue);
  ShapeView.AddDiscreteValue("Square", "Square");      //const std::string& ShortDiscretValue, const std::string& LongDiscretValue);

  DadUI::cUIParameters ItemMainMenu;
  DadUI::cUIParameters ItemLFOMenu2;
  DadUI::cUIMemory	   ItemMenuMemory;

  ItemMainMenu.Init(&DeepView, nullptr, &SpeedView);
  ItemLFOMenu2.Init(&SpeedView, &ShapeView, &RatioView);
  ItemMenuMemory.Init();

  DadUI::cUIMenu Menu1;
  Menu1.Init();

  Menu1.addMenuItem(&ItemMainMenu, "Main");
  Menu1.addMenuItem(&ItemLFOMenu2, "LFO");
  Menu1.addMenuItem(&ItemMenuMemory, "Memory");


  DadUI::cTapTempo TapTempo;
  TapTempo.Init(&DadUI::cPendaUI::m_FootSwitch2, &SpeedView, DadUI::eTempoType::frequency);

  DadUI::cPendaUI::setActiveObject(&Menu1);

  //Audio Initializations
  __LFO.Initialize(SAMPLING_RATE, 1.0f/__ParamSpeed, 1, 10, __ParamRatio.getNormalizedValue());
  StartAudio();

  // Display refresh
  __Display.flush();

// ===** End DAD **=================================================================

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
// =====** DAD **=================================================================
	  DadUI::cPendaUI::Update();
      __Display.flush();
      if(__CT >= (uint32_t) (((float)SAMPLING_RATE / 4.0f)  * 0.5f)){
    	  __CT =0;
    	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      }
	  HAL_Delay(100);

// ===** End DAD **=================================================================

  }
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
  PeriphClkInitStruct.PLL2.PLL2M = 1;
  PeriphClkInitStruct.PLL2.PLL2N = 50;
  PeriphClkInitStruct.PLL2.PLL2P = 66;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 5637;
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
  hsai_BlockA1.Init.NoDivider = SAI_MCK_OVERSAMPLING_DISABLE;
  hsai_BlockA1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
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
  hsai_BlockB1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
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

  /*Configure GPIO pins : Encoder3_SW_Pin FootSwitch1_Pin */
  GPIO_InitStruct.Pin = Encoder3_SW_Pin|FootSwitch1_Pin;
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

  /*Configure GPIO pins : Encoder0_SW_Pin Encoder0_A_Pin FootSwitch2_Pin Encoder1_B_Pin
                           Encoder2_A_Pin Encoder2_B_Pin Encoder1_A_Pin */
  GPIO_InitStruct.Pin = Encoder0_SW_Pin|Encoder0_A_Pin|FootSwitch2_Pin|Encoder1_B_Pin
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
