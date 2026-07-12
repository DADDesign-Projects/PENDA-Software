#pragma once
//****************************************************************************
// File: @EffectsConfig.h
//
// 
// Copyright (c) 2025-2026  DadDesign-Project.
//****************************************************************************
#define HARD_DRYWET     // "he Dry and Wet channel volumes are adjusted on PENDA via two potentiometers that are hardwired directly into the analog audio circuitry.

// --- Available effect identifiers (do not modify) -----------------------
#define EFFECT_DELAY                    1
#define EFFECT_MODULATIONS              2
#define EFFECT_REVERB                   3
#define EFFECT_TEMPLATE                 4
#define EFFECT_TEMPLATE_MULTI_MODE      5

#if __has_include("EffectsConfig.h")
	#include "EffectsConfig.h"
#else
	#include "ID.h"
	#include "stdint.h"
	#include "AudioManager.h"
	#include "cBypassOnOffManager.h"

	#define ACTIVE_EFFECT 0
	#define DECLARE_EFFECT DummyEffect __Effect
	#define EFFECT_NAME "Dummy"
	#define EFFECT_VERSION "Version 1.0"
	#define EFFECT_SPLATCH_SCREEN "Template.png"
	constexpr uint32_t EFFECT_BUILD =   BUILD_ID('D', 'U', 'M', '1');
	// ======================= Dummy Effect configuration ===========================
	class DummyEffect {
	public:
		DummyEffect()=default;
	    void Initialize(){};
	    void Process(AudioBuffer *pIn, AudioBuffer *pOut, DadGUI::eEffectState_t State, bool Silence){};
	    uint32_t getEffectID(){return 0;}
	};

#endif


//***End of file**************************************************************
