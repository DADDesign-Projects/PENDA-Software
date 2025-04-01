//------------------------------------------------------------------------
// Copyright(c) 2024 Dad Design.
//------------------------------------------------------------------------

#pragma once

#include <math.h>

namespace DadDSP {

	class PeakDetect {
	public:
		void Initilialise(float SampleRate, float Time, float Triger) {
			m_NbSamples = Time * SampleRate;
			m_Triger = Triger;
		}
		inline bool Process(float Sample) {
			if (Sample < 0) Sample = -Sample;
			if (Sample >= m_Triger) {
				m_Compteur = m_NbSamples;
				return true;
			}
			if(m_Compteur > 0) {
				m_Compteur--;
				return true;
			}else{
				return false;
			}
		}
	protected :
		float m_NbSamples=0;
		float m_Compteur=0;
		float m_Triger = 0;
	};

	class VUMeter {
	public:
		void Initialise(float SampleRate, float Time) {
			m_Meter = 0;
			m_SampleRate = SampleRate;
			m_CtIntegration = 1/(Time * SampleRate);
		}
		float Process(float sample) {
			if (sample < 0) sample = -sample;
			if (sample > m_Meter) {
				m_Meter = sample;
			}
			else {
				m_Meter -= m_CtIntegration;
			}
			return pow(10, m_Meter)/10;
		}
	protected:
		float m_Meter=0;
		float m_SampleRate = 0;
		float m_CtIntegration = 0;
	};
} // DadDSP
