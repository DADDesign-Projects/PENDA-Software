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
#include "UISystem.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
constexpr uint32_t DelaySerializeID ='Del0'; // SerializeID for Delay Effect
#pragma GCC diagnostic pop


namespace DadEffect {

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
	ITCM void Process(AudioBuffer *pIn, AudioBuffer *pOut, bool OnOff);

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

	// Parameters
	DadUI::cParameter m_Time;			// Time of delays
	DadUI::cParameter m_Repeat;   		// Feedback level for delay 1
	DadUI::cParameter m_Mix;      		// Mix level of delays

	DadUI::cParameter m_SubDelay;		// Subdivision of delay1
	DadUI::cParameter m_RepeatDelay2;	// Feedback level for delay 2
	DadUI::cParameter m_BlendD1D2;		// blend of delay 1 and delay 2

	DadUI::cParameter m_Bass;
	DadUI::cParameter m_Treble;

	DadUI::cParameter m_ModulationDeep; // LFO depth (modulates delay time)
	DadUI::cParameter m_ModulationSpeed;// LFO rate

	// View
	DadUI::cParameterNumNormalView 	m_TimeView;
	DadUI::cParameterNumNormalView 	m_RepeatView;
	DadUI::cParameterNumNormalView 	m_MixView;

	DadUI::cParameterDiscretView 	m_SubDelayView;
	DadUI::cParameterNumNormalView 	m_RepeatDelay2View;
	DadUI::cParameterNumNormalView 	m_BlendD1D2View;

	DadUI::cParameterNumNormalView m_BassView;
	DadUI::cParameterNumNormalView m_TrebleView;

	DadUI::cParameterNumNormalView m_ModulationDeepView;
	DadUI::cParameterNumNormalView m_ModulationSpeedView;

	// UI parameter groups
	DadUI::cUIParameters  m_ItemDelay1Menu;
	DadUI::cUIParameters  m_ItemDelay2Menu;
	DadUI::cUIParameters  m_ItemToneMenu;
	DadUI::cUIParameters  m_ItemLFOMenu;
	DadUI::cUIMemory      m_ItemMenuMemory;  	// Persistent UI memory
	DadUI::cUIImputVolume m_ItemInputVolume;    // Input volume menu

	// Main user interface menu
	DadUI::cUIMenu m_Menu;

	// Tap tempo controller (used for syncing delay times)
	DadUI::cTapTempo m_TapTempo;

	// ==============================================================================
	// DSP Components
	// ==============================================================================
	DadDSP::cDCO 	m_LFO;              // LFO for delay time modulation

	DadDSP::cBiQuad m_BassFilter1;       // High-pass filter (bass EQ) for left delay
	DadDSP::cBiQuad m_TrebleFilter1;     // Low-pass filter (treble EQ) for left delay
	DadDSP::cBiQuad m_BassFilter2;
	DadDSP::cBiQuad m_TrebleFilter2;

	// Stereo delay lines
	DadDSP::cDelayLine m_Delay1LineRight;
	DadDSP::cDelayLine m_Delay1LineLeft;
	DadDSP::cDelayLine m_Delay2LineRight;
	DadDSP::cDelayLine m_Delay2LineLeft;

	float			m_MemMixDelay;		// Memorize MixDelay Value
	float 			m_MemVol1Left;		// Memorize Vol1Left
	float 			m_MemVol1Right;		// Memorize Vol1Right
	float 			m_GainWet;			// GainWet
};

} // namespace DadEffect
