//====================================================================================
// PendaUI.cpp
//
// Main management of PENDA User Interface
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "PendaUI.h"
//=======================================================================================
// Declare graphical layers for the interface

// Back Layer - Background layer for the interface
DECLARE_LAYER(Back, SCREEN_WIDTH, SCREEN_HEIGHT)

// Front Layer - Dynamic and static front layers for the interface
DECLARE_LAYER(DynFront, SCREEN_WIDTH, SCREEN_HEIGHT)
DECLARE_LAYER(StatFront, SCREEN_WIDTH, SCREEN_HEIGHT)

// Menu Layers - Dynamic and static layers for the menu
DECLARE_LAYER(DynMenu, SCREEN_WIDTH, MENU_HEIGHT)
DECLARE_LAYER(StatMenu, SCREEN_WIDTH, MENU_HEIGHT)

// Param View Layers - Dynamic and static layers for parameter views
DECLARE_LAYER(DynParam1, PARAM_WIDTH, PARAM_HEIGHT)
DECLARE_LAYER(StatParam1, PARAM_WIDTH, PARAM_HEIGHT)

DECLARE_LAYER(DynParam2, PARAM_WIDTH, PARAM_HEIGHT)
DECLARE_LAYER(StatParam2, PARAM_WIDTH, PARAM_HEIGHT)

DECLARE_LAYER(DynParam3, PARAM_WIDTH, PARAM_HEIGHT)
DECLARE_LAYER(StatParam3, PARAM_WIDTH, PARAM_HEIGHT)

// Main Down Layers - Dynamic and static layers for the main lower section
DECLARE_LAYER(DynMainDown, MAIN_WIDTH, MAIN_HEIGHT)
DECLARE_LAYER(StatMainDown, MAIN_WIDTH, MAIN_HEIGHT)

// Main Up Layers - Dynamic and static layers for the main upper section
DECLARE_LAYER(DynMainUp, MAIN_WIDTH, PARAM_HEIGHT)
DECLARE_LAYER(StatMainUp, MAIN_WIDTH, PARAM_HEIGHT)

// Global UI manager
extern DadUI::cUIObjectManager __UIObjManager;

