#pragma once
//====================================================================================
// File: DigitalOscillator.h
// Description:
// Digital Controlled Oscillator (DCO) implementation
//====================================================================================
#include <cstdint>
#include <cmath>

namespace DadDSP {
	//***********************************************************************************
	//  cDCO
	//  Implements a Digital Controlled Oscillator (DCO)
	//***********************************************************************************
	class cDCO {

	public:
		// --------------------------------------------------------------------------
		// Initializes the DCO with sample rate, frequency range, and duty cycle
		void Initialize(float sampleRate, float frequency, float minFreq, float maxFreq, float dutyCycle) {
			m_sampleRate = sampleRate;
			m_minFreq = minFreq;
			m_maxFreq = maxFreq;
			m_dcoValue = 0;
			m_dcoStep = 0;

			setNormalizedFreq(frequency);
			setNormalizedDutyCycle(dutyCycle);
		}

		// --------------------------------------------------------------------------
		// Sets the frequency between 0 and 1 (0 = minFreq, 1 = maxFreq)
		inline void setNormalizedFreq(float frequency) {
			// Converts normalized frequency to actual step size
			m_dcoStep = ((m_minFreq + (m_maxFreq - m_minFreq) * frequency) / m_sampleRate);
		}

		// --------------------------------------------------------------------------
		// Sets the frequency in Hz
		inline void setFreq(float frequency) {
			m_dcoStep = frequency / m_sampleRate;
		}
		
		// --------------------------------------------------------------------------
		// Sets the duty cycle of the DCO between 0 and 1
		inline void setNormalizedDutyCycle(float dutyCycle) {
			// Clamps duty cycle to avoid extreme values
			constexpr float minDuty = 0.1f;
			constexpr float maxDuty = 0.9f;
			m_dutyCycle = minDuty + ((maxDuty - minDuty) * dutyCycle);
		}


		// --------------------------------------------------------------------------
		// Advances the oscillator by one step (-1/sampleRate)
		inline void Step() {
			m_dcoValue += m_dcoStep;
			if (m_dcoValue > 1.0f) {
				m_dcoValue -= 1.0f;
			}
		}

		// --------------------------------------------------------------------------
		// Reads the square wave output value
		inline float getSquareValue() {
			constexpr float riseTime = 0.04f;
			constexpr float fallStart = 0.7f;
			constexpr float fallEnd = fallStart + riseTime;

			// Generates a waveform with a defined rising and falling edge
			if (m_dcoValue > fallEnd) {
				return 0;
			} else if (m_dcoValue > fallStart) {
				return 1 - ((m_dcoValue - fallStart) / riseTime);
			} else if (m_dcoValue > riseTime) {
				return 1;
			} else {
				return (m_dcoValue / riseTime);
			}
		}

		// --------------------------------------------------------------------------
		// Reads the square wave output value with duty cycle variation
		inline float getSquareModValue() {
			constexpr float riseTime = 0.04f;

			// Adjusts the waveform based on duty cycle
			if (m_dcoValue > (m_dutyCycle + riseTime)) {
				return 0;
			} else if (m_dcoValue > m_dutyCycle) {
				return 1 - ((m_dcoValue - m_dutyCycle) / riseTime);
			} else if (m_dcoValue > riseTime) {
				return 1;
			} else {
				return (m_dcoValue / riseTime);
			}
		}

		// --------------------------------------------------------------------------
		// Reads the triangle wave output value
		inline float getTriangleValue() {
			// Creates a symmetrical triangle waveform
			if (m_dcoValue > 0.5f) {
				return 2 - (m_dcoValue * 2);
			} else {
				return (m_dcoValue * 2);
			}
		}

		// --------------------------------------------------------------------------
		// Reads the phase-shifted triangle wave output value
		inline float getTriangleValuePhased(float phaseShift) {
		    // Apply phase shift and wrap to [0, 1)
		    float t = fmod(m_dcoValue + phaseShift, 1.0f);
		    if (t < 0.0f) t += 1.0f;

		    // Creates a symmetrical triangle waveform
		    if (t > 0.5f) {
		        return 2.0f - (t * 2.0f);
		    } else {
		        return t * 2.0f;
		    }
		}

		// --------------------------------------------------------------------------
		// Reads the triangle wave output value with duty cycle variation
		inline float getTriangleModValue() {
			// Adjusts the waveform based on duty cycle
			if (m_dcoValue > m_dutyCycle) {
				return (1 - m_dcoValue) / (1 - m_dutyCycle);
			} else {
				return (m_dcoValue / m_dutyCycle);
			}
		}

		// --------------------------------------------------------------------------
		// Reads the sine wave output value
		inline float getSineValue() {
			// Generates a sine wave from phase
			return 0.5f + (sin((m_twoPI * m_dcoValue) + m_halfPI) / 2.0f);
		}

		// --------------------------------------------------------------------------
		// Reads the rectified sine wave output value
		inline float getRectifiedSineValue() {
			// Uses absolute sine to create a half-wave rectified signal
			return sin(m_PI * m_dcoValue);
		}

		// --------------------------------------------------------------------------
		// Sets the oscillator position
		inline void setPosition(float position) {
			m_dcoValue = position;
		}


	private:
		// --------------------------------------------------------------------------
		// Member variables
		const float m_twoPI = 6.28318530717959F;
		const float m_PI = 3.14159265358979F;
		const float m_halfPI = 1.5707963267949F;
		float m_sampleRate = 0.0f;
		float m_minFreq = 0.0f;
		float m_maxFreq = 0.0f;
		float m_dutyCycle;
		float m_dcoValue = 0.0f;
		float m_dcoStep = 0.0f;
	};

} // DadDSP

