/***************************************************************************/
/* BIQUAD Filter                                                           */
/*                                                                         */
/* Implementation of coefficient calculations provided in the               */
/* Cookbook formulae for audio EQ biquad filter coefficients               */
/* by Robert Bristow-Johnson  <rbj@audioimagination.com>                   */
/***************************************************************************/
#include "BiquadFilter.h"

namespace DadDSP {

// ==========================================================================
// Filter configuration
void cBiQuad::Initialize(float sampleRate, float cutoffFreq, float gainDb, float bandwidth, FilterType type) {
	// Store parameters
	m_sampleRate = sampleRate;
	m_cutoffFreq = cutoffFreq;
	m_gainDb = gainDb;
	m_bandwidth = bandwidth;
	m_type = type;
	CalculateParameters();
}

// ==========================================================================
// Calculate filter parameters
void cBiQuad::CalculateParameters() {
	float a0, a1, a2, b0, b1, b2; // Coefficients

	// Calculate intermediate variables
	float A = std::pow(10, m_gainDb / 40); // Gain in linear scale
	float omega = 2 * kPi * m_cutoffFreq / m_sampleRate; // Angular frequency
	float sn = std::sin(omega); // Sine of omega
	float cs = std::cos(omega); // Cosine of omega
	float alpha = sn * std::sinh(kNaturalLog2 / 2 * m_bandwidth * omega / sn); // Bandwidth parameter
	float beta = std::sqrt(A + A); // Intermediate variable for shelving filters

	// Calculate coefficients based on filter type
	switch (m_type) {
	case FilterType::LPF:
	case FilterType::LPF24:
		b0 = (1 - cs) / 2;
		b1 = 1 - cs;
		b2 = (1 - cs) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case FilterType::HPF:
	case FilterType::HPF24:
		b0 = (1 + cs) / 2;
		b1 = -(1 + cs);
		b2 = (1 + cs) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case FilterType::BPF:
		b0 = alpha;
		b1 = 0;
		b2 = -alpha;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case FilterType::NOTCH:
		b0 = 1;
		b1 = -2 * cs;
		b2 = 1;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
	case FilterType::PEQ:
		b0 = 1 + (alpha * A);
		b1 = -2 * cs;
		b2 = 1 - (alpha * A);
		a0 = 1 + (alpha / A);
		a1 = -2 * cs;
		a2 = 1 - (alpha / A);
		break;
	case FilterType::LSH:
		b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
		b1 = 2 * A * ((A - 1) - (A + 1) * cs);
		b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
		a0 = (A + 1) + (A - 1) * cs + beta * sn;
		a1 = -2 * ((A - 1) + (A + 1) * cs);
		a2 = (A + 1) + (A - 1) * cs - beta * sn;
		break;
	case FilterType::HSH:
	default:
		b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
		b1 = -2 * A * ((A - 1) + (A + 1) * cs);
		b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
		a0 = (A + 1) - (A - 1) * cs + beta * sn;
		a1 = 2 * ((A - 1) - (A + 1) * cs);
		a2 = (A + 1) - (A - 1) * cs - beta * sn;
		break;

	}

	// Precompute coefficients
    __disable_irq();
	m_a0 = b0 / a0;
	m_a1 = b1 / a0;
	m_a2 = b2 / a0;
	m_a3 = a1 / a0;
	m_a4 = a2 / a0;
    __enable_irq();
}

// ==========================================================================
// Calculate gain for a given frequency
float cBiQuad::GainDb(float freq) {
	float Phi = 4 * std::sin(std::pow(kPi * freq / m_sampleRate, 2.0));

	float num = ((m_a0 * m_a2 * (Phi * Phi)) + std::pow(m_a0 + m_a1 + m_a2, 2.0) - (((m_a0 * m_a1) + (4 * m_a0 * m_a2) + (m_a1 * m_a2)) * Phi));
	float denum = (m_a4 * std::pow(Phi, 2.0)) + std::pow(m_a3 + m_a4 + 1, 2.0) - (((m_a3 * m_a4) + m_a3 + (4 * +m_a4)) * Phi);

	if ((m_type == FilterType::LPF24) || (m_type == FilterType::HPF24)) {
		return (20 * std::log10(std::sqrt(num / denum))) + (20 * log10(std::sqrt(num / denum)));
	}
	else {
		return 20 * std::log10(std::sqrt(num / denum));
	}
}

// ==========================================================================
// process filter
float cBiQuad::Process(float sample, sFilterState &FilterState) {

	// Apply biquad filter
	float result = (   m_a0 * sample)
	                + (m_a1 * FilterState.x1)
					+ (m_a2 * FilterState.x2)
					- (m_a3 * FilterState.y1)
	                - (m_a4 * FilterState.y2);

	// Store and shift previous states
	// Input states
	FilterState.x2 = FilterState.x1;
	FilterState.x1 = sample;

	// Output states
	FilterState.y2 = FilterState.y1;
	FilterState.y1 = result;

	return result;
}

} // namespace DadDSP