namespace DadUI{

//***********************************************************************************
// class cPendaUI
//***********************************************************************************

//=======================================================================================
// Instantiate static data
DadGFX::cLayer*	     cPendaUI::m_pBackLayer;  // Background layer
DadGFX::cImageLayer* cPendaUI::m_pPendaLayer;  // Image PENDA Layer

DadGFX::cLayer*	cPendaUI::m_pDynFrontLayer;  // Dynamic front layer
DadGFX::cLayer*	cPendaUI::m_pStatFrontLayer;  // Static front layer

DadGFX::cLayer*	cPendaUI::m_pDynMenuLayer;  // Dynamic menu layer
DadGFX::cLayer*	cPendaUI::m_pStatMenuLayer;  // Static menu layer

DadGFX::cLayer*	cPendaUI::m_pDynParam1Layer;  // Dynamic parameter 1 layer
DadGFX::cLayer*	cPendaUI::m_pStatParam1Layer;  // Static parameter 1 layer
DadGFX::cLayer*	cPendaUI::m_pDynParam2Layer;  // Dynamic parameter 2 layer
DadGFX::cLayer*	cPendaUI::m_pStatParam2Layer;  // Static parameter 2 layer
DadGFX::cLayer*	cPendaUI::m_pDynParam3Layer;  // Dynamic parameter 3 layer
DadGFX::cLayer*	cPendaUI::m_pStatParam3Layer;  // Static parameter 3 layer

DadGFX::cLayer*	cPendaUI::m_pDynMainUpLayer;  // Dynamic main upper layer
DadGFX::cLayer*	cPendaUI::m_pStatMainUpLayer;  // Static main upper layer

DadGFX::cLayer*	cPendaUI::m_pDynMainDownLayer;  // Dynamic main lower layer
DadGFX::cLayer*	cPendaUI::m_pStatMainDownLayer;  // Static main lower layer

// Fonts
DadGFX::cFont*	cPendaUI::m_pFont_S;  			// Small font
DadGFX::cFont*	cPendaUI::m_pFont_M;			// Medium font
DadGFX::cFont*	cPendaUI::m_pFont_L;			// Large font
DadGFX::cFont*	cPendaUI::m_pFont_XL;			// XLarge font
DadGFX::cFont*	cPendaUI::m_pFont_LB;			// Large font bold
DadGFX::cFont*	cPendaUI::m_pFont_XLB;			// Extra large font bold
DadGFX::cFont*	cPendaUI::m_pFont_XXL;			// Extra extra large font Bold

// Hardware UI components
cEncoder		cPendaUI::m_Encoder0;			// Encoder 0
cEncoder		cPendaUI::m_Encoder1;			// Encoder 1
cEncoder		cPendaUI::m_Encoder2;			// Encoder 2
cEncoder		cPendaUI::m_Encoder3;			// Encoder 3
int32_t			cPendaUI::m_Encoder0Increment;  // Increment value for encoder 0
int32_t			cPendaUI::m_Encoder1Increment;  // Increment value for encoder 1
int32_t			cPendaUI::m_Encoder2Increment;  // Increment value for encoder 2
int32_t			cPendaUI::m_Encoder3Increment;  // Increment value for encoder 3

cSwitch			cPendaUI::m_FootSwitch1;  		// Foot switch 1
cSwitch			cPendaUI::m_FootSwitch2;  		// Foot switch 2

cMidi			cPendaUI::m_Midi;   			// MIDI manager

iGUIObject*	 	cPendaUI::m_pActiveObject;  	// Currently active GUI object

std::stack<iGUIObject*> cPendaUI::m_MainFocusStack;// Stack of Main focus


// Slot memory manager
DadQSPI::cMemory cPendaUI::m_Memory;

bool 			cPendaUI::m_AudioState;


// --------------------------------------------------------------------------
// Initialize the user interface
void cPendaUI::Init(const char* pSplashTxt1, const char* pSplashTxt2, const char* pSplashTxt3, UART_HandleTypeDef *phuart){

	m_Memory.Init();

	constexpr uint32_t EncoderUpdatePeriodMs = UIRT_RATE * 0.001f;  // 1  ms Encoder update period in milliseconds
	constexpr uint32_t SwitchUpdatePeriodMs = UIRT_RATE * 0.005f;   // 5  ms  Switch update period in milliseconds

	// Initialize each encoder with its respective pins.
	m_Encoder0.Init(Encoder0_A_GPIO_Port, Encoder0_A_Pin,
					Encoder0_B_GPIO_Port, Encoder0_B_Pin,
					Encoder0_SW_GPIO_Port, Encoder0_SW_Pin,
					EncoderUpdatePeriodMs, SwitchUpdatePeriodMs);
	m_Encoder3.Init(Encoder1_A_GPIO_Port, Encoder1_A_Pin,
					Encoder1_B_GPIO_Port, Encoder1_B_Pin,
					Encoder1_SW_GPIO_Port, Encoder1_SW_Pin,
					EncoderUpdatePeriodMs, SwitchUpdatePeriodMs);
	m_Encoder2.Init(Encoder2_A_GPIO_Port, Encoder2_A_Pin,
					Encoder2_B_GPIO_Port, Encoder2_B_Pin,
					Encoder2_SW_GPIO_Port, Encoder2_SW_Pin,
					EncoderUpdatePeriodMs, SwitchUpdatePeriodMs);
	m_Encoder1.Init(Encoder3_A_GPIO_Port, Encoder3_A_Pin,
					Encoder3_B_GPIO_Port, Encoder3_B_Pin,
					Encoder3_SW_GPIO_Port, Encoder3_SW_Pin,
					EncoderUpdatePeriodMs, SwitchUpdatePeriodMs);

	m_FootSwitch1.Init(FootSwitch1_GPIO_Port, FootSwitch1_Pin);  // Initialize foot switch 1
	m_FootSwitch2.Init(FootSwitch2_GPIO_Port, FootSwitch2_Pin);  // Initialize foot switch 2

	// Initialize fonts of different sizes
	m_pFont_S	= new DadGFX::cFont(FONTS);
	m_pFont_M	= new DadGFX::cFont(FONTM);
	m_pFont_L	= new DadGFX::cFont(FONTL);
	m_pFont_XL	= new DadGFX::cFont(FONTXL);

	m_pFont_LB	= new DadGFX::cFont(FONTLB);
	m_pFont_XLB	= new DadGFX::cFont(FONTXLB);
	m_pFont_XXL	= new DadGFX::cFont(FONTXXL);


	// Splash screen
	m_pBackLayer = ADD_LAYER(Back, 0, 0, 100);
	m_pBackLayer->eraseLayer(SPLASHSCREEN_BACK_COLOR);
    m_pPendaLayer = __Display.addLayer(PENDA, 19, 10, 80, 80, 101);

    const uint16_t TextCentre = 320/2;
    m_pBackLayer->setTextFrontColor(SPLASHSCREEN_TEXT_COLOR);

    m_pBackLayer->setFont(m_pFont_XLB);
    m_pBackLayer->setCursor(96, 5);
    m_pBackLayer->drawText("PENDA");

    m_pBackLayer->setFont(m_pFont_L);
    m_pBackLayer->setCursor(101, 65);
    m_pBackLayer->drawText("By DAD Design");

    m_pBackLayer->setFont(m_pFont_XL);
    uint32_t TextWidth = m_pBackLayer->getTextWidth(pSplashTxt1);
    m_pBackLayer->setCursor(TextCentre - (TextWidth/2), 100);
    m_pBackLayer->drawText(pSplashTxt1);

    m_pBackLayer->setFont(m_pFont_L);
    TextWidth = m_pBackLayer->getTextWidth(pSplashTxt2);
    m_pBackLayer->setCursor(TextCentre - (TextWidth/2), 150);
    m_pBackLayer->drawText(pSplashTxt2);

    m_pBackLayer->setFont(m_pFont_L);
    TextWidth = m_pBackLayer->getTextWidth(pSplashTxt3);
    m_pBackLayer->setCursor(TextCentre - (TextWidth/2), 180);
    m_pBackLayer->drawText(pSplashTxt3);


    __Display.flush();

    // Back Layer
    HAL_Delay(2000);
    m_pPendaLayer->changeZOrder(0);
    m_pBackLayer->changeZOrder(1);
    m_pBackLayer->eraseLayer(BACKCOLOR);

	// Front Layers
	m_pDynFrontLayer  = ADD_LAYER(DynFront, 0, 0, 0);
	m_pDynFrontLayer->setFont(m_pFont_M);
	m_pDynFrontLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pStatFrontLayer = ADD_LAYER(StatFront, 0, 0, 0);
	m_pStatFrontLayer->setFont(m_pFont_M);
	m_pStatFrontLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);

