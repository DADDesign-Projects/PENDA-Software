//====================================================================================
// cVolume.cpp
//
// Volume control module for digital audio output.
// Uses a software SPI interface and timer for precise transmission.
//
// Copyright (c) 2025 Dad Design.
//====================================================================================
#include "cVolume.h"

DadMisc::cSoftSPI 	DadMisc::cVolume::m_SoftSPI;       // Software SPI instance

// -----------------------------------------------------------------------
// Hardware timer interrupt callback
// This will be triggered by TIM6 periodically.
// It delegates to the static TimerCallback of the volume class.
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	DadMisc::cVolume::m_SoftSPI.TimerCallback();
}

namespace DadMisc {

//***********************************************************************************
// class cVolume
//
// Controls 4 independent volume channels via SPI.
// Uses software SPI to send values to a digital volume controller.
//***********************************************************************************

// -----------------------------------------------------------------------
// Initialization
// Configures the SPI interface and sets all volume levels to 0.
void cVolume::init(TIM_HandleTypeDef * phtim){
	// Initialize the software SPI interface
	m_SoftSPI.Initialize(SSPI_DATA_GPIO_Port, SSPI_DATA_Pin,
						 SSPI_CLK_GPIO_Port, SSPI_CLK_Pin,
						 SSPI_CS_GPIO_Port, SSPI_CS_Pin,
						 phtim);
	MuteOn();
	// Reset volume to 0 on all channels
	m_Volumes.Volume = 0x0;

	// Transmit initial volume state
	m_SoftSPI.Transmit(m_Volumes.Volume);
}

// -----------------------------------------------------------------------
// Sets Volume1 (left/right) and transmits it
void cVolume::Volume1Change(float VolumeLeft, float VolumeRight) {
	m_Volumes.Vol1 = static_cast<uint8_t>(VolumeLeft);
	m_Volumes.Vol2 = static_cast<uint8_t>(VolumeRight);

	m_SoftSPI.Transmit(m_Volumes.Volume);  // Send over SPI
}

// -----------------------------------------------------------------------
// Sets Volume2 (left/right) and transmits it
void cVolume::Volume2Change(float VolumeLeft, float VolumeRight) {
	m_Volumes.Vol3 = static_cast<uint8_t>(VolumeLeft);
	m_Volumes.Vol4 = static_cast<uint8_t>(VolumeRight);
	m_SoftSPI.Transmit(m_Volumes.Volume);  // Send over SPI
}

float cVolume::MixDryWet(float Mix){

	float GainDry =  20.0f * std::log10(cosf(Mix * 0.5f * M_PI)+0.0001);
	if(GainDry<-255) GainDry=-255;
	int16_t VolGain = static_cast<int16_t>((GainDry*2)+0.5f);

	int16_t VolDryLeft =  m_Volumes.Vol1 + VolGain;
	if(VolDryLeft < 0) VolDryLeft=0;

	int16_t VolDryRight = m_Volumes.Vol2 + VolGain;
	if(VolDryRight < 0) VolDryRight=0;

	Volume2Change(VolDryLeft, VolDryRight);

	return sinf(Mix * 0.5f * M_PI);
}

void cVolume::MuteOn(){
	HAL_GPIO_WritePin(AUDIO_MUTE_GPIO_Port, AUDIO_MUTE_Pin, GPIO_PIN_RESET);
}

void cVolume::MuteOff(){
	HAL_GPIO_WritePin(AUDIO_MUTE_GPIO_Port, AUDIO_MUTE_Pin, GPIO_PIN_SET);
}


} // namespace DadMisc


