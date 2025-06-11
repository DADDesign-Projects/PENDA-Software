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
SDRAM_SECTION	float 	__Delay2BufferLeft[(DELAY_BUFFER_SIZE)+100];
SDRAM_SECTION   float 	__Delay2BufferRight[(DELAY_BUFFER_SIZE)+100];

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

	// Member data Initialization ----------------------------------------------------------
	m_MemMixDelay = 0.0f;		// Memorize MixDelay Value
	m_MemVol1Left = 0.0f;		// Memorize Vol1Left
	m_MemVol1Right = 0.0f;		// Memorize Vol1Right
	m_GainWet = 0.0f;			// GainWet

	m_BassFilter1.Initialize(SAMPLING_RATE, 100, 0.0f, 1.8f, DadDSP::FilterType::HPF);
	m_TrebleFilter1.Initialize(SAMPLING_RATE, 1000, 0.0f, 1.8f, DadDSP::FilterType::LPF);
	m_BassFilter2.Initialize(SAMPLING_RATE, 100, 0.0f, 1.8f, DadDSP::FilterType::HPF);
	m_TrebleFilter2.Initialize(SAMPLING_RATE, 1000, 0.0f, 1.8f, DadDSP::FilterType::LPF);

	m_Delay1LineRight.Initialize(__DelayBufferRight, DELAY_BUFFER_SIZE);
	m_Delay1LineRight.Clear();
	m_Delay1LineLeft.Initialize(__DelayBufferLeft, DELAY_BUFFER_SIZE);
	m_Delay1LineLeft.Clear();

	m_Delay2LineRight.Initialize(__Delay2BufferRight, DELAY_BUFFER_SIZE);
	m_Delay2LineRight.Clear();
	m_Delay2LineLeft.Initialize(__Delay2BufferLeft, DELAY_BUFFER_SIZE);
	m_Delay2LineLeft.Clear();

	m_LFO.Initialize(SAMPLING_RATE, 0.5, 1, 10, 0.5f);

	// GUI Parameter Initialization ----------------------------------------------------------

	// Delay 1 ----------------------
	m_Time.Init(0.450f, 0.150f, DELAY_MAX_TIME, 0.05f, 0.01f, nullptr, 0,
	            5.0f * UI_RT_SAMPLING_RATE, 20, DelaySerializeID);

	// Feedback for delay 1
	m_Repeat.Init(30.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
	            0.2f * UI_RT_SAMPLING_RATE, 21, DelaySerializeID);

	// Mix delay 1
	m_Mix.Init(10.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
					 1.0f * UI_RT_SAMPLING_RATE, 22, DelaySerializeID);
	// Delay 2 ----------------------
	// Subdivision of delay1
	m_SubDelay.Init(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, nullptr, 0, 0, 23, DelaySerializeID);

	// Feedback level for delay 2
	m_RepeatDelay2.Init(0.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
            0.2f * UI_RT_SAMPLING_RATE, 24, DelaySerializeID);

	// blend of delay 1 and delay 2
	m_BlendD1D2.Init(0.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
			 1.0f * UI_RT_SAMPLING_RATE, 25, DelaySerializeID);

	// Tone controls -----------------
	m_Bass.Init(50.0f, 0.0f, 100.0f, 5.0f, 1.0f, BassChange, (uint32_t)this,
	            0.2f * UI_RT_SAMPLING_RATE, 26, DelaySerializeID);

	m_Treble.Init(50.0f, 0.0f, 100.0f, 5.0f, 1.0f, TrebleChange, (uint32_t)this,
	              0.2f * UI_RT_SAMPLING_RATE, 27, DelaySerializeID);

	// Modulation depth and speed
	m_ModulationDeep.Init(10.0f, 0.0f, 100.0f, 5.0f, 1.0f, nullptr, 0,
	                      1.0f * UI_RT_SAMPLING_RATE, 28, DelaySerializeID);

	m_ModulationSpeed.Init(1.5f, 0.5f, 10.0f, 0.5f, 0.05f, SpeedChange,
	                       (uint32_t)this, 0.5f * UI_RT_SAMPLING_RATE, 29, DelaySerializeID);

	// Parameter Views Setup -----------------------------------------------------------------
	m_TimeView.Init(&m_Time, "Time", "Time", "s", "second");
	m_RepeatView.Init(&m_Repeat, "Rep.", "Repeat", "%", "%");
	m_MixView.Init(&m_Mix, "Mix", "Mix", "%", "%");

	// Discrete values for musical subdivisions
	m_SubDelayView.Init(&m_SubDelay, "Mix", "Mix");
	m_SubDelayView.Init(&m_SubDelay, "Sub", "Sub Delay");
	m_SubDelayView.AddDiscreteValue("1/8", "1/8");
	m_SubDelayView.AddDiscreteValue("1/6", "1/6");
	m_SubDelayView.AddDiscreteValue("1/4", "1/4");
	m_SubDelayView.AddDiscreteValue("1/3", "1/3");
	m_SubDelayView.AddDiscreteValue("3/8", "3/8");
	m_SubDelayView.AddDiscreteValue("5/8", "5/8");
	m_SubDelayView.AddDiscreteValue("2/3", "2/3");
	m_SubDelayView.AddDiscreteValue("3/4", "3/4");
	m_SubDelayView.AddDiscreteValue("5/6", "5/6");
	m_SubDelayView.AddDiscreteValue("7/8", "7/8");

	m_RepeatDelay2View.Init(&m_RepeatDelay2, "Rep.", "Repeat", "%", "%");
	m_BlendD1D2View.Init(&m_BlendD1D2, "Blend", "Blend D1/D2", "%", "%");

	m_BassView.Init(&m_Bass, "Bass", "Bass", "%", "%");
	m_TrebleView.Init(&m_Treble, "Treble", "Treble", "%", "%");

	m_ModulationDeepView.Init(&m_ModulationDeep, "Deep", "Mod. Deep", "%", "%");
	m_ModulationSpeedView.Init(&m_ModulationSpeed, "Speed", "Mod. Speed", "Hz", "Hz");

	// Organize parameters into menu groups --------------------------------------------------
