//====================================================================================
// Delay.cpp
//
// Audio Delay Effect Module
//
// Copyright(c) 2025 Dad Design.
//====================================================================================

#include "Delay.h"

constexpr float DELAY_MAX_TIME = 1.5f; // Maximum delay time in seconds

// Utility to round up to the next uint
constexpr uint32_t ceil_to_uint(float value) {
    return static_cast<uint32_t>(value + 0.999f);
}

// Calculate buffer size based on sampling rate and max delay time
constexpr uint32_t DELAY_BUFFER_SIZE = ceil_to_uint(SAMPLING_RATE * DELAY_MAX_TIME);

// Allocate delay buffers in SDRAM (extra 100 samples for interpolation safety)
SDRAM_SECTION	float 	__DelayBufferLeft[DELAY_BUFFER_SIZE+100];
SDRAM_SECTION   float 	__DelayBufferRight[DELAY_BUFFER_SIZE+100];
SDRAM_SECTION	float 	__Delay2BufferLeft[DELAY_BUFFER_SIZE+100];
SDRAM_SECTION   float 	__Delay2BufferRight[DELAY_BUFFER_SIZE+100];

namespace DadEffect {

//***********************************************************************************
//  cDelay - Class responsible for managing delay effect parameters, processing audio,
//           and handling user interface interaction.
//
//  Implements a stereo delay effect with:
//    - Primary and secondary delay lines
//    - Feedback controls
//    - LFO-based modulation for time variation
//    - Tone shaping via high-pass and low-pass filters
//    - Full UI control using PendaUI components
//***********************************************************************************

// --------------------------------------------------------------------------
// Initializes parameters, UI, filters, buffers, and LFO
void cDelay::Initialize(){

	// GUI Parameter Initialization ----------------------------------------------------------

	// Time parameter (main delay time in seconds)
	m_Time.Init(0.350f, 0.150f, DELAY_MAX_TIME, 0.05f, 0.01f, nullptr, 0,
	            5.0f * UI_RT_SAMPLING_RATE, 20);

	// Feedback for delay 1
	m_Repeat.Init(45.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
	            0.2f * UI_RT_SAMPLING_RATE, 21);

	// Mix amount for delay 2
	m_MixDelay2.Init(0.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
					 1.0f * UI_RT_SAMPLING_RATE, 22);

	// Feedback for delay 2
	m_Repeat2.Init(45.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
					0.2f * UI_RT_SAMPLING_RATE, 23);

	// Subdivision for delay 2 (musical divisions)
	m_SubDelay2.Init(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, nullptr, 0, 0, 24);

	// Tone controls
	m_Bass.Init(50.0f, 0.0f, 100.0f, 5.0f, 1.0f, BassChange, (uint32_t)this,
	            0.2f * UI_RT_SAMPLING_RATE, 25);

	m_Treble.Init(20.0f, 0.0f, 100.0f, 5.0f, 1.0f, TrebleChange, (uint32_t)this,
	              0.2f * UI_RT_SAMPLING_RATE, 26);

	// Modulation depth and speed
	m_ModulationDeep.Init(10.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
	                      1.0f * UI_RT_SAMPLING_RATE, 27);

	m_ModulationSpeed.Init(1.5f, 0.5f, 10.0f, 0.5f, 0.05f, SpeedChange,
	                       (uint32_t)this, 0.5f * UI_RT_SAMPLING_RATE, 28);

	// Parameter Views Setup -----------------------------------------------------------------

	m_TimeView.Init(&m_Time, "Time", "Time", "s", "Sec.");
	m_RepeatView.Init(&m_Repeat, "Rep.", "Repeat", "%", "%");

	m_MixDelay2View.Init(&m_MixDelay2, "Mix", "Mix", "%", "%");
	m_Repeat2View.Init(&m_Repeat2, "Rep.", "Repeat", "%", "%");
	m_SubDelay2View.Init(&m_SubDelay2, "Sub", "Sub Delay");

	// Discrete values for musical subdivisions
	m_SubDelay2View.AddDiscreteValue("1/8", "1/8");
	m_SubDelay2View.AddDiscreteValue("1/6", "1/6");
	m_SubDelay2View.AddDiscreteValue("1/4", "1/4");
	m_SubDelay2View.AddDiscreteValue("1/3", "1/3");
	m_SubDelay2View.AddDiscreteValue("3/8", "3/8");
	m_SubDelay2View.AddDiscreteValue("5/8", "5/8");
	m_SubDelay2View.AddDiscreteValue("2/3", "2/3");
	m_SubDelay2View.AddDiscreteValue("3/4", "3/4");
	m_SubDelay2View.AddDiscreteValue("5/6", "5/6");
	m_SubDelay2View.AddDiscreteValue("7/8", "7/8");

	m_BassView.Init(&m_Bass, "Bass", "Bass", "%", "%");
	m_TrebleView.Init(&m_Treble, "Treble", "Treble", "%", "%");
	m_ModulationDeepView.Init(&m_ModulationDeep, "Deep", "Mod. Deep", "%", "%");
	m_ModulationSpeedView.Init(&m_ModulationSpeed, "Speed", "Mod. Speed", "Hz", "Hz");

	// Organize parameters into menu groups --------------------------------------------------

	m_ItemDelayMenu.Init(&m_TimeView, nullptr, &m_RepeatView);
	m_ItemDelay2Menu.Init(&m_MixDelay2View, &m_Repeat2View, &m_SubDelay2View);
	m_ItemToneMenu.Init(&m_BassView, nullptr, &m_TrebleView);
	m_ItemLFOMenu.Init(&m_ModulationDeepView, nullptr, &m_ModulationSpeedView);
	m_ItemMenuMemory.Init();

	// Build Main Menu -----------------------------------------------------------------------

	m_Menu.Init();
	m_Menu.addMenuItem(&m_ItemDelayMenu, "Delay1");
	m_Menu.addMenuItem(&m_ItemDelay2Menu, "Delay2");
	m_Menu.addMenuItem(&m_ItemToneMenu, "Tone");
	m_Menu.addMenuItem(&m_ItemLFOMenu, "LFO");
	m_Menu.addMenuItem(&m_ItemMenuMemory, "Mem.");

	// Tap tempo sync (from footswitch)
	m_TapTempo.Init(&DadUI::cPendaUI::m_FootSwitch2, &m_TimeView, DadUI::eTempoType::period);

	// Activate delay UI
	DadUI::cPendaUI::setActiveObject(&m_Menu);

	// LFO and Filters Initialization --------------------------------------------------------

	m_LFO.Initialize(SAMPLING_RATE, m_ModulationSpeed, 1, 10, m_ModulationRatio.getNormalizedValue());

	// Filters and Delay Buffers for Delay 1
	m_BassFilter.Initialize(SAMPLING_RATE, 100, 0.0f, 1.8f, DadDSP::FilterType::HPF);
	m_TrebleFilter.Initialize(SAMPLING_RATE, 100, 0.0f, 1.8f, DadDSP::FilterType::LPF);
	m_DelayLineRight.Initialize(__DelayBufferRight, DELAY_BUFFER_SIZE);
	m_DelayLineRight.Clear();
	m_DelayLineLeft.Initialize(__DelayBufferLeft, DELAY_BUFFER_SIZE);
	m_DelayLineLeft.Clear();

	// Filters and Delay Buffers for Delay 2
	m_BassFilter2.Initialize(SAMPLING_RATE, 100, 0.0f, 1.8f, DadDSP::FilterType::HPF);
	m_TrebleFilter2.Initialize(SAMPLING_RATE, 100, 0.0f, 1.8f, DadDSP::FilterType::LPF);
	m_Delay2LineRight.Initialize(__Delay2BufferRight, DELAY_BUFFER_SIZE);
	m_Delay2LineRight.Clear();
	m_Delay2LineLeft.Initialize(__Delay2BufferLeft, DELAY_BUFFER_SIZE);
	m_Delay2LineLeft.Clear();
}

// --------------------------------------------------------------------------
// Main audio processing function
void cDelay::Process(AudioBuffer *pIn, AudioBuffer *pOut){
	m_LFO.Step(); // Advance LFO phase

	// Compute modulated delay time
	float Delay = m_Time * SAMPLING_RATE * (1 - (m_LFO.getTriangleValue() * m_ModulationDeep * 0.00008f));

	// Compute musical subdivision for delay 2
	float SubDelay;
	switch((uint32_t) m_SubDelay2.getValue()){
		case 0: SubDelay = Delay/8.0f; break;
		case 1: SubDelay = Delay/6.0f; break;
		case 2: SubDelay = Delay/4.0f; break;
		case 3: SubDelay = Delay/3.0f; break;
		case 4: SubDelay = Delay * 3.0f / 8.0f; break;
		case 5: SubDelay = Delay * 5.0f / 8.0f; break;
		case 6: SubDelay = Delay * 2.0f / 3.0f; break;
		case 7: SubDelay = Delay * 3.0f / 4.0f; break;
		case 8: SubDelay = Delay * 5.0f / 6.0f; break;
		case 9: SubDelay = Delay * 7.0f / 8.0f; break;
		default: SubDelay = Delay;
	}

	// --- Delay 1 Processing ---

	float OutLigneRight = m_DelayLineRight.Pull(Delay);
	float OutLigneLeft  = m_DelayLineLeft.Pull(Delay);

	OutLigneRight = m_BassFilter.Process(OutLigneRight, DadDSP::eChannel::Right);
	OutLigneLeft  = m_BassFilter.Process(OutLigneLeft, DadDSP::eChannel::Left);

	OutLigneRight = m_TrebleFilter.Process(OutLigneRight, DadDSP::eChannel::Right);
	OutLigneLeft  = m_TrebleFilter.Process(OutLigneLeft, DadDSP::eChannel::Left);

	m_DelayLineRight.Push(pIn->Right + (OutLigneRight * m_Repeat/100));
	m_DelayLineLeft.Push(pIn->Left + (OutLigneLeft * m_Repeat/100));

	// --- Delay 2 Processing ---

	float OutLigne2Right = m_Delay2LineRight.Pull(SubDelay);
	float OutLigne2Left  = m_Delay2LineLeft.Pull(SubDelay);

	OutLigne2Right = m_BassFilter.Process(OutLigne2Right, DadDSP::eChannel::Right);
	OutLigne2Left  = m_BassFilter.Process(OutLigne2Left, DadDSP::eChannel::Left);

	OutLigne2Right = m_TrebleFilter.Process(OutLigne2Right, DadDSP::eChannel::Right);
	OutLigne2Left  = m_TrebleFilter.Process(OutLigne2Left, DadDSP::eChannel::Left);

	m_Delay2LineRight.Push(pIn->Right + (OutLigne2Right * m_Repeat2/100));
	m_Delay2LineLeft.Push(pIn->Left + (OutLigne2Left * m_Repeat2/100));

	// Mix Delay 1 and Delay 2 with crossfade
	float mix = m_MixDelay2 / 100.0f;
	float gain1 = cosf(mix * 0.5f * M_PI); // Crossfade gain A
	float gain2 = sinf(mix * 0.5f * M_PI); // Crossfade gain B

	pOut->Right = (OutLigneRight * gain1) + (OutLigne2Right * gain2);
	pOut->Left  = (OutLigneLeft * gain1) + (OutLigne2Left * gain2);
}

// --------------------------------------------------------------------------
// Modulation speed callback (updates LFO frequency)
void cDelay::SpeedChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData){
	cDelay * pthis = (cDelay *)CallbackUserData;
	pthis->m_LFO.setFreq(pParameter->getValue());
}

