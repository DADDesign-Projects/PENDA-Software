//====================================================================================
// Midi.cpp
// Management of MIDI interface
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "Midi.h"

// Aligned buffer for DMA + MIDI ring buffer
// NO_CACHE_RAM ensures the buffer is not cached for proper DMA operation
NO_CACHE_RAM uint8_t __RxData[2];        // DMA receive buffer (double buffered)
uint8_t __MidiBuffer[MIDI_BUFFER_SIZE];  // Ring buffer to store MIDI bytes
uint8_t __MidiBufferWriteIndex = 0;      // Write position in the ring buffer
uint8_t __MidiBufferReadIndex  = 0;      // Read position in the ring buffer

// HAL UART Reception Complete Callback
// Called when DMA has filled the second half of __RxData
ITCM void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    __MidiBuffer[__MidiBufferWriteIndex] = __RxData[1];
    __MidiBufferWriteIndex = (__MidiBufferWriteIndex + 1) % MIDI_BUFFER_SIZE;
}

// HAL UART Reception Half Complete Callback
// Called when DMA has filled the first half of __RxData
ITCM void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart){
    __MidiBuffer[__MidiBufferWriteIndex] = __RxData[0];
    __MidiBufferWriteIndex = (__MidiBufferWriteIndex + 1) % MIDI_BUFFER_SIZE;
}

namespace DadUI {

//***********************************************************************************
// class cMidi
// MIDI message parser and event handler with callback registration
//***********************************************************************************

// --------------------------------------------------------------------------
// Initialize the MIDI interface
// @param phuart - Pointer to UART handle that will receive MIDI data
// @param Channel - MIDI listening channel
void cMidi::Initialize(UART_HandleTypeDef* phuart, uint8_t Channel){
	m_phuart = phuart;
	m_Channel = Channel;
	__MidiBufferWriteIndex = 0;   // Reset buffer indices
	__MidiBufferReadIndex = 0;
	m_status = 0;                 // Clear the current MIDI status byte
	m_dataIndex = 0;              // Reset data byte counter
	m_ccCallbacks.clear();        // Clear any existing callbacks

	// Start DMA reception in circular mode (continuously receives data)
	HAL_UART_Receive_DMA(phuart, __RxData, 2);
}

// --------------------------------------------------------------------------
// Process any MIDI messages in the buffer
// Should be called regularly from the main loop
void cMidi::ProcessBuffer(){
	// Process all available bytes in the buffer
	while (__MidiBufferReadIndex != __MidiBufferWriteIndex) {
		uint8_t byte = __MidiBuffer[__MidiBufferReadIndex];
		__MidiBufferReadIndex = (__MidiBufferReadIndex + 1) % MIDI_BUFFER_SIZE;

		if (byte & 0x80) {
			// This is a status byte (MSB set)
			m_status = byte;
			m_dataIndex = 0;
		} else {
			// This is a data byte (MSB clear)
			if (m_status == 0) continue; // Skip if no valid status yet

			// Store the data byte
			m_data[m_dataIndex++] = byte;

			// Check if we have received all expected data bytes for this message
			uint8_t expected = getDataLength(m_status);
			if (m_dataIndex >= expected) {
				parseMessage(m_status, m_data);  // Process the complete MIDI message
				m_dataIndex = 0;                 // Reset for next message
			}
		}
	}
}

// --------------------------------------------------------------------------
// Register a callback for a specific Control Change message
// @param channel - MIDI channel (0-15)
// @param control - Control Change number (0-127)
// @param pCallback - Function to call when this CC is received
void cMidi::addControlChangeCallback(uint8_t control, uint32_t userData, ControlChangeCallback pCallback) {
	// Add the entry to the vector of callbacks
	m_ccCallbacks.push_back({control, userData, pCallback});
}

// --------------------------------------------------------------------------
// Remove a previously registered Control Change callback
// @param pCallback - The callback function to remove
void cMidi::removeControlChangeCallback(ControlChangeCallback pCallback) {
	for (auto it = m_ccCallbacks.begin(); it != m_ccCallbacks.end(); ) {
		// Compare function pointers to find matching callback
		auto targetCurrent = it->callback.template target<void(*)(uint8_t, uint32_t)>();
		auto targetToRemove = pCallback.template target<void(*)(uint8_t, uint32_t)>();
		if (targetCurrent && targetToRemove && (*targetCurrent == *targetToRemove)) {
			it = m_ccCallbacks.erase(it);  // Remove this entry
		} else {
			++it;  // Move to next entry
		}
	}
}

// --------------------------------------------------------------------------
// Register a callback for a specific Program Change message
// @param channel - MIDI channel (0-15)
// @param program - Program Change number (0-127)
// @param pCallback - Function to call when this PC is received
void cMidi::addProgramChangeCallback(uint32_t userData, ProgramChangeCallback pCallback) {
	// Add the entry to the vector of callbacks
	m_pcCallbacks.push_back({userData, pCallback});
}

// --------------------------------------------------------------------------
// Remove a previously registered Program Change callback
// @param pCallback - The callback function to remove
void cMidi::removeProgramChangeCallback(ProgramChangeCallback pCallback) {
	for (auto it = m_pcCallbacks.begin(); it != m_pcCallbacks.end(); ) {
		// Compare function pointers to find matching callback
		auto targetCurrent = it->callback.template target<void(*)(uint32_t)>();
		auto targetToRemove = pCallback.template target<void(*)(uint32_t)>();
		if (targetCurrent && targetToRemove && (*targetCurrent == *targetToRemove)) {
			it = m_pcCallbacks.erase(it);  // Remove this entry
		} else {
			++it;  // Move to next entry
		}
	}
}

// --------------------------------------------------------------------------
// Register a callback for Note On/Off messages on a specific channel
// @param channel - MIDI channel (0-15)
// @param pCallback - Function to call when Note messages are received
void cMidi::addNoteChangeCallback(uint32_t userData, NoteChangeCallback pCallback) {
	// Add the entry to the vector of callbacks
	m_noteCallbacks.push_back({userData, pCallback});
}

// --------------------------------------------------------------------------
// Remove a previously registered Note callback
// @param pCallback - The callback function to remove
void cMidi::removeNoteChangeCallback(NoteChangeCallback pCallback) {
	for (auto it = m_noteCallbacks.begin(); it != m_noteCallbacks.end(); ) {
		// Compare function pointers to find matching callback
		auto targetCurrent = it->callback.template target<void(*)(uint8_t, uint8_t, uint8_t, uint32_t)>();
		auto targetToRemove = pCallback.template target<void(*)(uint8_t, uint8_t, uint8_t, uint32_t)>();
		if (targetCurrent && targetToRemove && (*targetCurrent == *targetToRemove)) {
			it = m_noteCallbacks.erase(it);  // Remove this entry
		} else {
			++it;  // Move to next entry
		}
	}
}

// --------------------------------------------------------------------------
// Handle Note On MIDI messages
// @param channel - MIDI channel (0-15)
// @param note - MIDI note number (0-127)
// @param velocity - Note velocity (0-127)
void cMidi::OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) const {
	// Call all registered callbacks
	if((channel == m_Channel) || (m_Channel == MULTI_CHANNEL)){
		for (auto& entry : m_noteCallbacks) {
			entry.callback(1, note, velocity, entry.userData );  // 1 = Note On
		}
	}
}

