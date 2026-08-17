//==============================================================================
// File        : HardwareAndCoDefines.h
// Description :
// Hardware and application configuration definitions used by the
// DAD_FORGE library.
//
// Copyright (c) 2026 DadDesign-Projects.
//==============================================================================
#pragma once

#include "main.h"
#include "sections.h"

//**********************************************************************************
// The Dry and Wet channel volumes are adjusted on PENDA via two potentiometers
// that are hardwired directly into the analog audio circuitry.
//**********************************************************************************
#define HARD_DRYWET

//**********************************************************************************
// General defines
//**********************************************************************************
#define GUI_UPDATE_MS      300     // GUI update interval in milliseconds
#define GUI_FAST_UPDATE_MS 10      // GUI fast process update interval in milliseconds
#define MONITOR_UPDATE_MS  200     // Monitor update interval in milliseconds
#define GENERAL_UPDATE_MS  100     // General system update interval in milliseconds

//**********************************************************************************
// Daisy Seed Revision
//**********************************************************************************
enum class eHardRev_t{
	Rev5,
	Rev7
};

//**********************************************************************************
// DryWet Parameter
//**********************************************************************************
constexpr float MIN_DRY = -45.0f;  // Minimum dry signal level cDryWet
constexpr float MAX_DRY  = 0.0f;   // Maximum dry signal level cDryWet
constexpr float FAD_TIME = 5.0f;   // Dry/Wet Fading time in second