	// Menu Layers
	m_pDynMenuLayer =  ADD_LAYER(DynMenu, 0, 0, 0);
	m_pDynMenuLayer->setFont(m_pFont_M);
	m_pDynMenuLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pStatMenuLayer = ADD_LAYER(StatMenu, 0, 0, 0);
	m_pStatMenuLayer->setFont(m_pFont_M);
	m_pStatMenuLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);

	// Param View Layers
	m_pDynParam1Layer =  ADD_LAYER(DynParam1, 0, MENU_HEIGHT, 0);
	m_pDynParam1Layer->setFont(m_pFont_M);
	m_pDynParam1Layer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pStatParam1Layer = ADD_LAYER(StatParam1, 0, MENU_HEIGHT, 0);
	m_pStatParam1Layer->setFont(m_pFont_M);
	m_pStatParam1Layer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pDynParam2Layer =  ADD_LAYER(DynParam2, PARAM_WIDTH, MENU_HEIGHT, 0);
	m_pDynParam2Layer->setFont(m_pFont_M);
	m_pDynParam2Layer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pStatParam2Layer = ADD_LAYER(StatParam2, PARAM_WIDTH, MENU_HEIGHT, 0);
	m_pStatParam2Layer->setFont(m_pFont_M);
	m_pStatParam2Layer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pDynParam3Layer =  ADD_LAYER(DynParam3, PARAM_WIDTH+PARAM_WIDTH, MENU_HEIGHT, 0);
	m_pDynParam3Layer->setFont(m_pFont_M);
	m_pDynParam3Layer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pStatParam3Layer = ADD_LAYER(StatParam3, PARAM_WIDTH+PARAM_WIDTH, MENU_HEIGHT, 0);
	m_pStatParam3Layer->setFont(m_pFont_M);
	m_pStatParam3Layer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);

	// Main Layers
	m_pDynMainUpLayer = ADD_LAYER(DynMainUp, 0, MENU_HEIGHT, 0);
	m_pDynMainUpLayer->setFont(m_pFont_M);
	m_pDynMainUpLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pStatMainUpLayer = ADD_LAYER(StatMainUp, 0, MENU_HEIGHT, 0);
	m_pStatMainUpLayer->setFont(m_pFont_M);
	m_pStatMainUpLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);

	m_pDynMainDownLayer = ADD_LAYER(DynMainDown, 0, MENU_HEIGHT+PARAM_HEIGHT, 0);
	m_pDynMainDownLayer->setFont(m_pFont_XL);
	m_pDynMainDownLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
	m_pStatMainDownLayer = ADD_LAYER(StatMainDown, 0, MENU_HEIGHT+PARAM_HEIGHT, 0);
	m_pStatMainDownLayer->setFont(m_pFont_L);
	m_pStatMainDownLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);

	m_pActiveObject= nullptr;

	m_Midi.Initialize(phuart);
	m_AudioState = false;
}

