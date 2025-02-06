//------------------------------------------------------------------------
// TFT_SPI.cpp
//  Management of an SPI connection to a TFT screen
// Copyright(c) 2024 Dad Design.
//------------------------------------------------------------------------

#include "TFT_SPI.h"

#if TFT_CONTROLEUR_TFT == 7735
    #include "ST7735_Defines.h"
#elif TFT_CONTROLEUR_TFT == 7789
    #include "ST7789_Defines.h"
#endif

namespace DadGFX {

volatile TFT_DMACallback pTFT_DMACallback= nullptr;
volatile void *pTFT_DMA_Context = nullptr;


//***********************************************************************************
// TFT_SPI
//  SPI management for communication with the TFT screen
//*********************************************************************************** 

void TxCpltCallback(SPI_HandleTypeDef *phspi){
	if(pTFT_DMACallback != nullptr){
		pTFT_DMACallback((void *) pTFT_DMA_Context);
	}
}

// --------------------------------------------------------------------------
// Initialize the SPI connection
void TFT_SPI::Init_TFT_SPI(SPI_HandleTypeDef* phspi) {
	// SPI configuration
	m_phspi = phspi;
	HAL_SPI_RegisterCallback(m_phspi, HAL_SPI_TX_COMPLETE_CB_ID, DadGFX::TxCpltCallback);

    // Reset the LCD screen
	resetRST();
	HAL_Delay(50);
    setRST();
	HAL_Delay(50);

    // Perform additional initialization
    Initialise();
}
// --------------------------------------------------------------------------
// Send a command using DMA (non-blocking)
void TFT_SPI::SendDMACommand(uint8_t *cmd, TFT_DMACallback pEndCallback, void* pCallbackContext) {
	//while(TFT_DMA_Busy != eTFT_DMA_Busy::Free) {HAL_Delay(1);};
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET); // Set DC pin to command mode
	pTFT_DMACallback = pEndCallback;
	pTFT_DMA_Context = pCallbackContext;
	HAL_SPI_Transmit_DMA(m_phspi, cmd, 1);
}

// --------------------------------------------------------------------------
// Send a block of data using DMA (non-blocking)
void TFT_SPI::SendDMAData(uint8_t* buff, size_t size, TFT_DMACallback pEndCallback, void* pCallbackContext) {
	//while(TFT_DMA_Busy != eTFT_DMA_Busy::Free){HAL_Delay(1);};
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET); // Set DC pin to command mode
	pTFT_DMACallback = pEndCallback;
	pTFT_DMA_Context = pCallbackContext;
	HAL_SPI_Transmit_DMA(m_phspi, buff, size);
}

// --------------------------------------------------------------------------
// Set the display rotation (0, 90, 180, 270 degrees)
void TFT_SPI::setTFTRotation(Rotation r) {
    // Send the memory access control command
    SendCommand(TFT_MADCTL);
    switch (r) {
        case Rotation::Degre_0: // Portrait
            SendData(TFT_MAD_COLOR_ORDER);
            break;
        case Rotation::Degre_90: // Landscape (Portrait + 90 degrees)
            SendData(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_COLOR_ORDER);
            break;
        case Rotation::Degre_180: // Inverted portrait
            SendData(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);
            break;
        case Rotation::Degre_270: // Inverted landscape
            SendData(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);
            break;
    }
}

} // namespace DadGFX
