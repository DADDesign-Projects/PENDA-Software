#pragma once
//****************************************************************************
// File: EffectsConfig.h
//
// 
// Copyright (c) 2025 Dad Design.
//****************************************************************************
#include "ID.h"
#include "stdint.h"

#ifndef ACTIVE_EFFECT
// ==========================================================================
// EFFECT SELECTION
// --------------------------------------------------------------------------
// Select the active effect by setting ACTIVE_EFFECT to one of the values
// defined below. Only ONE effect can be active at a time.
// ==========================================================================


#define ACTIVE_EFFECT  EFFECT_DELAY
//#define ACTIVE_EFFECT EFFECT_REVERB
//#define ACTIVE_EFFECT EFFECT_MODULATIONS
//#define ACTIVE_EFFECT EFFECT_TEMPLATE
//#define ACTIVE_EFFECT EFFECT_TEMPLATE_MULTI_MODE


// --- Available effect identifiers (do not modify) -----------------------
#endif

#define EFFECT_DELAY                    1
#define EFFECT_MODULATIONS              2
#define EFFECT_REVERB                   3
#define EFFECT_TEMPLATE                 4
#define EFFECT_TEMPLATE_MULTI_MODE      5


// ==========================================================================
// EFFECT INCLUDE DISPATCH
// --------------------------------------------------------------------------
// Includes the header corresponding to the active effect.
// ==========================================================================

#if ACTIVE_EFFECT == EFFECT_DELAY
    #include "Delay.h"

#elif ACTIVE_EFFECT == EFFECT_MODULATIONS
    #include "cModulations.h"

#elif ACTIVE_EFFECT == EFFECT_REVERB
    #include "Reverb.h"

#elif ACTIVE_EFFECT == EFFECT_TEMPLATE
    #include "cTemplateEffect.h"

#elif ACTIVE_EFFECT == EFFECT_TEMPLATE_MULTI_MODE
    #include "TemplateMultiModeEffect.h"

#else
    #error "ACTIVE_EFFECT is not defined or does not match any known effect."
#endif

//***End of file**************************************************************
