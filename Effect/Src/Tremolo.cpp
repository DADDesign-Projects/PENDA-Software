//====================================================================================
// Tremolo.cpp
//
// Audio Tremolo Effect Module
//
// Copyright (c) 2025 Dad Design.
//====================================================================================

#include "Tremolo.h"

constexpr float DELAY_MAX_TIME = 0.02f; // Maximum modulation delay time in seconds
constexpr float FREQ_MIN = 0.5f;        // Minimum LFO frequency in Hz
constexpr float FREQ_MAX = 9.0f;        // Maximum LFO frequency in Hz

// Utility function to round a float up to the next unsigned integer
constexpr uint32_t ceil_to_uint(float value) {
    return static_cast<uint32_t>(value + 0.999f);
}

// Compute delay buffer size based on the sampling rate and max delay time
constexpr uint32_t DELAY_BUFFER_SIZE = ceil_to_uint(SAMPLING_RATE * DELAY_MAX_TIME);

// Allocate modulation delay buffers in external SDRAM (+100 samples for safe interpolation)
SDRAM_SECTION float __ModulationBufferLeft[DELAY_BUFFER_SIZE + 100];
SDRAM_SECTION float __ModulationBufferRight[DELAY_BUFFER_SIZE + 100];

namespace DadEffect {

//***********************************************************************************
//  cTremolo - Class handling the tremolo effect parameters, audio processing,
//             and user interface integration.
//***********************************************************************************

// --------------------------------------------------------------------------
// Initializes UI parameters, LFO, delay buffers, and menu interface
void cTremolo::Initialize(){

	// ---------------- Parameter Initialization ----------------

	// LFO Frequency
	m_Freq.Init(5.0f, FREQ_MIN, FREQ_MAX, 0.5f, 0.1f, SpeedChange, (uint32_t)this,
	            5.0f * UI_RT_SAMPLING_RATE, 20);

	// Tremolo Depth
	m_TremoloDeep.Init(45.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
	                   0.5f * UI_RT_SAMPLING_RATE, 21);

	// Vibrato Depth (used for delay-based pitch modulation)
	m_VibratoDeep.Init(0.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
	                   0.5f * UI_RT_SAMPLING_RATE, 22);

	// LFO Shape (0: Triangle, 1: Square)
	m_LFOShape.Init(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, nullptr, 0,
	                0.0f, 23);

	// LFO Duty Cycle Ratio
	m_LFORatio.Init(50.0f, 0.0f, 100.0f, 5.0f, 1.0f, RatioChange, (uint32_t)this,
	                0.5f * UI_RT_SAMPLING_RATE, 24);

	// ---------------- View Setup ----------------
	m_FreqView.Init(&m_Freq, "Freq", "Frequency", "Hz", "Hz");
	m_TremoloDeepView.Init(&m_TremoloDeep, "Tremolo", "Tremolo depth", "%", "%");
	m_VibratoDeepView.Init(&m_VibratoDeep, "Vibrato", "Vibrato depth", "%", "%");
	m_LFORatioView.Init(&m_LFORatio, "Ratio", "Ratio", "%", "%");
	m_LFOShapeView.Init(&m_LFOShape, "Shape", "Shape");
	m_LFOShapeView.AddDiscreteValue("Rect.", "Rectangular");
	m_LFOShapeView.AddDiscreteValue("Square", "Square");

	// ---------------- Menu Grouping ----------------
	m_ItemTremoloMenu.Init(&m_TremoloDeepView, &m_VibratoDeepView, &m_FreqView);
	m_ItemLFOMenu.Init(&m_LFOShapeView, &m_LFORatioView, &m_FreqView);
	m_ItemMenuMemory.Init();
	m_ItemInputVolume.Init();

	// ---------------- Main Menu Configuration ----------------
	m_Menu.Init();
	m_Menu.addMenuItem(&m_ItemTremoloMenu, "Main");
	m_Menu.addMenuItem(&m_ItemLFOMenu, "LFO");
	m_Menu.addMenuItem(&m_ItemMenuMemory, "Mem.");
	m_Menu.addMenuItem(&m_ItemInputVolume, "Input");

	// Sync with footswitch for tap-tempo
	m_TapTempo.Init(&DadUI::cPendaUI::m_FootSwitch2, &m_FreqView, DadUI::eTempoType::frequency);

	// Activate the menu interface
	DadUI::cPendaUI::setActiveObject(&m_Menu);

	// ---------------- LFO and Delay Buffer Initialization ----------------
	m_LFO.Initialize(SAMPLING_RATE, m_Freq, 1, 10, m_LFORatio.getNormalizedValue());

	m_ModulationLineRight.Initialize(__ModulationBufferRight, DELAY_BUFFER_SIZE);
	m_ModulationLineRight.Clear();

	m_ModulationLineLeft.Initialize(__ModulationBufferLeft, DELAY_BUFFER_SIZE);
	m_ModulationLineLeft.Clear();
}

// --------------------------------------------------------------------------
// Audio processing routine: applies volume and pitch modulation
void cTremolo::Process(AudioBuffer *pIn, AudioBuffer *pOut, bool OnOff){
	m_LFO.Step(); // Update LFO phase
	m_ItemInputVolume.Process(pIn);		// Input volume VU-Meter

	float Left;
	float Right;
	if(false == OnOff){
		Left = 0.0f;
		Right = 0.0f;
	}else{
		Left = pIn->Left;
		Right = pIn->Right;
	}

	float VolumeModulation = 0.0f;
	switch(static_cast<uint32_t>(m_LFOShape.getValue())){
		case 0:
			VolumeModulation = m_LFO.getTriangleModValue() * m_TremoloDeep / 100.0f;
			break;
		case 1:
			VolumeModulation = m_LFO.getSquareModValue() * m_TremoloDeep / 100.0f;
			break;
	}

	// Compute vibrato delay in samples.
	// The delay is modulated by the LFO sine wave, scaled by the user-defined depth,
	// and adjusted with a compensation factor to keep the vibrato range independent of LFO frequency.
	float LFOSin = m_LFO.getSineValue();
	float Delay = DELAY_BUFFER_SIZE * LFOSin * m_CoefComp * m_VibratoDeep / 100.0f;

	// Push current samples to delay line and read modulated delayed output
	m_ModulationLineLeft.Push(Left);
	m_ModulationLineRight.Push(Right);

	pOut->Left = m_ModulationLineLeft.Pull(Delay) * VolumeModulation;
	pOut->Right = m_ModulationLineRight.Pull(Delay) * VolumeModulation;
}

// --------------------------------------------------------------------------
// Callback to update the LFO frequency based on user interaction
void cTremolo::SpeedChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData){
	cTremolo *pthis = reinterpret_cast<cTremolo *>(CallbackUserData);
	pthis->m_LFO.setFreq(pParameter->getValue());
	// Compute compensation factor to keep vibrato depth consistent across LFO frequencies.
	// This avoids a shallower vibrato effect when the LFO speed increases.
	pthis->m_CoefComp = (2 * FREQ_MIN) / (2 * (FREQ_MIN + (FREQ_MAX - FREQ_MIN) * pParameter->getNormalizedValue()));
}

// --------------------------------------------------------------------------
// Callback to update the LFO duty cycle ratio
void cTremolo::RatioChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData){
	cTremolo *pthis = reinterpret_cast<cTremolo *>(CallbackUserData);
	pthis->m_LFO.setNormalizedDutyCycle(pParameter->getNormalizedValue());
}

} // namespace DadEffect
