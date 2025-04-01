#pragma once
//====================================================================================
// cEncoder.h
//
// Management of a rotary encoder
// This class provides functionality to interface with a rotary encoder,
// including initialization, reading its position increment, and debouncing the switch.
//
//====================================================================================
#include "main.h"

namespace DadUI {

//***********************************************************************************
// class cEncoder
//***********************************************************************************
class cEncoder
{
  public:
    // --------------------------------------------------------------------------
    // Constructor/destructor
    // The default constructor and destructor initialize and clean up resources.
	cEncoder() {}
    ~cEncoder() {}

    // --------------------------------------------------------------------------
    // Initialization
    // Initializes the rotary encoder by associating GPIO pins for channels A, B,
    // and the switch. Additionally, sets update periods for the encoder and switch.
    // EncoderUpdatePeriod and SwitchUpdatePeriod are expressed in units of 1/SAMPLING_RATE.
    void Init(GPIO_TypeDef* pAPort , uint16_t APIn,
              GPIO_TypeDef* pBPort , uint16_t BPIn,
              GPIO_TypeDef* pSWPort , uint16_t SWPIn,
              uint32_t EncoderUpdatePeriod, uint32_t SwitchUpdatePeriod);

    // --------------------------------------------------------------------------
    // Debouncing and Encoder State Update
    // Handles debouncing for the rotary encoder and its switch. Updates the
    // encoder's increment and switch state based on the elapsed time since
    // the last update.
    ITCM void Debounce();

    // --------------------------------------------------------------------------
    // Get Increment
    // Returns the current increment value of the encoder and resets it to 0.
    inline int8_t getIncrement(){
        uint8_t result = m_Inc;
        m_Inc = 0;
        return result;
    }

    // --------------------------------------------------------------------------
    // Get Switch State
    // Returns the current state of the switch (pressed or released).
    inline uint8_t getSwitchState() const {
        return m_SwitchState;
    }

  protected:
    // --------------------------------------------------------------------------
    // Data Members

    // GPIO configuration for encoder pins (A, B) and the switch
    GPIO_TypeDef* 	m_pGPIO_APort;
    uint16_t 		m_GPIO_APin;
    GPIO_TypeDef* 	m_pGPIO_BPort;
    uint16_t 		m_GPIO_BPin;
    GPIO_TypeDef* 	m_pGPIO_SWPort;
    uint16_t 		m_GPIO_SWPin;

    // Encoder and switch parameters
    uint32_t		m_ctEncoderPeriod;		// Counter for encoder debouncing
    uint32_t		m_EncoderUpdatePeriod; 	// Update period for encoder * 1/SAMPLING_RATE
    uint8_t			m_A; 					// State of encoder A pin
    uint8_t			m_B; 					// State of encoder B pin
    int8_t			m_Inc; 					// Encoder increment value

    uint8_t			m_SwitchState; 			// Current state of the switch
    uint32_t		m_ctSwitchPeriod; 		// Counter for switch debouncing
    uint32_t		m_SwitchUpdatePeriod; 	// Update period for the switch * 1/SAMPLING_RATE
    int32_t			m_ctSwitchIntegrate;

};
} // DadUI
