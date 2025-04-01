//====================================================================================
// cEncoder.cpp
//
// Management of a rotary encoder
// This class provides functionality to interface with a rotary encoder,
// including initialization, reading its position increment, and debouncing the switch.
//
//====================================================================================
#include "cEncoder.h"

namespace DadUI {

//***********************************************************************************
// class cEncoder
//***********************************************************************************
// --------------------------------------------------------------------------
// Initialization
// Initializes the rotary encoder by associating GPIO pins for channels A, B,
// and the switch. Additionally, sets update periods for the encoder and switch.
// EncoderUpdatePeriod and SwitchUpdatePeriod are expressed in units of 1/SAMPLING_RATE.
void cEncoder::Init(GPIO_TypeDef* pAPort , uint16_t APIn,
          GPIO_TypeDef* pBPort , uint16_t BPIn,
          GPIO_TypeDef* pSWPort , uint16_t SWPIn,
          uint32_t EncoderUpdatePeriod, uint32_t SwitchUpdatePeriod){

	m_pGPIO_APort = pAPort;
	m_GPIO_APin = APIn;
	m_pGPIO_BPort = pBPort;
	m_GPIO_BPin = BPIn;
	m_pGPIO_SWPort = pSWPort;
	m_GPIO_SWPin = SWPIn;
	m_A = 0;
	m_B = 0;
	m_Inc = 0;
	m_EncoderUpdatePeriod =  EncoderUpdatePeriod;
	m_SwitchUpdatePeriod =  SwitchUpdatePeriod;
	m_ctEncoderPeriod = 0; 			// Counter for encoder debouncing
	m_ctSwitchPeriod = 0;  			// Counter for switch debouncing
	m_ctSwitchIntegrate = 0;		// Initialize the Counter for switch signal integration
}

// --------------------------------------------------------------------------
// Debouncing and Encoder State Update
// Handles debouncing for the rotary encoder and its switch. Updates the
// encoder's increment and switch state based on the elapsed time since
// the last update.
void cEncoder::Debounce(){

	// Encoder processing
	m_ctEncoderPeriod++;

	if (m_ctEncoderPeriod > m_EncoderUpdatePeriod) {
		m_ctEncoderPeriod = 0;
		// Shift and update the state of the encoder pins
		m_A = ((m_A << 1) | (uint8_t) HAL_GPIO_ReadPin(m_pGPIO_APort, m_GPIO_APin)) & 0x03;
		m_B = ((m_B << 1) | (uint8_t) HAL_GPIO_ReadPin(m_pGPIO_BPort, m_GPIO_BPin)) & 0x03;

		m_Inc = 0; // Reset increment
		// Determine increment direction
		if ((m_A == 0x02) && (m_B == 0x00)) {
			m_Inc += 1; // Clockwise rotation
		} else if ((m_B == 0x02) && (m_A == 0x00)) {
			m_Inc -= 1; // Counter-clockwise rotation
		}
	}

	// 	Switch debouncing
	constexpr int32_t INTEGRATION_FACTOR = 10;
	m_ctSwitchPeriod++;
	if (m_ctSwitchPeriod > m_SwitchUpdatePeriod) {
		m_ctSwitchPeriod = 0;

		//  Integrates the input switch signal over time.
		if (HAL_GPIO_ReadPin(m_pGPIO_SWPort, m_GPIO_SWPin) == GPIO_PIN_SET) {
			m_ctSwitchIntegrate++;
			if (m_ctSwitchIntegrate > INTEGRATION_FACTOR) {
				m_ctSwitchIntegrate = INTEGRATION_FACTOR;
				m_SwitchState = 0; // Switch released
			}
		} else {
			m_ctSwitchIntegrate--;
			if (m_ctSwitchIntegrate < -INTEGRATION_FACTOR) {
				m_ctSwitchIntegrate = -INTEGRATION_FACTOR;
				m_SwitchState = 1; // Switch pressed
			}
		}
	}
}

} // DadUI
