//====================================================================================
// UISystem.cpp
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "UISystem.h"

namespace DadUI {

//***********************************************************************************
// class cUIVuMeterView
// Description: Implements a VU meter visualization for audio levels
//***********************************************************************************

// ------------------------------------------------------------------------------
// Function: Init
// Description: Initializes meter variables and state
void cUIVuMeterView::Init() {
	m_OldMeterLeft = 0;
	m_OldMeterRight = 0;
	m_MeterLeft = 0;
	m_MeterRight = 0;
	m_CtIntegration = 1 / (TIME_INTEGRATION * SAMPLING_RATE);
	m_CtPeakLeft = 0;
	m_CtPeakRight = 0;
	m_MemPeakWidthLeft = 0;
	m_MemPeakWidthRight = 0;
	m_MeterWidthLeft = 0;
	m_MeterWidthRight = 0;
}

// ------------------------------------------------------------------------------
// Function: Draw
// Description: Renders the VU meter visualization
void cUIVuMeterView::Draw() {
	// Left channel drawing
	if(m_MemPeakWidthLeft != 0) {
		cPendaUI::m_pDynMainDownLayer->drawFillRect(m_MemPeakWidthLeft+65, 18, 3, 22, DadGFX::sColor(45, 64, 59));
	}

	float Delta = m_MeterLeft - m_OldMeterLeft;
	m_OldMeterLeft = m_MeterLeft;
	if((Delta > DELTA_CHANGE)||(Delta < -DELTA_CHANGE)) {
		// Clear previous meter
		cPendaUI::m_pDynMainDownLayer->drawFillRect(67, 18, VU_WIDTH, 22, DadGFX::sColor(45, 64, 59));

		// Draw new meter level
		m_MeterWidthLeft = SampleToDbPixel(m_MeterLeft);
		cPendaUI::m_pDynMainDownLayer->drawFillRect(67, 18, m_MeterWidthLeft, 22, DadGFX::sColor(60, 153, 131));
	}

	// Handle peak indicator for left channel
	if(m_MeterWidthLeft > m_MemPeakWidthLeft) {
		m_MemPeakWidthLeft = m_MeterWidthLeft;
	} else {
		// Smooth peak indicator decay
		int16_t Delta = (m_MemPeakWidthLeft-m_MeterWidthLeft) / 6;
		if(Delta < 4) Delta = 4;
		m_MemPeakWidthLeft -= Delta;
		if(m_MemPeakWidthLeft < m_MeterWidthLeft) {
			m_MemPeakWidthLeft = m_MeterWidthLeft;
		}
	}

	// Draw peak indicator
	if(m_MemPeakWidthLeft != 0) {
		cPendaUI::m_pDynMainDownLayer->drawFillRect(m_MemPeakWidthLeft+65, 18, 3, 22, DadGFX::sColor(180, 160, 160));
		cPendaUI::m_pDynMainDownLayer->drawFillRect(m_MemPeakWidthLeft+65, 18, 3, 22, DadGFX::sColor(180, 180, 180));
	}

	// Handle peak hold indicator
	if(m_CtPeakLeft > 0) {
		m_CtPeakLeft--;
		if(m_CtPeakLeft != 0) {
			cPendaUI::m_pDynMainDownLayer->drawFillRect(216+67-15, 18, 15, 22, DadGFX::sColor(200, 100, 100));
		} else {
			cPendaUI::m_pDynMainDownLayer->drawFillRect(216+67-15, 18, 15, 22, DadGFX::sColor(45, 64, 59));
		}
	}

	// Right channel drawing (same logic as left channel)
	if(m_MemPeakWidthRight != 0) {
		cPendaUI::m_pDynMainDownLayer->drawFillRect(m_MemPeakWidthRight+65, 53, 3, 22, DadGFX::sColor(45, 64, 59));
	}

	Delta = m_MeterRight - m_OldMeterRight;
	m_OldMeterRight = m_MeterRight;
	if((Delta > DELTA_CHANGE)||(Delta < -DELTA_CHANGE)) {
		cPendaUI::m_pDynMainDownLayer->drawFillRect(67, 53, VU_WIDTH, 22, DadGFX::sColor(45, 64, 59));

		m_MeterWidthRight = SampleToDbPixel(m_MeterRight);
		cPendaUI::m_pDynMainDownLayer->drawFillRect(67, 53, m_MeterWidthRight, 22, DadGFX::sColor(60, 153, 131));
	}

	if(m_MeterWidthRight > m_MemPeakWidthRight) {
		m_MemPeakWidthRight = m_MeterWidthRight;
	} else {
		int16_t Delta = (m_MemPeakWidthRight-m_MeterWidthRight) / 6;
		if(Delta < 4) Delta = 4;
		m_MemPeakWidthRight -= Delta;
		if(m_MemPeakWidthRight < m_MeterWidthRight) {
			m_MemPeakWidthRight = m_MeterWidthRight;
		}
	}
	if(m_MemPeakWidthRight != 0) {
		cPendaUI::m_pDynMainDownLayer->drawFillRect(m_MemPeakWidthRight+65, 53, 3, 22, DadGFX::sColor(180, 160, 160));
		cPendaUI::m_pDynMainDownLayer->drawFillRect(m_MemPeakWidthRight+65, 53, 3, 22, DadGFX::sColor(180, 180, 180));
	}

	if(m_CtPeakRight > 0) {
		m_CtPeakRight--;
		if(m_CtPeakRight != 0) {
			cPendaUI::m_pDynMainDownLayer->drawFillRect(216+67-15, 53, 15, 22, DadGFX::sColor(200, 100, 100));
		} else {
			cPendaUI::m_pDynMainDownLayer->drawFillRect(216+67-15, 53, 15, 22, DadGFX::sColor(45, 64, 59));
		}
	}
};

// ------------------------------------------------------------------------------
// Function: Process
// Description: Processes audio samples to update meter values
// Parameters:
//   pIn - Pointer to audio buffer containing samples
void cUIVuMeterView::Process(AudioBuffer *pIn) {
	ProcessSample(pIn->Left, &m_MeterLeft, &m_CtPeakLeft);
	ProcessSample(pIn->Right, &m_MeterRight, &m_CtPeakRight);
}

// ------------------------------------------------------------------------------
// Function: OnMainFocusLost
// Description: Called when this view loses focus
void cUIVuMeterView::OnMainFocusLost(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(0);
	cPendaUI::m_pDynMainDownLayer->changeZOrder(0);
};

// ------------------------------------------------------------------------------
// Function: drawMainDownStat
// Description: Draws the static elements of the VU meter display
void cUIVuMeterView::drawMainDownStat() {
	cPendaUI::m_pStatMainDownLayer->eraseLayer(MENU_BACK_COLOR);
	cPendaUI::m_pStatMainDownLayer->setFont(cPendaUI::m_pFont_M);
	cPendaUI::m_pStatMainDownLayer->setCursor(10, 20);
	cPendaUI::m_pStatMainDownLayer->drawText("Left");
	cPendaUI::m_pStatMainDownLayer->setCursor(10, 55);
	cPendaUI::m_pStatMainDownLayer->drawText("Right");
	cPendaUI::m_pStatMainDownLayer->drawRect(65, 16, 220, 26, 2, DadGFX::sColor(200,200,200));
	cPendaUI::m_pStatMainDownLayer->drawFillRect(67, 18, 216, 22, DadGFX::sColor(45, 64, 59));
	cPendaUI::m_pStatMainDownLayer->drawRect(65, 51, 220, 26, 2, DadGFX::sColor(200,200,200));
	cPendaUI::m_pStatMainDownLayer->drawFillRect(67, 53, 216, 22, DadGFX::sColor(45, 64, 59));
}

// ------------------------------------------------------------------------------
// Function: OnMainFocusGained
// Description: Called when this view gains focus
void cUIVuMeterView::OnMainFocusGained(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(40);
	cPendaUI::m_pDynMainDownLayer->changeZOrder(41);
	drawMainDownStat();
	cPendaUI::m_pDynMainDownLayer->eraseLayer();
	Draw();
};

// ------------------------------------------------------------------------------
// Function: SampleToDbPixel
// Description: Converts a sample value to pixel width for display
// Parameters:
//   sample - Audio sample value to convert
// Returns: Pixel width corresponding to the sample's dB level
uint16_t cUIVuMeterView::SampleToDbPixel(float sample) {
	float db = 20.0f * log10f(fabs(sample) + 1e-6f);  // Convert to dB with small offset to avoid log(0)
	if (db < MIN_DB) db = MIN_DB;  // Clamp to minimum dB
	if (db > 0) db = 0;            // Clamp to 0dB
	return static_cast<uint16_t>((((db - MIN_DB) / -MIN_DB) * VU_WIDTH) + 0.5f);  // Scale to pixel width
}

// ------------------------------------------------------------------------------
// Function: ProcessSample
// Description: Processes a single audio sample to update meter and peak values
// Parameters:
//   sample - Audio sample value
//   pMeter - Pointer to meter value to update
//   pPeak - Pointer to peak counter to update
void cUIVuMeterView::ProcessSample(float sample, float *pMeter, int32_t *pPeak) {
	if (sample < 0) sample = -sample;  // Use absolute value

	// Trigger peak indicator if level exceeds threshold
	if(sample > PEAK_LEVEL) {
		*pPeak = PEAK_TIME;
	}

	// Update meter with ballistics
	if (sample > *pMeter) {
		*pMeter = sample;  // Fast attack
	} else {
		*pMeter -= m_CtIntegration;  // Slow decay
		if(*pMeter < 0) {
			*pMeter = 0;  // Clamp to zero
		}
	}
}

//***********************************************************************************
// class cUIImputVolume
// Description: Implements the input volume control UI with VU meters
//***********************************************************************************

// ------------------------------------------------------------------------------
// Function: Init
// Description: Initializes the UI components and parameters
void cUIImputVolume::Init(){
	/* PENDA II
	// Initialize parameters with ranges and callbacks
	m_InputVolume.Init(50.0f, 0.0f, 100.0f, 10.0f, 1.0f, VolumePanChange, (uint32_t) this);
	m_InputPanning.Init(0.0f, -100.0f, +100.0f, 5.0f, 1.0f, VolumePanChange, (uint32_t) this);

	// Initialize parameter views
	m_InputVolumeView.Init(&m_InputVolume, "Input Vol.", "Input Volume", "%", "%");
	m_InputPanningView.Init(&m_InputPanning, "Pan", "Input Panning", "%", "%");
    */
	// Initialize VU meter
	m_UIVuMeterView.Init();

	// Initialize base class with parameter views
	cUIParameters::Init(nullptr, nullptr, nullptr);
	//cUIParameters::Init(&m_InputVolumeView, nullptr, &m_InputPanningView);
}

// ------------------------------------------------------------------------------
// Function: Activate
// Description: Activates the UI and requests focus for the VU meter
void cUIImputVolume::Activate(){
	cPendaUI::RequestFocus(&m_UIVuMeterView);
	cUIParameters::Activate();
}

// ------------------------------------------------------------------------------
// Function: DeActivate
// Description: Deactivates the UI and releases focus
void cUIImputVolume::DeActivate(){
	cUIParameters::DeActivate();
	if(cPendaUI::HasFocus(&m_UIVuMeterView)) {
		cPendaUI::ReleaseFocus();
	}
}

// ------------------------------------------------------------------------------
// Function: Update
// Description: Updates the UI state and handles VU meter drawing
void cUIImputVolume::Update(){
	cUIParameters::Update();
	if(cPendaUI::HasFocus(&m_UIVuMeterView)) {
		m_UIVuMeterView.Draw();
	}
};
/* PENDA II
// ------------------------------------------------------------------------------
// Static Function: VolumePanChange
// Description: Callback for volume/pan parameter changes
// Parameters:
//   pParameter - The changed parameter
//   CallbackUserData - Pointer to this instance
void cUIImputVolume::VolumePanChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData) {
	cUIImputVolume* pThis = (cUIImputVolume*)CallbackUserData;
	float Pan = pThis->m_InputPanning.getNormalizedValue();
	float Vol = pThis->m_InputVolume.getNormalizedValue();

	// Calculate left/right gain based on pan position
	float Left = (Pan <= 0.5f) ? 1.0f : 1.0f - 2.0f * (Pan - 0.5f);
	float Right = (Pan >= 0.5f) ? 1.0f : 2.0f * Pan;

	// Apply volume
	Left *= Vol;
	Right *= Vol;

	// Update hardware volume control
	cPendaUI::m_Volumes.Volume1Change(static_cast<uint8_t>(Left * 255.0f + 0.5f),
							static_cast<uint8_t>(Right * 255.0f + 0.5f));
}
*/
} // namespace DadUI
