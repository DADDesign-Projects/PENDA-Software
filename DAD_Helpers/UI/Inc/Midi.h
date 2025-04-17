#pragma once
//====================================================================================
// Midi.h
// Management of MIDI interface
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "main.h"
#include <vector>
#include <functional>

// Size of the MIDI ring buffer
#define MIDI_BUFFER_SIZE 20
#define MULTI_CHANNEL 0xFF

// Function type definitions for MIDI callbacks
using ControlChangeCallback = std::function<void(uint8_t control, uint8_t value, uint32_t userData)>;
using ProgramChangeCallback = std::function<void(uint8_t program, uint32_t userData)>;
using NoteChangeCallback = std::function<void(uint8_t OnOff, uint8_t note, uint8_t velocity, uint32_t userData)>;

// Structures to store callback information
struct CC_CallbackEntry {
    uint8_t control;                 // Control Change number (0-127)
    uint32_t userData;				 // User data
    ControlChangeCallback callback;  // Function to call when this CC is received
};

struct PC_CallbackEntry {
    uint32_t userData;				 // User data
    ProgramChangeCallback callback;  // Function to call when this PC is received
};

struct Note_CallbackEntry {
    uint32_t userData;				 // User data
    NoteChangeCallback callback;     // Function to call when Note On/Off is received
};

namespace DadUI {
//***********************************************************************************
// class cMidi
// MIDI message parser and event handler with callback registration
//***********************************************************************************
class cMidi {
public:
    // --------------------------------------------------------------------------
    // Constructor/destructor
    cMidi(){}

    // --------------------------------------------------------------------------
    // Initialize the MIDI interface
    // @param phuart - Pointer to UART handle that will receive MIDI data
    // @param Channel - MIDI listening channel
    void Initialize(UART_HandleTypeDef* phuart, uint8_t Channel = MULTI_CHANNEL);

    // --------------------------------------------------------------------------
    // Change MIDI channel
    // @param Channel - new MIDI listening channel
    void ChangeChanel(uint8_t Channel){
    	m_Channel = Channel;
    }

    // --------------------------------------------------------------------------
    // Process any MIDI messages in the buffer
    // Should be called regularly from the main loop
    void ProcessBuffer();

    // --------------------------------------------------------------------------
    // Register a callback for a specific Control Change message
    // @param channel - MIDI channel (0-15)
    // @param control - Control Change number (0-127)
    // @param pCallback - Function to call when this CC is received
    void addControlChangeCallback(uint8_t control, uint32_t userData, ControlChangeCallback pCallback);

    // --------------------------------------------------------------------------
    // Remove a previously registered Control Change callback
    // @param pCallback - The callback function to remove
    void removeControlChangeCallback(ControlChangeCallback pCallback);

    // --------------------------------------------------------------------------
    // Register a callback for a specific Program Change message
    // @param channel - MIDI channel (0-15)
    // @param program - Program Change number (0-127)
    // @param pCallback - Function to call when this PC is received
    void addProgramChangeCallback(uint32_t userData, ProgramChangeCallback pCallback);

    // --------------------------------------------------------------------------
    // Remove a previously registered Program Change callback
    // @param pCallback - The callback function to remove
    void removeProgramChangeCallback(ProgramChangeCallback pCallback);

    // --------------------------------------------------------------------------
    // Register a callback for Note On/Off messages on a specific channel
    // @param channel - MIDI channel (0-15)
    // @param pCallback - Function to call when Note messages are received
    void addNoteChangeCallback(uint32_t userData, NoteChangeCallback pCallback);

    // --------------------------------------------------------------------------
    // Remove a previously registered Note callback
    // @param pCallback - The callback function to remove
    void removeNoteChangeCallback(NoteChangeCallback pCallback);

protected:
    // --------------------------------------------------------------------------
    // Handle Note On MIDI messages
    // @param channel - MIDI channel (0-15)
    // @param note - MIDI note number (0-127)
    // @param velocity - Note velocity (0-127)
    void OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) const ;

    // --------------------------------------------------------------------------
    // Handle Note Off MIDI messages
    // @param channel - MIDI channel (0-15)
    // @param note - MIDI note number (0-127)
    // @param velocity - Release velocity (0-127)
    void OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) const ;

    // --------------------------------------------------------------------------
    // Handle Control Change MIDI messages
    // @param channel - MIDI channel (0-15)
    // @param control - Control Change number (0-127)
    // @param value - Control value (0-127)
    void OnControlChange(uint8_t channel, uint8_t control, uint8_t value) const ;

    // --------------------------------------------------------------------------
    // Handle Program Change MIDI messages
    // @param channel - MIDI channel (0-15)
    // @param program - Program number (0-127)
    void OnProgramChange(uint8_t channel, uint8_t program) const ;

    // --------------------------------------------------------------------------
    // Determine the number of data bytes expected for a given status byte
    // @param status - MIDI status byte
    // @return Number of data bytes (1 or 2)
    uint8_t getDataLength(uint8_t status) const ;

    // --------------------------------------------------------------------------
    // Parse and dispatch a complete MIDI message
    // @param status - MIDI status byte
    // @param data - Array of data bytes
    void parseMessage(uint8_t status, uint8_t* data) const ;

    // --------------------------------------------------------------------------
    // Member variables
    UART_HandleTypeDef* m_phuart;              // UART interface for MIDI
    uint8_t m_Channel;							// Current MIDI channel
    uint8_t m_status;                          // Current MIDI status byte
    uint8_t m_data[2];                         // Data bytes for current message
    uint8_t m_dataIndex;                       // Number of data bytes received
    std::vector<CC_CallbackEntry> m_ccCallbacks;     // Control Change callbacks
    std::vector<PC_CallbackEntry> m_pcCallbacks;     // Program Change callbacks
    std::vector<Note_CallbackEntry> m_noteCallbacks; // Note On/Off callbacks
};
}// namespace DadUI
