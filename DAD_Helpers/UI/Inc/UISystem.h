#pragma once
//====================================================================================
// UISystem.h
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "main.h"
#include "PendaUI.h"
#include "Parameter.h"
#include "cDisplay.h"
#include "UIDefines.h"
#include "UIComponent.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultichar"
constexpr uint32_t SysSerializeID ='Sys0'; // SerializeID for System
#pragma GCC diagnostic pop

namespace DadUI {

// Constants for VU meter behavior
#define TIME_INTEGRATION 1.0f      // Time constant for meter ballistics (in seconds)
#define DELTA_CHANGE 0.001f        // Minimum change threshold for redrawing meter
#define PEAK_LEVEL 0.9f            // Level at which peak indicator is triggered
#define PEAK_TIME 10               // Duration of peak indicator (in frames)
#define VU_WIDTH 216               // Width of the VU meter in pixels
#define MIN_DB -30.0f              // Minimum dB level displayed on meter

//***********************************************************************************
// class cUIVuMeterView
// Description: Implements a VU meter visualization for audio levels
//***********************************************************************************
class cUIVuMeterView : public iGUIObject {
public:
    // ------------------------------------------------------------------------------
    // Constructor
    cUIVuMeterView() {}

    // ------------------------------------------------------------------------------
    // Destructor
    virtual ~cUIVuMeterView() {}

    // ------------------------------------------------------------------------------
    // Function: Init
    // Description: Initializes meter variables and state
    void Init();

    // ------------------------------------------------------------------------------
    // Function: Draw
    // Description: Renders the VU meter visualization
    void Draw();

    // ------------------------------------------------------------------------------
    // Function: Process
    // Description: Processes audio samples to update meter values
    // Parameters:
    //   pIn - Pointer to audio buffer containing samples
    void Process(AudioBuffer *pIn);

    // ------------------------------------------------------------------------------
    // Function: OnMainFocusLost
    // Description: Called when this view loses focus
    void OnMainFocusLost() override;

    // ------------------------------------------------------------------------------
    // Function: drawMainDownStat
    // Description: Draws the static elements of the VU meter display
    void drawMainDownStat();

    // ------------------------------------------------------------------------------
    // Function: OnMainFocusGained
    // Description: Called when this view gains focus
    void OnMainFocusGained() override;

protected:
    // ------------------------------------------------------------------------------
    // Function: SampleToDbPixel
    // Description: Converts a sample value to pixel width for display
    // Parameters:
    //   sample - Audio sample value to convert
    // Returns: Pixel width corresponding to the sample's dB level
    uint16_t SampleToDbPixel(float sample);

    // ------------------------------------------------------------------------------
    // Function: ProcessSample
    // Description: Processes a single audio sample to update meter and peak values
    // Parameters:
    //   sample - Audio sample value
    //   pMeter - Pointer to meter value to update
    //   pPeak - Pointer to peak counter to update
    void ProcessSample(float sample, float *pMeter, int32_t *pPeak);

    // ------------------------------------------------------------------------------
    // Member variables
    float       m_OldMeterLeft;        // Previous left channel meter value
    float       m_OldMeterRight;       // Previous right channel meter value
    float       m_MeterLeft;           // Current left channel meter value
    float       m_MeterRight;          // Current right channel meter value
    int32_t     m_CtPeakLeft;          // Left peak indicator counter
    int32_t     m_CtPeakRight;         // Right peak indicator counter
    uint16_t    m_MeterWidthLeft;      // Left meter width in pixels
    uint16_t    m_MeterWidthRight;     // Right meter width in pixels
    int16_t     m_MemPeakWidthLeft;    // Left peak hold width in pixels
    int16_t     m_MemPeakWidthRight;   // Right peak hold width in pixels
    float       m_CtIntegration;       // Integration constant for meter ballistics
};

//***********************************************************************************
// class cUIImputVolume
// Description: Implements the input volume control UI with VU meters
//***********************************************************************************
class cUIImputVolume : public cUIParameters {
public:
    // ------------------------------------------------------------------------------
    // Constructor
    cUIImputVolume() {}

    // ------------------------------------------------------------------------------
    // Function: Init
    // Description: Initializes the UI components and parameters
    void Init();

    // ------------------------------------------------------------------------------
    // Function: Activate
    // Description: Activates the UI and requests focus for the VU meter
    void Activate() override;

    // ------------------------------------------------------------------------------
    // Function: DeActivate
    // Description: Deactivates the UI and releases focus
    void DeActivate() override;

    // ------------------------------------------------------------------------------
    // Function: Update
    // Description: Updates the UI state and handles VU meter drawing
    void Update() override;

    // ------------------------------------------------------------------------------
    // Function: Process
    // Description: Processes audio samples for the VU meter
    // Parameters:
    //   pIn - Pointer to audio buffer
    inline void Process(AudioBuffer *pIn) {
        m_UIVuMeterView.Process(pIn);
    }

    // ------------------------------------------------------------------------------
    // Static Function: VolumePanChange
    // Description: Callback for volume/pan parameter changes
    // Parameters:
    //   pParameter - The changed parameter
    //   CallbackUserData - Pointer to this instance
    static void VolumePanChange(DadUI::cParameter *pParameter, uint32_t CallbackUserData);

protected:
    // ------------------------------------------------------------------------------
    // Member variables
    DadUI::cParameter           m_InputVolume;		// Volume parameter
    DadUI::cParameter           m_InputPanning;     // Pan parameter

    cParameterNumNormalView     m_InputVolumeView;  // Volume parameter view
    cParameterNumLeftRightView  m_InputPanningView; // Pan parameter view

    float     					m_MemInputVolume;  	// Volume parameter view
    float						m_MemInputPanning; 	// Pan parameter view

    cUIVuMeterView              m_UIVuMeterView;    // VU meter visualization
};

} // namespace DadUI