#ifdef PENDAI
	m_ItemDelay1Menu.Init(&m_TimeView, nullptr, &m_RepeatView);
#elif defined(PENDAII)
	m_ItemDelay1Menu.Init(&m_TimeView, &m_RepeatView, &m_MixView);
#endif
	m_ItemDelay2Menu.Init(&m_SubDelayView, &m_RepeatDelay2View, &m_BlendD1D2View);

	m_ItemToneMenu.Init(&m_BassView, nullptr, &m_TrebleView);

	m_ItemLFOMenu.Init(&m_ModulationDeepView, nullptr, &m_ModulationSpeedView);

	m_ItemInputVolume.Init();
	m_ItemMenuMemory.Init(DelaySerializeID);

	// Build Main Menu -----------------------------------------------------------------------
	m_Menu.Init();
	m_Menu.addMenuItem(&m_ItemDelay1Menu, "Delay1");
	m_Menu.addMenuItem(&m_ItemDelay2Menu, "Delay2");
	m_Menu.addMenuItem(&m_ItemToneMenu, "Tone");
	m_Menu.addMenuItem(&m_ItemLFOMenu, "LFO");
	m_Menu.addMenuItem(&m_ItemMenuMemory, "Mem.");
	m_Menu.addMenuItem(&m_ItemInputVolume, "Input");

	// Tap tempo sync (from footswitch)
	m_TapTempo.Init(&DadUI::cPendaUI::m_FootSwitch2, &m_TimeView, DadUI::eTempoType::period);

	// Activate delay UI
	DadUI::cPendaUI::setActiveObject(&m_Menu);

	// LFO and Filters Initialization --------------------------------------------------------
	m_LFO.Initialize(SAMPLING_RATE, 0.5, 1, 10, 0.5f);

	DadUI::cPendaUI::m_Volumes.MuteOff();
}

