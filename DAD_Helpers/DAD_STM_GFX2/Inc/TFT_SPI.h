#pragma once
//------------------------------------------------------------------------
// TFT_SPI.h
//  Management of an SPI connection to a TFT screen
// Copyright(c) 2024 Dad Design.
//------------------------------------------------------------------------
#include "main.h"
#include "UserConfig.h"

// TFT Generic commands
#define TFT_NOP     0x00    // No operation
#define TFT_SWRST   0x01    // Software reset

#define TFT_INVOFF  0x20    // Disable inversion
#define TFT_INVON   0x21    // Enable inversion

#define TFT_DISPOFF 0x28    // Turn display off
#define TFT_DISPON  0x29    // Turn display on

#define TFT_CASET   0x2A    // Set column address
#define TFT_RASET   0x2B    // Set row address
#define TFT_RAMWR   0x2C    // Write to display RAM

#define TFT_MADCTL  0x36    // Memory access control
#define TFT_MAD_MY  0x80    // Row address order
#define TFT_MAD_MX  0x40    // Column address order
#define TFT_MAD_MV  0x20    // Row/Column exchange
#define TFT_MAD_ML  0x10    // Vertical refresh order
#define TFT_MAD_BGR 0x08    // Blue-Green-Red pixel order
#define TFT_MAD_MH  0x04    // Horizontal refresh order
#define TFT_MAD_RGB 0x00    // Red-Green-Blue pixel order

enum class Rotation {
    Degre_0,     // 0-degree rotation
    Degre_90,    // 90-degree rotation
    Degre_180,   // 180-degree rotation
    Degre_270    // 270-degree rotation
};

// GPIO configurations for TFT
namespace DadGFX {

typedef void (*TFT_DMACallback)(void *pContext);
enum class eTFT_DMA_Busy{
	Free,
	Busy
};

void TxCpltCallback(SPI_HandleTypeDef *phspi) ;

//***********************************************************************************
// TFT_SPI
//  SPI management for communication with the TFT screen
//*********************************************************************************** 
class TFT_SPI {
    public :

    // --------------------------------------------------------------------------
    // Initialize SPI communication
    void Init_TFT_SPI(SPI_HandleTypeDef* phspi);

    // --------------------------------------------------------------------------
    // Change the screen orientation
    void setTFTRotation(Rotation r);

    // --------------------------------------------------------------------------
    // Send a command to the TFT screen
    inline void SendCommand(uint8_t cmd) {
    	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET); // Set DC pin to command mode
        HAL_SPI_Transmit(m_phspi, &cmd, 1, 500);
    }

    // --------------------------------------------------------------------------
    // Send a single data byte to the TFT screen
    inline void SendData(uint8_t Data) {
    	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET); // Set DC pin to command mode
        HAL_SPI_Transmit(m_phspi, &Data, 1, 500); // Transmit the data
    }

    // --------------------------------------------------------------------------
    // Send a block of data to the TFT screen
    inline void SendData(uint8_t* buff, size_t size) {
    	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET); // Set DC pin to command mode
        HAL_SPI_Transmit(m_phspi, buff, size, 500); // Transmit the data
    }

    // --------------------------------------------------------------------------
    // Send a command using DMA (non-blocking)
    void SendDMACommand(uint8_t *cmd, TFT_DMACallback pEndCallback = nullptr, void* pCallbackContext = nullptr);

    // --------------------------------------------------------------------------
    // Send a block of data using DMA (non-blocking)
    void SendDMAData(uint8_t* buff, size_t size, TFT_DMACallback pEndCallback = nullptr, void* pCallbackContext = nullptr);

    // --------------------------------------------------------------------------
    // Control the Data/Command pin
    inline void setDC() {
    	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET); // Set DC pin to high (data mode)
    }
    inline void resetDC() {
    	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET); // Set DC pin to low (command mode)
    }

    // --------------------------------------------------------------------------
    // Control the Reset pin
    inline void setRST() {
    	HAL_GPIO_WritePin(TFT_Reset_GPIO_Port, TFT_Reset_Pin, GPIO_PIN_SET);  // Set Reset pin to high (inactive)
    }
    inline void resetRST() {
    	HAL_GPIO_WritePin(TFT_Reset_GPIO_Port, TFT_Reset_Pin, GPIO_PIN_RESET);; // Set Reset pin to low (active)
    }

    protected :
    // --------------------------------------------------------------------------
    // Initialize the TFT screen (see #include Controlerxxx.h)
    void Initialise();

    // --------------------------------------------------------------------------
    // Data members
    SPI_HandleTypeDef*  m_phspi;         // SPI handle for communication
};
} // DadGFX
