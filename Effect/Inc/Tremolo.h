#pragma once
//====================================================================================
// Tremolo.h
//
// Declaration of the Tremolo effect class.
// Implements a tremolo effect with optional vibrato and a configurable LFO.
//
// Copyright (c) 2025 Dad Design.
//====================================================================================

#include "main.h"
#include "PendaUI.h"
#include "UIComponent.h"
#include "Parameter.h"
#include "cDCO.h"
#include "cDelayLine.h"
#include "UISystem.h"

namespace DadEffect {

//***********************************************************************************
//  cTremolo
//
//  Main class for the Tremolo effect.
//  Handles parameter initialization, audio processing, LFO modulation,
//  vibrato (pitch modulation), and user interface integration.
//***********************************************************************************

class cTremolo {
public:
	// --------------------------------------------------------------------------
	// Constructor
	// Note: Does not perform any initialization. Call Initialize() explicitly.
	cTremolo() {};

	// --------------------------------------------------------------------------
	// Initializes DSP modules, LFO, delay buffers, and UI parameters.
	void Initialize();

	// --------------------------------------------------------------------------
	// Audio processing function
	// Applies tremolo and vibrato to the input audio buffer.
	ITCM void Process(AudioBuffer *pIn, AudioBuffer *pOut, bool OnOff);

	// --------------------------------------------------------------------------
	// UI Callbacks
	// Triggered when related parameters are changed by the user.
	static void SpeedChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);
	static void RatioChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);

protected:

	// ==============================================================================
	// User Interface Components
	// ==============================================================================

	// DSP-linked parameters (user controlled)
	DadUI::cParameter m_Freq;           // LFO frequency (Hz)
	DadUI::cParameter m_TremoloDeep;    // Tremolo depth (% volume modulation)
	DadUI::cParameter m_VibratoDeep;    // Vibrato depth (% pitch modulation)
	DadUI::cParameter m_LFOShape;       // LFO waveform shape (e.g., triangle, square)
	DadUI::cParameter m_LFORatio;       // LFO duty cycle (%)

	// Parameter views (UI widgets to display/edit parameters)
	DadUI::cParameterNumNormalView      m_FreqView;
	DadUI::cParameterNumNormalView      m_TremoloDeepView;
	DadUI::cParameterNumNormalView      m_VibratoDeepView;
	DadUI::cParameterDiscretView        m_LFOShapeView;
	DadUI::cParameterNumLeftRightView   m_LFORatioView;

	// UI parameter groupings (menu sections)
	DadUI::cUIParameters m_ItemTremoloMenu;   // Group for tremolo/vibrato parameters
	DadUI::cUIParameters m_ItemLFOMenu;       // Group for LFO shape and ratio
	DadUI::cUIMemory     m_ItemMenuMemory;    // Persistent parameter storage
	DadUI::cUIImputVolume m_ItemInputVolume;  // Input volume menu

	// Main menu container
	DadUI::cUIMenu m_Menu;

	// Tap tempo controller
	// Allows syncing LFO frequency to footswitch tempo.
	DadUI::cTapTempo m_TapTempo;

	// ==============================================================================
	// DSP Components
	// ==============================================================================

	DadDSP::cDCO m_LFO;                 // Low-Frequency Oscillator for modulation

	// Delay lines for vibrato (stereo processing)
	DadDSP::cDelayLine m_ModulationLineRight;
	DadDSP::cDelayLine m_ModulationLineLeft;

	// Compensation factor to maintain vibrato depth regardless of LFO frequency
	float m_CoefComp = 0.0f;
};

} // namespace DadEffect
