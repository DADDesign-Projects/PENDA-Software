#pragma once
//====================================================================================
// PendaUI.h
//
// Main management of PENDA User Interface
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "main.h"
#include "cDisplay.h"
#include "cMemory.h"
#include "Serialize.h"
#include "cEncoder.h"
#include "cSwitch.h"
#include "UIDefines.h"
#include <vector>
#include <stack>

// --------------------------------------------------------------------------
// Global references
extern DadGFX::cDisplay	__Display;  // External reference to the display object
constexpr float UIRT_RATE = SAMPLING_RATE / (float) AUDIO_BUFFER_SIZE;
namespace DadUI{
class iGUIObject;

//***********************************************************************************
// class cUIObject
//***********************************************************************************
class cUIObjectManager{
public:
	cUIObjectManager(){
		m_TabGUIObject.clear();
	}

	// Array of cGUIObject for serialization and real-time process
    std::vector<iGUIObject*> m_TabGUIObject;  // List of GUI objects
};

//***********************************************************************************
// class cPendaUI
//***********************************************************************************
class cPendaUI {
public:

    // --------------------------------------------------------------------------
    // Initialize the user interface
	static void Init(const char* pSplashTxt1, const char* pSplashTxt2, const char* pSplashTxt3);

	// --------------------------------------------------------------------------
	// Set the active GUI object
	static iGUIObject* setActiveObject(iGUIObject* pActiveObject);

    // --------------------------------------------------------------------------
	// Update all GUI objects
	static void Update();

    // --------------------------------------------------------------------------
	// Real-time processing
	static bool RTProcess();

	// --------------------------------------------------------------------------
	// Force to draw the dynamic view
	static void ReDraw();

	// --------------------------------------------------------------------------
	// Save UI state
	static void Save(DadQSPI::cSerialize &Serializer);

	// --------------------------------------------------------------------------
	// Restore UI state
	static void Restore(DadQSPI::cSerialize &Serializer);

    // --------------------------------------------------------------------------
	// Request focus for an object
    static void RequestFocus(iGUIObject* pGUIObject);

    // --------------------------------------------------------------------------
    // Release focus from an object
    static void ReleaseFocus();

    // --------------------------------------------------------------------------
    // Check if an object currently has focus
    static uint8_t HasFocus(iGUIObject* pGUIObject);

	//--------------------------------------------------------------
	// Data members

	// Display Layers
	static DadGFX::cLayer	*m_pBackLayer;  		// Background layer
	static DadGFX::cImageLayer	*m_pPendaLayer;

	static DadGFX::cLayer	*m_pDynFrontLayer;  	// Dynamic front layer
	static DadGFX::cLayer	*m_pStatFrontLayer;  	// Static front layer

	static DadGFX::cLayer	*m_pDynMenuLayer;  		// Dynamic menu layer
	static DadGFX::cLayer	*m_pStatMenuLayer;  	// Static menu layer

	static DadGFX::cLayer	*m_pDynParam1Layer;  	// Dynamic parameter 1 layer
	static DadGFX::cLayer	*m_pStatParam1Layer;  	// Static parameter 1 layer
	static DadGFX::cLayer	*m_pDynParam2Layer;  	// Dynamic parameter 2 layer
	static DadGFX::cLayer	*m_pStatParam2Layer;  	// Static parameter 2 layer
	static DadGFX::cLayer	*m_pDynParam3Layer;  	// Dynamic parameter 3 layer
	static DadGFX::cLayer	*m_pStatParam3Layer;  	// Static parameter 3 layer

	static DadGFX::cLayer	*m_pDynMainUpLayer;    	// Dynamic main upper layer
	static DadGFX::cLayer	*m_pStatMainUpLayer;   	// Static main upper layer

	static DadGFX::cLayer	*m_pDynMainDownLayer;  	// Dynamic main lower layer
	static DadGFX::cLayer	*m_pStatMainDownLayer; 	// Static main lower layer

    // Fonts
	static DadGFX::cFont*	m_pFont_S;  			// Small font
	static DadGFX::cFont*	m_pFont_M;  			// Medium font
    static DadGFX::cFont*	m_pFont_L;  			// Large font
    static DadGFX::cFont*	m_pFont_XL;  			// Extra large font
    static DadGFX::cFont*	m_pFont_LB;  			// large font bold
    static DadGFX::cFont*	m_pFont_XLB;  			// Extra large font bold
    static DadGFX::cFont*	m_pFont_XXL;  			// Extra extra large font bold

	// Hardware UI components
    static cEncoder			m_Encoder0;  			// Encoder 0
    static cEncoder			m_Encoder1;  			// Encoder 1
    static cEncoder			m_Encoder2;  			// Encoder 2
    static cEncoder			m_Encoder3;  			// Encoder 3
    static int32_t			m_Encoder0Increment;  	// Increment value for encoder 0
    static int32_t			m_Encoder1Increment;  	// Increment value for encoder 1
    static int32_t			m_Encoder2Increment;  	// Increment value for encoder 2
    static int32_t			m_Encoder3Increment;  	// Increment value for encoder 3

    static cSwitch			m_FootSwitch1;  		// Foot switch 1
    static cSwitch			m_FootSwitch2;  		// Foot switch 2

    static DadQSPI::cMemory	m_Memory;    			// Slot memory manager

    static bool				m_AudioState;			// Audio State On/Off

protected:

	static iGUIObject*	 	m_pActiveObject;  		// Currently active GUI object


    static std::stack<iGUIObject*> m_MainFocusStack; // Stack of Main focus
};

//***********************************************************************************
// class iGUIObject
//***********************************************************************************
class iGUIObject : public DadQSPI::cSerializedObject {

public:
	iGUIObject();

	virtual ~iGUIObject(){}
	virtual void Activate(){};  		// Activate the object
	virtual void DeActivate(){};  		// Deactivate the object

	virtual void Update(){};  			// Update the object
	virtual void RTProcess(){};  		// Real-time processing for the object
	virtual void ReDraw(){};			// Force to draw the dynamic view

    virtual void OnMainFocusLost(){};
    virtual void OnMainFocusGained(){};

    void Save(DadQSPI::cSerialize &Serializer) override {};    // Serialize the object
    void Restore(DadQSPI::cSerialize &Serializer) override {}; // Deserialize the object

};



} // DadUI
