#pragma once
/***************************************************************************/
/* BIQUAD Filter                                                           */
/*                                                                         */
/* Implementation of coefficient calculations provided in the               */
/* Cookbook formulae for audio EQ biquad filter coefficients               */
/* by Robert Bristow-Johnson  <rbj@audioimagination.com>                   */
/***************************************************************************/
#include "main.h"
#include <cstdint>
#include <cmath>


namespace DadDSP {
constexpr float kNaturalLog2 = 0.69314718055994530942; 	// Natural logarithm of 2
constexpr float kPi = 3.14159265358979323846;  			// Value of PI

enum class FilterType : int {
    LPF = 0,   // Low pass filter
    LPF24,     // Low pass filter 24 dB
    HPF,       // High pass filter
    HPF24,     // High pass filter 24 dB
    BPF,       // Band pass filter
    NOTCH,     // Notch Filter
    PEQ,       // Peaking band EQ filter
    LSH,       // Low shelf filter
    HSH        // High shelf filter
};

struct sFilterState{
	float x1; // Input
	float x2;
	float y1; // Output
	float y2;
};

enum class eChannel{
	Left,
	Right
};

class cBiQuad {
public:
    // ==========================================================================
    // Filter configuration
    void Initialize(float sampleRate, float cutoffFreq, float gainDb, float bandwidth, FilterType type);

    // ==========================================================================
    // Calculate filter parameters
    ITCM void CalculateParameters();

    // ==========================================================================
    // Calculate gain for a given frequency
    float GainDb(float freq);

    // ==========================================================================
    // Process Filter
    inline float Process(float sample, eChannel Channel){

    	switch(Channel){
    	case eChannel::Left :
    		 if ((m_type == FilterType::LPF24) || (m_type == FilterType::HPF24)) {
    			 sample = Process(sample, m_FilterState[0]);
    		 }
    		return Process(sample, m_FilterState[1]);
    		break;
    	case eChannel::Right :
    		if ((m_type == FilterType::LPF24) || (m_type == FilterType::HPF24)) {
    			sample = Process(sample, m_FilterState[2]);
    		}
    		return Process(sample, m_FilterState[3]);
    		break;
    	}
    	return sample;
    }
    

    // ==========================================================================
    // Set filter parameters
    inline void setSampleRate(float sampleRate) { m_sampleRate = sampleRate; }
    inline void setCutoffFreq(float cutoffFreq) { m_cutoffFreq = cutoffFreq; }
    inline void setGainDb(float gainDb) { m_gainDb = gainDb; }
    inline void setBandwidth(float bandwidth) { m_bandwidth = bandwidth; }
    inline void setType(FilterType type) { m_type = type; }

    // ==========================================================================
    // Get filter parameters
    inline float getSampleRate() { return m_sampleRate; }
    inline float getCutoffFreq() { return m_cutoffFreq; }
    inline float getGainDb() { return m_gainDb; }
    inline float getBandwidth() { return m_bandwidth; }
    inline FilterType getType() { return m_type; }
	
protected:
    // ==========================================================================
    // Filter Mono or Stereo CH1 signal: must be called for each sample
    ITCM float Process(float sample, sFilterState &FilterState);

    // Filter parameters
    float m_sampleRate;
    float m_cutoffFreq;
    float m_gainDb;
    float m_bandwidth;
    FilterType m_type;


    // Coefficients
    // Coefficients
    float m_a0 = 0;
    float m_a1 = 0;
    float m_a2 = 0;
    float m_a3 = 0;
    float m_a4 = 0;
	
    // Previous samples storage
    sFilterState	m_FilterState[4]={};
    

};
} // namespace DadDSP
