#pragma once
//====================================================================================
//
// File: cDelayLine.h
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
// Copyright (c) 2024 Dad Design.
//
//====================================================================================
#include "main.h"
#include <math.h>
#include <cstdint>
#include <cstring>


namespace DadDSP {

//***********************************************************************************
// class cDelayLine
// The "DelayLine" class implements a delay line buffer.
// Input -> XXXXXXXXXX -> Output
//***********************************************************************************
class cDelayLine
{
public:
    // --------------------------------------------------------------------------
    // Constructor / destructor
    cDelayLine() {};

    ~cDelayLine() {};

    // --------------------------------------------------------------------------
    // Initializes the FIFO buffer
    void Initialize(float* buffer, uint32_t bufferSize);

    // --------------------------------------------------------------------------
    // Clears the FIFO buffer
    void Clear();

    // --------------------------------------------------------------------------
    // Adds an element to the delay line
    ITCM void Push(float inputSample);

    // --------------------------------------------------------------------------
    // Retrieves a sample without interpolation
    ITCM float Pull(int32_t delay);

    // --------------------------------------------------------------------------
    // Retrieves a sample with interpolation
    ITCM float Pull(float delay);

private:
    // --------------------------------------------------------------------------
    // Data Members
    //

    float* m_Buffer = nullptr;    // Pointer to allocated memory
    int32_t  m_NumElements = 0;   // Number of elements in the buffer
    int32_t  m_CurrentIndex = 0;  // Current index (zero delay position)
};

} // DadDSP
