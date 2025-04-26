#pragma once
//====================================================================================
// Delay.h
//
// Declaration of the Delay effect class, implementing stereo delays with feedback,
// LFO modulation, and tone control (bass/treble). Includes full user interface
// integration via PendaUI.
// Copyright(c) 2025 Dad Design.
//====================================================================================
#include "main.h"
#include "PendaUI.h"
#include "UIComponent.h"
#include "Parameter.h"
#include "cDCO.h"
#include "BiquadFilter.h"
#include "cDelayLine.h"

namespace DadEffect {

// ------------------------------------------------------------------------
// UI-triggered parameter callbacks.
// These functions are called when parameters are modified by the user.
// ------------------------------------------------------------------------
//void SpeedChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);
//void BassChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);
//void TrebleChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);


//***********************************************************************************
//  cDelay
//
//  Implements a stereo delay effect with:
//    - Primary and secondary delay lines
//    - Feedback controls
//    - LFO-based modulation for time variation
//    - Tone shaping via high-pass and low-pass filters
//    - Full UI control using PendaUI components
//***********************************************************************************

class cDelay {
public:
	// --------------------------------------------------------------------------
	// Constructor (initializes nothing by itself).
	cDelay() {};

	// --------------------------------------------------------------------------
	// Initializes DSP components and user interface parameters.
	void Initialize();

	// --------------------------------------------------------------------------
	// Audio processing function: processes one input/output audio buffer.
	ITCM void Process(AudioBuffer *pIn, AudioBuffer *pOut);

	// --------------------------------------------------------------------------
	// Static callbacks triggered when UI parameters change.
	static void SpeedChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);
	static void BassChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);
	static void TrebleChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);

protected:
	// --------------------------------------------------------------------------
	// Maps a normalized value [0.0, 1.0] to a logarithmic frequency range.
	float getLogFrequency(float normValue, float freqMin, float freqMax) const;

	// ==============================================================================
	// User Interface Components
	// ==============================================================================

	// Parameters (linked to UI and DSP)
	DadUI::cParameter m_Time;           // Delay time (ms or synced)
	DadUI::cParameter m_Repeat;         // Feedback level for main delay
	DadUI::cParameter m_MixDelay2;      // Mix level of secondary delay
	DadUI::cParameter m_Repeat2;        // Feedback level of secondary delay
	DadUI::cParameter m_SubDelay2;      // Subdivision for second delay (sync options)
	DadUI::cParameter m_Bass;           // Low-frequency tone control
	DadUI::cParameter m_Treble;         // High-frequency tone control
	DadUI::cParameter m_ModulationDeep; // LFO depth (modulates delay time)
	DadUI::cParameter m_ModulationSpeed;// LFO rate
	DadUI::cParameter m_ModulationRatio;// LFO waveform ratio (may be fixed)

	// UI views (visual representations of the parameters)
	DadUI::cParameterNumNormalView m_TimeView;
	DadUI::cParameterNumNormalView m_RepeatView;
	DadUI::cParameterNumNormalView m_MixDelay2View;
	DadUI::cParameterNumNormalView m_Repeat2View;
	DadUI::cParameterDiscretView   m_SubDelay2View;
	DadUI::cParameterNumNormalView m_BassView;
	DadUI::cParameterNumNormalView m_TrebleView;
	DadUI::cParameterNumNormalView m_ModulationDeepView;
	DadUI::cParameterNumNormalView m_ModulationSpeedView;

	// UI parameter groups
	DadUI::cUIParameters m_ItemDelayMenu;
	DadUI::cUIParameters m_ItemDelay2Menu;
	DadUI::cUIParameters m_ItemToneMenu;
	DadUI::cUIParameters m_ItemLFOMenu;
	DadUI::cUIMemory     m_ItemMenuMemory;  // Persistent UI memory

	// Main user interface menu
	DadUI::cUIMenu m_Menu;

	// Tap tempo controller (used for syncing delay times)
	DadUI::cTapTempo m_TapTempo;

	// ==============================================================================
	// DSP Components
	// ==============================================================================

	DadDSP::cDCO m_LFO;                 // LFO for delay time modulation
	DadDSP::cBiQuad m_BassFilter;       // High-pass filter (bass EQ) for left delay
	DadDSP::cBiQuad m_TrebleFilter;     // Low-pass filter (treble EQ) for left delay
	DadDSP::cBiQuad m_BassFilter2;      // High-pass filter (bass EQ) for right delay
	DadDSP::cBiQuad m_TrebleFilter2;    // Low-pass filter (treble EQ) for right delay

	// Stereo delay lines (main and secondary)
	DadDSP::cDelayLine m_DelayLineRight;
	DadDSP::cDelayLine m_DelayLineLeft;
	DadDSP::cDelayLine m_Delay2LineRight;
	DadDSP::cDelayLine m_Delay2LineLeft;
};

} // namespace DadEffect