// --------------------------------------------------------------------------
// Set the active GUI object
iGUIObject* cPendaUI::setActiveObject(iGUIObject* pActiveObject){
	iGUIObject* pOldActiveOject = m_pActiveObject;  // Store the old active object
	if(nullptr != pOldActiveOject){
		pOldActiveOject->DeActivate();  // Deactivate the old object
	}
		m_pActiveObject = pActiveObject;  // Set the new active object
		pActiveObject->Activate();  // Activate the new object

	return pOldActiveOject;  // Return the old active object
}

// --------------------------------------------------------------------------
// Proceed with the processing of all GUI objects
void cPendaUI::Update(){
	for(iGUIObject *pObject : __UIObjManager.m_TabGUIObject){
		pObject->Update();  // Call the real-time process method for each object
	}
}

// --------------------------------------------------------------------------
// Real-time processing of encoders, switches and GUI objects
bool cPendaUI::RTProcess() {
	m_FootSwitch1.Debounce();  // Debounce foot switch 1
	m_FootSwitch2.Debounce();  // Debounce foot switch 2

	m_Encoder0.Debounce();  // Debounce encoder 0
	m_Encoder0Increment += m_Encoder0.getIncrement();  // Update encoder 0 increment

	m_Encoder1.Debounce();  // Debounce encoder 1
	m_Encoder1Increment += m_Encoder1.getIncrement();  // Update encoder 1 increment

	m_Encoder2.Debounce();  // Debounce encoder 2
	m_Encoder2Increment += m_Encoder2.getIncrement();  // Update encoder 2 increment

	m_Encoder3.Debounce();  // Debounce encoder 3
	m_Encoder3Increment += m_Encoder3.getIncrement();  // Update encoder 3 increment

	// Process all GUI objects in real-time
	for(iGUIObject *pObject : __UIObjManager.m_TabGUIObject){
		pObject->RTProcess();
	}

	return m_AudioState;
}

// --------------------------------------------------------------------------
// Force to draw the dynamic view
void cPendaUI::ReDraw(){
	for(iGUIObject *pObject : __UIObjManager.m_TabGUIObject){
		pObject->ReDraw();  // Call the ReDraw method for each object
	}
}

// --------------------------------------------------------------------------
// Save UI state
void cPendaUI::Save(DadQSPI::cSerialize &Serializer){
	for(iGUIObject *pObject : __UIObjManager.m_TabGUIObject){
		pObject->Save(Serializer);
	}
}

// --------------------------------------------------------------------------
// Restore UI state
void cPendaUI::Restore(DadQSPI::cSerialize &Serializer){
	for(iGUIObject *pObject : __UIObjManager.m_TabGUIObject){
		pObject->Restore(Serializer);
	}
}

// --------------------------------------------------------------------------
// Request focus for an object
void cPendaUI::RequestFocus(iGUIObject* pGUIObject) {

    // Check if the object already has focus
    if ((0 == m_MainFocusStack.empty()) && (m_MainFocusStack.top() == pGUIObject)){
    	return;
    }

    // Notify the current focused object that it lost focus
    if (0 == m_MainFocusStack.empty()) {
    	m_MainFocusStack.top()->OnMainFocusLost();
    }

    // Push the new object onto the stack
    m_MainFocusStack.push(pGUIObject);
    pGUIObject->OnMainFocusGained();
}

// --------------------------------------------------------------------------
// Release focus from an object
void cPendaUI::ReleaseFocus() {

	if(!m_MainFocusStack.empty()){
		// Release focus and remove the current object from the stack
		m_MainFocusStack.top()->OnMainFocusLost();
		m_MainFocusStack.pop();
	    if(!m_MainFocusStack.empty()){
	    	// Restore focus to the previous object if available
	    	m_MainFocusStack.top()->OnMainFocusGained();
	    }else{
			m_pDynMainDownLayer->eraseLayer();
			m_pStatMainDownLayer->eraseLayer(BACKCOLOR);
		}
	}
}

// --------------------------------------------------------------------------
// Check if an object currently has focus
uint8_t cPendaUI::HasFocus(iGUIObject* pGUIObject){
    if((0 == m_MainFocusStack.empty()) && (m_MainFocusStack.top() == pGUIObject)){
    	return 1;
    }else{
    	return 0;
    }
}

//***********************************************************************************
// class iGUIObject
//***********************************************************************************
iGUIObject::iGUIObject(){
	__UIObjManager.m_TabGUIObject.push_back(this);  // Add the object to the list
}

} // DadUI
