#pragma once
//****************************************************************************
// Emulation of a 3-wire SPI interface
//
// File: cSoftSPI.h
// Copyright (c) 2025 Dad Design.
//****************************************************************************
#include "main.h"

namespace DadMisc {

// Macros to set and reset GPIO pins
#define SetPIN(PIN) HAL_GPIO_WritePin(m_Port_##PIN, m_Pin_##PIN, GPIO_PIN_SET)
#define ResetPIN(PIN) HAL_GPIO_WritePin(m_Port_##PIN, m_Pin_##PIN, GPIO_PIN_RESET)

// Enumeration representing the transmission states of the SPI interface
enum class eTransState {
	Stop,         // Transmission stopped
	Cs_Down,      // Chip select goes low
	Data_Change,  // Data bit is updated
	Clk_Up,       // Clock signal goes high
	Nop,          // No operation (small delay)
	Clk_Down,     // Clock signal goes low
	Cs_Up         // Chip select goes high (end of transmission)
};

//****************************************************************************
// Class implementing a software-emulated 3-wire SPI interface
//****************************************************************************
class cSoftSPI {
public:
	// -----------------------------------------------------------------------
	// Constructor
	cSoftSPI() {}

	// -----------------------------------------------------------------------
	// Initializes the software SPI with specified GPIO pins and a timer
	void Initialize(GPIO_TypeDef *MOSI_GPIO, uint16_t MOSI_Pin,
			        GPIO_TypeDef *CLK_GPIO, uint16_t CLK_Pin,
			        GPIO_TypeDef *CS_GPIO, uint16_t CS_Pin,
					TIM_HandleTypeDef *phtim);

	// -----------------------------------------------------------------------
	// Timer interrupt callback function to handle SPI transmission
	ITCM void TimerCallback();

	// -----------------------------------------------------------------------
	// Starts a new SPI transmission
	void Transmit(uint32_t Data);

protected:
	// -----------------------------------------------------------------------
	// Data

	// GPIO configurations
	GPIO_TypeDef*		m_Port_MOSI;
	uint16_t 			m_Pin_MOSI;
	GPIO_TypeDef*		m_Port_CLK;
	uint16_t 			m_Pin_CLK;
	GPIO_TypeDef*		m_Port_CS;
	uint16_t 			m_Pin_CS;
	TIM_HandleTypeDef*	m_phtim; 		// Pointer to timer handle

	// Transmission data
	uint32_t			m_Data;     	// Data to be transmitted
	uint32_t			m_DataBit;  	// Current bit being transmitted
	volatile eTransState m_TransState; 	// State of the transmission
	uint32_t			m_NextData;		// Next Data to send
	uint8_t				m_ValidNextData;// Next Data is valid
};

} // namespace DadMisc
