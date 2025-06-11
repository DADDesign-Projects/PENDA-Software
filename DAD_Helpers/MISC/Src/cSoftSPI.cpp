//****************************************************************************
// Emulation of a 3-wire SPI interface
//
// File: cSoftSPI.cpp
// Copyright (c) 2025 Dad Design.
//****************************************************************************
#include "cSoftSPI.h"

namespace DadMisc {

//****************************************************************************
// Class implementing a software-emulated 3-wire SPI interface
//****************************************************************************

// -----------------------------------------------------------------------
// Initializes the software SPI with specified GPIO pins and a timer
void cSoftSPI::Initialize(GPIO_TypeDef *MOSI_GPIO, uint16_t MOSI_Pin,
				GPIO_TypeDef *CLK_GPIO, uint16_t CLK_Pin,
				GPIO_TypeDef *CS_GPIO, uint16_t CS_Pin,
				TIM_HandleTypeDef *phtim) {

	// Store pin configurations
	m_Port_MOSI = MOSI_GPIO;
	m_Pin_MOSI 	= MOSI_Pin;
	m_Port_CLK 	= CLK_GPIO;
	m_Pin_CLK 	= CLK_Pin;
	m_Port_CS 	= CS_GPIO;
	m_Pin_CS 	= CS_Pin;
	m_phtim 	= phtim;

	// Initialize transmission parameters
	m_DataBit = 0;
	m_Data = 0;
	m_TransState = eTransState::Stop;
	m_ValidNextData = 0;

	// Set initial pin states
	SetPIN(CS);   	// Chip select high (inactive)
	ResetPIN(CLK); 	// Clock low
	ResetPIN(MOSI); // MOSI low
}

// -----------------------------------------------------------------------
// Timer interrupt callback function to handle SPI transmission
void cSoftSPI::TimerCallback() {
	switch (m_TransState) {
	case eTransState::Stop:
		// Do nothing if transmission is stopped
		return;
		break;
	case eTransState::Cs_Down:
		// Lower chip select to start transmission
		ResetPIN(CS);
		m_DataBit = 0x80000000; // Start with the highest bit
		m_TransState = eTransState::Data_Change;
		break;
	case eTransState::Data_Change:
		// Set MOSI according to the current bit in data
		if ((m_DataBit & m_Data) == 0) {
			ResetPIN(MOSI);
		} else {
			SetPIN(MOSI);
		}
		m_DataBit >>= 1; // Shift to the next bit
		m_TransState = eTransState::Clk_Up;
		break;
	case eTransState::Clk_Up:
		// Raise the clock to signal data read
		SetPIN(CLK);
		m_TransState = eTransState::Nop;
		break;
	case eTransState::Nop:
		// No operation, used as a small delay
		m_TransState = eTransState::Clk_Down;
		break;
	case eTransState::Clk_Down:
		// Lower the clock, checking if all bits are sent
		ResetPIN(CLK);
		if (m_DataBit == 0) {
			m_TransState = eTransState::Cs_Up; 		 // End transmission
		} else {
			m_TransState = eTransState::Data_Change; // Continue with the next bit
		}
		break;
	case eTransState::Cs_Up:
		// Raise chip select to indicate end of transmission
		SetPIN(CS);
		if(m_ValidNextData == 1){
			m_Data = m_NextData;       				// Store the data to send
			m_TransState = eTransState::Cs_Down; 	// Begin transmission
			m_ValidNextData = 0;
		}else{
			m_TransState = eTransState::Stop;
			// Stop the timer interrupt
			HAL_TIM_Base_Stop_IT(m_phtim);
		}
		break;
	}
}

// -----------------------------------------------------------------------
// Starts a new SPI transmission
void cSoftSPI::Transmit(uint32_t Data) {
    __disable_irq();
	if(m_TransState != eTransState::Stop){
		m_NextData = Data;
		m_ValidNextData = 1;
	}else{
		m_Data = Data;           				// Store the data to send
		m_TransState = eTransState::Cs_Down; 	// Begin transmission
		HAL_TIM_Base_Start_IT(m_phtim); 		// Start timer interrupts
	}
	__enable_irq();
}

} // namespace DadMisc
