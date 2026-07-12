#pragma once
//****************************************************************************
// File: Options.h
//
// Copyright (c) 2025-2026  DadDesign-Project.
//****************************************************************************

// ==========================================================================
// USB / MONITORING CONFIGURATION
// --------------------------------------------------------------------------
// Select the desired options below. Only ONE value per option must be set.
// ==========================================================================

#define USB_DEVICE      DEVICE_MIDI
// #define USB_DEVICE   DEVICE_COM

#define MONITORING      MON_YES
//#define MONITORING   MON_NO


// --- Option values (do not modify) ---------------------------------------
#define DEVICE_MIDI     1
#define DEVICE_COM      2

#define MON_YES         1
#define MON_NO          0


// ==========================================================================
// USB DEVICE DISPATCH
// --------------------------------------------------------------------------
// In DEBUG builds, DEVICE_COM enables the palette builder instead of MIDI.
// In release builds, USB MIDI is always used.
// ==========================================================================

#ifdef DEBUG
    #if USB_DEVICE == DEVICE_COM
        #define PALETTE_BUILDER
    #else
        #define USB_MIDI
    #endif
#else
    #define USB_MIDI
#endif


// ==========================================================================
// MONITORING DISPATCH
// --------------------------------------------------------------------------
// Monitoring is only available in DEBUG builds, and only if enabled above.
// ==========================================================================

#if defined(DEBUG) && (MONITORING == MON_YES)
    #define MONITOR
#endif
//***End of file**************************************************************