// --------------------------------------------------------------------------
// Main audio processing function
void cDelay::Process(AudioBuffer *pIn, AudioBuffer *pOut, bool OnOff){
	m_LFO.Step();
	m_ItemInputVolume.Process(pIn);		// Input volume VU-Meter

	float Left;
	float Right;
	if(false == OnOff){
		Left = 0.0f;
		Right = 0.0f;
	}else{
		Left = pIn->Left;
		Right = pIn->Right;
	}

	// Compute modulated delay time
	float LFO1 =  m_LFO.getTriangleValue();
	float LFO2 =  m_LFO.getTriangleValuePhased(0.25f);
	float Delay = m_Time * SAMPLING_RATE;
	float DelayL = Delay - (LFO1  *  m_ModulationDeep * 0.8);
	float DelayR = Delay - (LFO2  *  m_ModulationDeep * 0.8);

	// Compute musical subdivision for delay 2
	float SubDelayL;
	float SubDelayR;

	switch((uint32_t) m_SubDelay.getValue()){
		case 0:
			SubDelayL = DelayL/8.0f;		 	// 0.125
			SubDelayR = DelayR/8.0f;		 	// 0.125
			break;
		case 1:
			SubDelayL = DelayL/6.0f;          // 0.166
			SubDelayR = DelayR/6.0f;          // 0.166
			break;
		case 2:
			SubDelayL = DelayL/4.0f;          // 0.250
			SubDelayR = DelayR/4.0f;          // 0.250
			break;
		case 3:
			SubDelayL = DelayL/3.0f; 		 	// 0.333
			SubDelayR = DelayR/3.0f; 		 	// 0.333
			break;
		case 4:
			SubDelayL = DelayL * 3.0f / 8.0f; //0.375
			SubDelayR = DelayR * 3.0f / 8.0f; //0.375
			break;
		case 5:
			SubDelayL = DelayL * 5.0f / 8.0f; // 0.625
			SubDelayR = DelayR * 5.0f / 8.0f; // 0.625
			break;
		case 6:
			SubDelayL = DelayL * 2.0f / 3.0f; // 0.666
			SubDelayR = DelayR * 2.0f / 3.0f; // 0.666
			break;
		case 7:
			SubDelayL = DelayL * 3.0f / 4.0f; // 0.750
			SubDelayR = DelayR * 3.0f / 4.0f; // 0.750
			break;
		case 8:
			SubDelayL = DelayL * 5.0f / 6.0f;	// 0.833
			SubDelayR = DelayR * 5.0f / 6.0f;	// 0.833
			break;
		case 9:
			SubDelayL = DelayL * 7.0f / 8.0f;	// 0.875
			SubDelayR = DelayR * 7.0f / 8.0f;	// 0.875
			break;
		default:
			SubDelayL = DelayL;
			SubDelayR = DelayR;
	}

	// --- Delay Processing 1 ---
	float OutRight = m_Delay1LineRight.Pull(DelayR);
	float OutLeft  = m_Delay1LineLeft.Pull(DelayL);

	OutRight = m_BassFilter1.Process(OutRight, DadDSP::eChannel::Right);
	OutLeft  = m_BassFilter1.Process(OutLeft, DadDSP::eChannel::Left);

	OutRight = m_TrebleFilter1.Process(OutRight, DadDSP::eChannel::Right);
	OutLeft  = m_TrebleFilter1.Process(OutLeft, DadDSP::eChannel::Left);

	m_Delay1LineRight.Push((Right + OutRight) * m_Repeat/100);
	m_Delay1LineLeft.Push((Left + OutLeft) * m_Repeat/100);

	// --- Delay Processing 2 ---
	float Out2Right;
	float Out2Left;
	if(m_RepeatDelay2 == 0){
		Out2Right = m_Delay1LineRight.Pull(SubDelayR);
		Out2Left  = m_Delay1LineLeft.Pull(SubDelayL);
	}else{
		Out2Right = m_Delay2LineRight.Pull(SubDelayR);
		Out2Left  = m_Delay2LineLeft.Pull(SubDelayL);
	}

	Out2Right = m_BassFilter2.Process(Out2Right, DadDSP::eChannel::Right);
	Out2Left  = m_BassFilter2.Process(Out2Left, DadDSP::eChannel::Left);

	Out2Right = m_TrebleFilter2.Process(Out2Right, DadDSP::eChannel::Right);
	Out2Left  = m_TrebleFilter2.Process(Out2Left, DadDSP::eChannel::Left);

	m_Delay2LineRight.Push((Right + Out2Right) * m_RepeatDelay2/100);
	m_Delay2LineLeft.Push((Left + Out2Left) * m_RepeatDelay2/100);

	// --- Delay1 ans Delay2  Blending ---
	float mix = m_BlendD1D2 / 100.0f;
	float gain1 = cosf(mix * 0.5f * M_PI); // Crossfade gain A
	float gain2 = sinf(mix * 0.5f * M_PI); // Crossfade gain B

	OutRight = ((OutRight * gain1) + (Out2Right * gain2));
	OutLeft  = ((OutLeft * gain1) + (Out2Left * gain2));
#ifdef PENDAI
	pOut->Right = OutRight;
	pOut->Left  = OutLeft;
#elif defined(PENDAII)
	if( (m_MemMixDelay != m_Mix) ||
		(m_MemVol1Left != DadUI::cPendaUI::m_Volumes.getVol1Left())||
		(m_MemVol1Right != DadUI::cPendaUI::m_Volumes.getVol1Right())
	   ){
		m_GainWet = DadUI::cPendaUI::m_Volumes.MixDryWet(m_Mix / 100);
		m_MemMixDelay =  m_Mix;
	}

	pOut->Right = OutRight * m_GainWet;
	pOut->Left  = OutLeft * m_GainWet;
#endif
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
	pthis->m_BassFilter1.setCutoffFreq(Freq);
	pthis->m_BassFilter1.CalculateParameters();
	pthis->m_BassFilter2.setCutoffFreq(Freq);
	pthis->m_BassFilter2.CalculateParameters();
}

// --------------------------------------------------------------------------
// Treble control callback - sets low-pass filter frequency
#define MIN_TREBLE_FREQ 600
#define MAX_TREBLE_FREQ 12000
void cDelay::TrebleChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData){
	cDelay * pthis = (cDelay *)CallbackUserData;
	float Freq = pthis->getLogFrequency(pParameter->getNormalizedValue(), MIN_TREBLE_FREQ, MAX_TREBLE_FREQ);
	pthis->m_TrebleFilter1.setCutoffFreq(Freq);
	pthis->m_TrebleFilter1.CalculateParameters();
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
