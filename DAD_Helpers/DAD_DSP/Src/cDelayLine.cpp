//====================================================================================
// Copyright (c) 2024 Dad Design.
//
// File: cDelayLine.cpp
// Description: This class implements a delay line buffer, commonly used in
//              digital signal processing (DSP) applications for audio effects,
//              feedback loops, and time-based signal modifications.
//
//              The delay line allows inserting new samples and retrieving past
//              samples either with or without interpolation.
//
// Usage:
//   - Call `Initialize()` with a preallocated buffer to set up the delay line.
//   - Use `push()` to add a new sample.
//   - Use `pull()` to retrieve a sample with or without interpolation.
//
// Notes:
//   - Interpolation is linear, which may be improved for higher fidelity.
//
//====================================================================================
#include "cDelayLine.h"

namespace DadDSP {

//***********************************************************************************
// class cDelayLine
//***********************************************************************************

// --------------------------------------------------------------------------
// Initializes the FIFO buffer
void cDelayLine::Initialize(float* buffer, uint32_t bufferSize) {
	m_Buffer = buffer;
	m_NumElements = bufferSize + 5; // Adds an offset for safety
	m_CurrentIndex = 0;
}

// --------------------------------------------------------------------------
// Clears the FIFO buffer
void cDelayLine::Clear() {
	memset(m_Buffer, 0, (m_NumElements * sizeof(float)));
}

// --------------------------------------------------------------------------
// Adds an element to the delay line
void cDelayLine::Push(float inputSample) {
	if (m_Buffer) {
		m_CurrentIndex++;
		if (m_CurrentIndex == m_NumElements)
			m_CurrentIndex = 0; // Wrap around when reaching the end

		m_Buffer[m_CurrentIndex] = inputSample;
	}
}

// --------------------------------------------------------------------------
// Retrieves a sample without interpolation
float cDelayLine::Pull(int32_t delay) {
	//assert(delay < m_NumElements);
	if (m_Buffer) {
		int32_t outputIndex = m_CurrentIndex - delay;
		if (outputIndex < 0)
			outputIndex += m_NumElements; // Handle negative index wrap-around

		return m_Buffer[outputIndex];
	}
	else return 0.0f;
}

// --------------------------------------------------------------------------
// Retrieves a sample with interpolation
float cDelayLine::Pull(float delay) {
	//assert(delay < m_NumElements);

	if (m_Buffer) {
		float interpFactor = delay - floor(delay); // Fractional part of the delay
		if (interpFactor == 0)
			return Pull((int32_t)delay); // No interpolation needed

		// Compute indices for interpolation
		int32_t index1 = m_CurrentIndex - static_cast<int32_t>(ceil(delay));
		if (index1 < 0)
			index1 += m_NumElements;

		int32_t index2 = index1 + 1;
		if (index2 == m_NumElements)
			index2 = 0; // Wrap around to the beginning of the buffer

		// Fetch the two adjacent samples
		float sample1 = m_Buffer[index1];
		float sample2 = m_Buffer[index2];

		// Perform linear interpolation
		float interpolatedValue = sample2 + ((sample1 - sample2) * interpFactor);

		return interpolatedValue;
	}
	else return 0.0f;
}

}//DadDSP