// --------------------------------------------------------------------------
// Bass control callback - sets high-pass filter frequency
#define MIN_BASS_FREQ 30
#define MAX_BASS_FREQ 600
void cDelay::BassChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData){
	cDelay * pthis = (cDelay *)CallbackUserData;
	float Freq = pthis->getLogFrequency(1.0f - pParameter->getNormalizedValue(), MIN_BASS_FREQ, MAX_BASS_FREQ);
	pthis->m_BassFilter.setCutoffFreq(Freq);
	pthis->m_BassFilter.CalculateParameters();
	pthis->m_BassFilter2.setCutoffFreq(Freq);
	pthis->m_BassFilter2.CalculateParameters();
}

// --------------------------------------------------------------------------
// Treble control callback - sets low-pass filter frequency
#define MIN_TREBLE_FREQ 800
#define MAX_TREBLE_FREQ 12000
void cDelay::TrebleChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData){
	cDelay * pthis = (cDelay *)CallbackUserData;
	float Freq = pthis->getLogFrequency(pParameter->getNormalizedValue(), MIN_TREBLE_FREQ, MAX_TREBLE_FREQ);
	pthis->m_TrebleFilter.setCutoffFreq(Freq);
	pthis->m_TrebleFilter.CalculateParameters();
	pthis->m_TrebleFilter2.setCutoffFreq(Freq);
	pthis->m_TrebleFilter2.CalculateParameters();
}

// --------------------------------------------------------------------------
// Returns a frequency from a normalized value using a logarithmic scale
float cDelay::getLogFrequency(float normValue, float freqMin, float freqMax) const{
	float logMin = std::log(freqMin);
	float logMax = std::log(freqMax);
	float logFreq = logMin + normValue * (logMax - logMin);
	return std::exp(logFreq);
};

} // namespace DadEffect