// --------------------------------------------------------------------------
// Handle Note Off MIDI messages
// @param channel - MIDI channel (0-15)
// @param note - MIDI note number (0-127)
// @param velocity - Release velocity (0-127)
void cMidi::OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) const {
	// Call all registered callbacks
	if((channel == m_Channel) || (m_Channel == MULTI_CHANNEL)){
		for (auto& entry : m_noteCallbacks) {
			entry.callback(0, note, velocity, entry.userData);   // 0 = Note On
		}
	}
}

// --------------------------------------------------------------------------
// Handle Control Change MIDI messages
// @param channel - MIDI channel (0-15)
// @param control - Control Change number (0-127)
// @param value - Control value (0-127)
void cMidi::OnControlChange(uint8_t channel, uint8_t control, uint8_t value) const {
	// Call all registered callbacks for this CC number
	if((channel == m_Channel) || (m_Channel == MULTI_CHANNEL)){
		for (auto& entry : m_ccCallbacks) {
			if (entry.control == control) {
				entry.callback(control, value, entry.userData);
			}
		}
	}
}

// --------------------------------------------------------------------------
// Handle Program Change MIDI messages
// @param channel - MIDI channel (0-15)
// @param program - Program number (0-127)
void cMidi::OnProgramChange(uint8_t channel, uint8_t program) const {
	// Call all registered callbacks for this channel
	if((channel == m_Channel) || (m_Channel == MULTI_CHANNEL)){
		for (auto& entry : m_pcCallbacks) {
			entry.callback(program, entry.userData);
		}
	}
}

// --------------------------------------------------------------------------
// Determine the number of data bytes expected for a given status byte
// @param status - MIDI status byte
// @return Number of data bytes (1 or 2)
uint8_t cMidi::getDataLength(uint8_t status) const {
	switch (status & 0xF0) {
		case 0xC0: // Program Change
		case 0xD0: // Channel Pressure (Aftertouch)
			return 1;
		default:   // All other message types
			return 2;
	}
}

// --------------------------------------------------------------------------
// Parse and dispatch a complete MIDI message
// @param status - MIDI status byte
// @param data - Array of data bytes
void cMidi::parseMessage(uint8_t status, uint8_t* data) const {
	uint8_t type = status & 0xF0;      // Message type (Note On, CC, etc.)
	uint8_t channel = status & 0x0F;   // MIDI channel (0-15)

	switch (type) {
		case 0x80: // Note Off
			OnNoteOff(channel, data[0], data[1]);
			break;
		case 0x90: // Note On (velocity 0 = Note Off)
			if (data[1]) OnNoteOn(channel, data[0], data[1]);
			else OnNoteOff(channel, data[0], 0);
			break;
		case 0xB0: // Control Change
			OnControlChange(channel, data[0], data[1]);
			break;
		case 0xC0: // Program Change
			OnProgramChange(channel, data[0]);
			break;
		// Other MIDI message types could be added here
		// (e.g. Pitch Bend, Aftertouch, System messages)
	}
}

}// namespace DadUI
