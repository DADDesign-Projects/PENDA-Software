//====================================================================================
// UIComponent.cpp
//  
// Management of a parameter-based graphical interface.
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//
//====================================================================================
#include <UIComponent.h>
namespace DadUI{
//***********************************************************************************
// class cParameterSmallView
//***********************************************************************************

// Initialize the parameter view with associated layers and parameter
void cParameterSmallView::Init(cParameter* pParameter, DadGFX::cLayer* pStaticLayer, DadGFX::cLayer* pDynamicLayer){
    m_pParameter = pParameter;
    m_pStaticLayer = pStaticLayer;
    m_pDynamicLayer = pDynamicLayer;
}

// --------------------------------------------------------------------------
// Refresh the dynamic layer based on the parameter's type of view
void cParameterSmallView::Refresh(){
    switch(m_pParameter->getTypeView()){
        case PARAM_TYPE_VIEW::Normal :
            DrawDynamicLayer();
        break;
        case PARAM_TYPE_VIEW::LeftRight :
            DrawLefRightDynamicLayer();
        break;
        case PARAM_TYPE_VIEW::Discret :
            DrawDiscretDynamicLayer();
        break;
    }
}

// --------------------------------------------------------------------------
// Activate the parameter view, adjusting Z-order and rendering layers
void cParameterSmallView::Activate(){
    m_pStaticLayer->changeZOrder(10);
    m_pDynamicLayer->changeZOrder(11);
    if(m_pParameter->getTypeView() == PARAM_TYPE_VIEW::Discret){
        DrawDiscretStaticLayer();
    }else{
        DrawStaticLayer();
    }
    Refresh();
}

// --------------------------------------------------------------------------
// Deactivate the parameter view, resetting Z-order
void cParameterSmallView::DeActivate(){
    m_pStaticLayer->changeZOrder(0);
    m_pDynamicLayer->changeZOrder(0);
}

// --------------------------------------------------------------------------
// DrawStaticLayer: Renders the static portion of the parameter view.
// This includes the parameter name and fixed graphical elements.
void cParameterSmallView::DrawStaticLayer(){
    // Clear the static layer to prepare for new content
    m_pStaticLayer->eraseLayer();
    
    // Draw the parameter name centered at the top of the layer
    const std::string& ParamName = m_pParameter->getShortName(); // Get the short name of the parameter
    uint16_t NameWitdh = m_pStaticLayer->getTextWidth(ParamName.c_str()); // Measure the text width
    m_pStaticLayer->setCursor((PARAM_STAT_LAYER_WITDH - NameWitdh)/2, 4); // Center the text horizontally
    m_pStaticLayer->drawText(ParamName.c_str()); // Render the parameter name
    
    // Draw the static arcs representing the potentiometer boundaries
    m_pStaticLayer->drawArc(PARAM_STAT_LAYER_WITDH/2, PARAM_STAT_LAYER_HEIGHT-PARAM_POT_RADIUS, 
                            PARAM_POT_RADIUS+1, 
                            PARAM_POT_RADIUS+1+180, (PARAM_POT_ALPHA_MAX+180) % 360,
                            LAYER_POT_COLOR); // Outer arc
    
    m_pStaticLayer->drawArc(PARAM_STAT_LAYER_WITDH/2, PARAM_STAT_LAYER_HEIGHT-PARAM_POT_RADIUS, 
                            PARAM_POT_RADIUS-9, 
                            PARAM_POT_ALPHA_MIN+180, (PARAM_POT_ALPHA_MAX+180) % 360,
                            LAYER_POT_COLOR); // Inner arc
    
    // Calculate the endpoints for the lines delimiting the potentiometer
    float CosAlpha =  std::cos(static_cast<float>(90-PARAM_POT_ALPHA_MIN) * (3.14159265f / 180.f)); // Cosine of angle
    float SinAlpha =  std::sin(static_cast<float>(90-PARAM_POT_ALPHA_MIN) * (3.14159265f / 180.f)); // Sine of angle
    float R1 =  static_cast<float>(PARAM_POT_RADIUS+1); // Outer radius
    float R2 =  static_cast<float>(PARAM_POT_RADIUS-9); // Inner radius
    float DeltaX0 = R1 * CosAlpha; // X-offset for outer point
    float DeltaY0 = R1 * SinAlpha; // Y-offset for outer point
    float DeltaX1 = R2 * CosAlpha; // X-offset for inner point
    float DeltaY1 = R2 * SinAlpha; // Y-offset for inner point
    
    // Calculate and draw the left boundary line
    uint16_t x0 = PARAM_STAT_LAYER_WITDH/2 + static_cast<uint16_t>(DeltaX0); // Outer point x
    uint16_t y0 = PARAM_STAT_LAYER_HEIGHT - PARAM_POT_RADIUS + static_cast<uint16_t>(DeltaY0); // Outer point y
    uint16_t x1 = PARAM_STAT_LAYER_WITDH/2 + static_cast<uint16_t>(DeltaX1); // Inner point x
    uint16_t y1 = PARAM_STAT_LAYER_HEIGHT - PARAM_POT_RADIUS + static_cast<uint16_t>(DeltaY1); // Inner point y
    m_pStaticLayer->drawLine(x0, y0, x1, y1, LAYER_POT_COLOR); // Draw the line

    // Calculate and draw the right boundary line (mirrored horizontally)
    x0 = PARAM_STAT_LAYER_WITDH/2 - static_cast<uint16_t>(DeltaX0); // Outer point x (mirrored)
    x1 = PARAM_STAT_LAYER_WITDH/2 - static_cast<uint16_t>(DeltaX1); // Inner point x (mirrored)
    m_pStaticLayer->drawLine(x0, y0, x1, y1, LAYER_POT_COLOR); // Draw the line
}
// --------------------------------------------------------------------------
// DrawDiscretStaticLayer: Renders the static portion of the parameter view.
// This includes the parameter name and fixed graphical elements.
void cParameterSmallView::DrawDiscretStaticLayer(){
    // Clear the static layer to prepare for new content
    m_pStaticLayer->eraseLayer();
    
    // Draw the parameter name centered at the top of the layer
    const std::string& ParamName = m_pParameter->getShortName();            // Get the short name of the parameter
    uint16_t NameWitdh = m_pStaticLayer->getTextWidth(ParamName.c_str());   // Measure the text width
    m_pStaticLayer->setCursor((PARAM_STAT_LAYER_WITDH - NameWitdh)/2, 4);   // Center the text horizontally
    m_pStaticLayer->drawText(ParamName.c_str());                            // Render the parameter name

    // Draw static point
    m_pStaticLayer->drawArc(PARAM_STAT_LAYER_WITDH/2, PARAM_STAT_LAYER_HEIGHT-PARAM_DISCRET_POT_RADIUS, 
                            PARAM_DISCRET_POT_RADIUS, 
                            PARAM_DISCRET_POT_RADIUS+4+180, (PARAM_POT_ALPHA_MAX+180) % 360,
                            LAYER_POT_COLOR);

    uint8_t NbDiscretValues = m_pParameter->getDiscretValuesSize();
    if(NbDiscretValues != 0){
		for(uint8_t i = 1; i <= NbDiscretValues; i++){
			float Alpha = (270.0 - (static_cast<float>(PARAM_POT_ALPHA)/ static_cast<float>(NbDiscretValues+1)) + static_cast<float>(PARAM_POT_ALPHA_MIN)) * i * __PI / 180.0;;
			float X = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * cos(Alpha);
			float Y = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * sin(Alpha);
			m_pStaticLayer->drawFillCircle((PARAM_STAT_LAYER_WITDH/2) + X, PARAM_STAT_LAYER_HEIGHT - PARAM_DISCRET_POT_RADIUS - Y, PARAM_DISCRET_RADIUS+1, DadGFX::sColor(0,0,0));
			m_pStaticLayer->drawCircle((PARAM_STAT_LAYER_WITDH/2) + X, PARAM_STAT_LAYER_HEIGHT - PARAM_DISCRET_POT_RADIUS - Y, PARAM_DISCRET_RADIUS+1, LAYER_POT_COLOR);
		}
    }
}

// --------------------------------------------------------------------------
// DrawDynamicLayer: Renders the dynamic portion of the parameter view.
// This includes the current parameter value and the graphical representation of its value.
void cParameterSmallView::DrawDynamicLayer(){
    // Clear the dynamic layer to prepare for new content
    m_pDynamicLayer->eraseLayer();
    
    // Render the parameter's current value as text
    std::string strValue; // String to hold the parameter value
    m_pParameter->getStringValue(strValue); // Retrieve the value as a string
    uint16_t TextWitdh = m_pDynamicLayer->getTextWidth(strValue.c_str()); // Measure the text width
    m_pDynamicLayer->setCursor((PARAM_STAT_LAYER_WITDH - TextWitdh)/2, 
                            PARAM_STAT_LAYER_HEIGHT - m_pDynamicLayer->getTextHeight() - 2); // Position the text
    m_pDynamicLayer->drawText(strValue.c_str()); // Render the parameter value

    // Calculate the maximum angle for the graphical representation
    uint16_t AlphaMax = (static_cast<uint16_t>(m_pParameter->getNormalizedValue() 
                    * static_cast<float>(PARAM_POT_ALPHA)) 
                    + 180 + PARAM_POT_ALPHA_MIN) % 360; // Normalize and offset the angle

    // Draw concentric arcs representing the parameter's graphical value
    for (uint8_t i = 0; i < 8; i++) { // Draw 8 layers of arcs for a gradient effect
        m_pDynamicLayer->drawArc(PARAM_STAT_LAYER_WITDH/2, 
                                PARAM_STAT_LAYER_HEIGHT - PARAM_POT_RADIUS - PARAM_STAT_DYN_OFFSET, 
                                PARAM_POT_RADIUS - i, // Reduce the radius for each arc
                                PARAM_POT_ALPHA_MIN + 180, // Starting angle
                                AlphaMax, // Ending angle
                                LAYER_POT_INDEX_COLOR); // Color of the arc
    }
}


// --------------------------------------------------------------------------
// DrawLefRightDynamicLayer: Renders a left-right dynamic visualization for the parameter.
// This includes the parameter value as text and its graphical representation in a circular range.
void cParameterSmallView::DrawLefRightDynamicLayer(){
    // Clear the dynamic layer to prepare for new content
    m_pDynamicLayer->eraseLayer();
    
    // Render the parameter's current value as text
    std::string strValue; // String to hold the parameter value
    m_pParameter->getStringValue(strValue); // Retrieve the value as a string
    uint16_t TextWitdh = m_pDynamicLayer->getTextWidth(strValue.c_str()); // Measure the text width
    m_pDynamicLayer->setCursor((PARAM_STAT_LAYER_WITDH - TextWitdh)/2, 
                            PARAM_STAT_LAYER_HEIGHT - m_pDynamicLayer->getTextHeight() - 2); // Position the text
    m_pDynamicLayer->drawText(strValue.c_str()); // Render the parameter value

    // Calculate the maximum angle for the graphical representation
    uint16_t AlphaMax = (static_cast<uint16_t>(m_pParameter->getNormalizedValue() 
                    * static_cast<float>(PARAM_POT_ALPHA)) 
                    + 180 + PARAM_POT_ALPHA_MIN) % 360; // Normalize and offset the angle

    // Draw arcs representing the parameter's graphical value
    for (uint8_t i = 0; i < 8; i++) { // Draw 8 layers of arcs for a gradient effect
        if (AlphaMax < 180) { // Case when the angle is in the lower half of the circle
            m_pDynamicLayer->drawArc(PARAM_STAT_LAYER_WITDH/2, 
                                    PARAM_STAT_LAYER_HEIGHT - PARAM_POT_RADIUS - PARAM_STAT_DYN_OFFSET, 
                                    PARAM_POT_RADIUS - i, // Reduce the radius for each arc
                                    0, // Starting angle
                                    AlphaMax, // Ending angle
                                    LAYER_POT_INDEX_COLOR); // Color of the arc
        } else { // Case when the angle spans the upper half of the circle
            m_pDynamicLayer->drawArc(PARAM_STAT_LAYER_WITDH/2, 
                                    PARAM_STAT_LAYER_HEIGHT - PARAM_POT_RADIUS - PARAM_STAT_DYN_OFFSET, 
                                    PARAM_POT_RADIUS - i, // Reduce the radius for each arc
                                    AlphaMax, // Starting angle
                                    0, // Ending angle
                                    LAYER_POT_INDEX_COLOR); // Color of the arc
        }
    }
}

void cParameterSmallView::DrawDiscretDynamicLayer(){
    // Clear the dynamic layer to prepare for new content
    m_pDynamicLayer->eraseLayer();
    
    // Render the parameter's current value as text
    std::string strValue; // String to hold the parameter value
    m_pParameter->getDiscretShortValue(strValue); // Retrieve the value as a string
    uint16_t TextWitdh = m_pDynamicLayer->getTextWidth(strValue.c_str()); // Measure the text width
    m_pDynamicLayer->setCursor((PARAM_STAT_LAYER_WITDH - TextWitdh)/2, 
                            PARAM_STAT_LAYER_HEIGHT - m_pDynamicLayer->getTextHeight() - 2); // Position the text
    m_pDynamicLayer->drawText(strValue.c_str()); // Render the parameter value

    uint8_t NbDiscretValues = m_pParameter->getDiscretValuesSize();
    if(NbDiscretValues != 0){
		float Value = m_pParameter->getValue()+1;
		float Alpha = (270.0 - (static_cast<float>(PARAM_POT_ALPHA)/ static_cast<float>(NbDiscretValues+1)) + static_cast<float>(PARAM_POT_ALPHA_MIN)) * Value * __PI / 180.0;;
		float X = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * cos(Alpha);
		float Y = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * sin(Alpha);

		m_pDynamicLayer->drawFillCircle((PARAM_STAT_LAYER_WITDH/2) + X, PARAM_STAT_LAYER_HEIGHT - PARAM_DISCRET_POT_RADIUS - PARAM_STAT_DYN_OFFSET - Y, PARAM_DISCRET_RADIUS, LAYER_POT_INDEX_COLOR);
    }
}

//***********************************************************************************
// class cUIParameter
//***********************************************************************************

// -------------------------------------------------------------------------- 
// Destructor:
cUIParameter::~cUIParameter(){}

// -------------------------------------------------------------------------- 
// InitParameters: Initializes the UI with parameters and their layers.
// Parameter1, Parameter2, Parameter3: pointers to parameters.
void cUIParameter::InitParameters(cParameter* Parameter1, cParameter* Parameter2, cParameter* Parameter3){

    // Initialize Parameter 1 layers and views if the parameter is provided.
    if (Parameter1 != nullptr) {          
        DadGFX::cLayer* pStatLayer = ADD_LAYER(ParamStat1, 0, MENU_HEIGHT, 0);
        pStatLayer->setFont(m_pFont_M);
        pStatLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
        DadGFX::cLayer* pDynLayer = ADD_LAYER(ParamDyn1, 0, PARAM_STAT_DYN_OFFSET + MENU_HEIGHT, 0);
        pDynLayer->setFont(m_pFont_M);
        pDynLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
        m_ParameterSmallView1.Init(Parameter1, pStatLayer, pDynLayer);
    }

    // Initialize Parameter 2 layers and views if the parameter is provided.
    if (Parameter2 != nullptr) {
        DadGFX::cLayer* pStatLayer = ADD_LAYER(ParamStat2, PARAM_STAT_LAYER_WITDH, MENU_HEIGHT, 0);
        pStatLayer->setFont(m_pFont_M);
        pStatLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
        DadGFX::cLayer* pDynLayer = ADD_LAYER(ParamDyn2, PARAM_STAT_LAYER_WITDH, PARAM_STAT_DYN_OFFSET + MENU_HEIGHT, 0);
        pDynLayer->setFont(m_pFont_M);
        pDynLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
        m_ParameterSmallView2.Init(Parameter2, pStatLayer, pDynLayer);
    }

    // Initialize Parameter 3 layers and views if the parameter is provided.
    if (Parameter3 != nullptr) {
        DadGFX::cLayer* pStatLayer = ADD_LAYER(ParamStat3, 2 * PARAM_STAT_LAYER_WITDH, MENU_HEIGHT, 0);
        pStatLayer->setFont(m_pFont_M);
        pStatLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
        DadGFX::cLayer* pDynLayer = ADD_LAYER(ParamDyn3, 2 * PARAM_STAT_LAYER_WITDH, PARAM_STAT_DYN_OFFSET + MENU_HEIGHT, 0);
        pDynLayer->setFont(m_pFont_M);
        pDynLayer->setTextFrontColor(LAYER_PARAMETER_VALUE_COLOR);
        m_ParameterSmallView3.Init(Parameter3, pStatLayer, pDynLayer);
    }
        
    // Initialize Main Layers
    m_pParamDynMain = ADD_LAYER(ParamDynMain, 0, PARAM_STAT_LAYER_HEIGHT + PARAM_STAT_DYN_OFFSET + MENU_HEIGHT, 0);
    m_pParamDynMain->setTextFrontColor(LAYER_PARAMETER_MAIN_COLOR);         
    m_pParamDynMain->setFont(m_pFont_XL);

    m_pParamStatMain = ADD_LAYER(ParamStatMain, 0, PARAM_STAT_LAYER_HEIGHT + PARAM_STAT_DYN_OFFSET + MENU_HEIGHT, 0);
    m_pParamStatMain->setTextFrontColor(LAYER_PARAMETER_MAIN_COLOR);         
    m_pParamStatMain->setFont(m_pFont_L);
}

// -------------------------------------------------------------------------- 
// Proceed: Handles user interaction by updating parameters based on encoder inputs
// and refreshing the views accordingly.
void cUIParameter::Proceed(){
    // Handling updates for the first parameter if it is initialized and encoder1 increment is non-zero
    if((m_ParameterSmallView1.isInit() != 0) && ((m_Encoder1Increment != 0) || (m_Encoder1.getSwitchState()))){
        // Increment the parameter value based on the encoder input
        m_ParameterSmallView1.getParameter()->Increment(m_Encoder1Increment, m_Encoder1.getSwitchState());
        m_Encoder1Increment = 0;
        m_ParameterSmallView1.Refresh();

        // Activate the main parameter view if it's not already active
        if(m_ParamFocusMain != 1){
            m_ParamFocusMain = 1;
            ActivateMain(m_ParameterSmallView1.getParameter());
        }
        // Refresh the main view for this parameter
        if(m_ParamFocusMain == 1){
            RefreshMain(m_ParameterSmallView1.getParameter());
        }
    }

    // Handling updates for the second parameter if it is initialized and encoder2 increment is non-zero
    if((m_ParameterSmallView2.isInit() != 0) && ((m_Encoder2Increment != 0)|| (m_Encoder2.getSwitchState()))){
        m_ParameterSmallView2.getParameter()->Increment(m_Encoder2Increment, m_Encoder2.getSwitchState());
        m_Encoder2Increment = 0;
        m_ParameterSmallView2.Refresh();

        // Activate the main parameter view if it's not already active
        if(m_ParamFocusMain != 2){
            m_ParamFocusMain = 2;
            ActivateMain(m_ParameterSmallView2.getParameter());
        }
        // Refresh the main view for this parameter
        if(m_ParamFocusMain == 2){
            RefreshMain(m_ParameterSmallView2.getParameter());
        }
    }

    // Handling updates for the third parameter if it is initialized and encoder3 increment is non-zero
    if((m_ParameterSmallView3.isInit() != 0) && ((m_Encoder3Increment != 0)|| (m_Encoder3.getSwitchState()))){
        m_ParameterSmallView3.getParameter()->Increment(m_Encoder3Increment, m_Encoder3.getSwitchState());
        m_Encoder3Increment = 0;
        m_ParameterSmallView3.Refresh();

        // Activate the main parameter view if it's not already active
        if(m_ParamFocusMain != 3){
            m_ParamFocusMain = 3;
            ActivateMain(m_ParameterSmallView3.getParameter());
        }
        // Refresh the main view for this parameter
        if(m_ParamFocusMain == 3){
            RefreshMain(m_ParameterSmallView3.getParameter());
        }
    }

    // Handle main view deactivation based on a counter
    if(m_CtMainActivate != 0){
        m_CtMainActivate--;
        if(m_CtMainActivate == 0){
            DeActivateMain();
            m_ParamFocusMain = 0;
        }
    }
}

// -------------------------------------------------------------------------- 
// ActivateMain: Activates the main view for the given parameter by bringing its 
// layers to the front and displaying the parameter's long name.
void cUIParameter::ActivateMain(cParameter* pParameter){
    // Change the Z-order to bring the main layers to the front
    m_pParamStatMain->changeZOrder(10);
    m_pParamDynMain->changeZOrder(11);

    // Erase previous layer content and draw the new parameter's name
    m_pParamStatMain->eraseLayer();
    const std::string& ParamName = pParameter->getLongName();
    uint16_t NameWitdh = m_pParamStatMain->getTextWidth(ParamName.c_str());
    m_pParamStatMain->setCursor((PARAM_LAYER_MAIN_WITDH - NameWitdh)/2, 4);
    m_pParamStatMain->drawText(ParamName.c_str());
}

// -------------------------------------------------------------------------- 
// RefreshMain: Refreshes the main view for the given parameter by erasing the 
// dynamic layer and displaying the updated parameter value.
void cUIParameter::RefreshMain(cParameter* pParameter){
    // Set a countdown timer for main focus deactivation
    m_CtMainActivate = TIME_FOCUS_MAIN;

    // Erase previous content from the dynamic layer and draw the updated value
    m_pParamDynMain->eraseLayer();
    std::string strValue;
    if(pParameter->getTypeView()==PARAM_TYPE_VIEW::Discret){
        pParameter->getDiscretLongValue(strValue);
    }else{
        pParameter->getStringValue(strValue);
    }
    uint16_t TextWitdh = m_pParamDynMain->getTextWidth(strValue.c_str());
    m_pParamDynMain->setCursor((PARAM_LAYER_MAIN_WITDH - TextWitdh)/2, 
                                ((PARAM_LAYER_MAIN_HEIGHT + m_pParamStatMain->getTextHeight() - m_pParamDynMain->getTextHeight())/2) -4 );
    m_pParamDynMain->drawText(strValue.c_str());
}

// -------------------------------------------------------------------------- 
// DeActivateMain: Deactivates the main view by resetting the Z-order of the layers.
void cUIParameter::DeActivateMain(){
    // Reset the Z-order of the main layers to their default state
    m_pParamStatMain->changeZOrder(0);
    m_pParamDynMain->changeZOrder(0);
}

// -------------------------------------------------------------------------- 
// Activate: Activates the small parameter views if they are initialized.
void cUIParameter::Activate(){
    if(m_ParameterSmallView1.isInit() != 0){
        m_ParameterSmallView1.Activate();
    }
    if(m_ParameterSmallView2.isInit() != 0){
        m_ParameterSmallView2.Activate();
    }
    if(m_ParameterSmallView3.isInit() != 0){
        m_ParameterSmallView3.Activate();
    }
    m_CtMainActivate = 0;
    m_ParamFocusMain = 0;
}

// -------------------------------------------------------------------------- 
// DeActivate: Deactivates the small parameter views if they are initialized.
void cUIParameter::DeActivate(){
    if(m_ParameterSmallView1.isInit() != 0){
        m_ParameterSmallView1.DeActivate();
    }
    if(m_ParameterSmallView2.isInit() != 0){
        m_ParameterSmallView2.DeActivate();
    }
    if(m_ParameterSmallView3.isInit() != 0){
        m_ParameterSmallView3.DeActivate();
    }
    m_CtMainActivate = 0;
    DeActivateMain();
    m_ParamFocusMain = 0;
}

//***********************************************************************************
// class cUISaveRestore
//***********************************************************************************

// -------------------------------------------------------------------------- 
// Member static 
DadGFX::cLayer*      cUISaveRestore::m_pSaveStatMain=nullptr;  
DadGFX::cLayer*      cUISaveRestore::m_pSaveDynMain=nullptr; 


Dad::cSerialize                  cUISaveRestore::m_Serializer;
std::vector<DadUI::cParameter *> cUISaveRestore::m_TabParameter;

// -------------------------------------------------------------------------- 
// Destructor: Ensures proper cleanup of dynamically allocated fonts.
cUISaveRestore::~cUISaveRestore(){

};
// --------------------------------------------------------------------------    
// Initialize 
void cUISaveRestore::InitSaveRestore(){

    if(m_pSaveStatMain==nullptr){
        m_pSaveStatMain = ADD_LAYER(SaveRestoreUIStat,0,MENU_HEIGHT,0);
        m_pSaveDynMain = ADD_LAYER(SaveRestoreUIDyn,0, MENU_HEIGHT,0);
        m_TabParameter.clear();
    }
}

// -------------------------------------------------------------------------- 
// Activate:
void cUISaveRestore::Activate(){
    m_pSaveStatMain->changeZOrder(11);
    m_pSaveDynMain->changeZOrder(10);
    m_pSaveStatMain->eraseLayer();
    m_pSaveStatMain->setTextFrontColor(LAYER_PARAMETER_MAIN_COLOR);
}

// -------------------------------------------------------------------------- 
// DeActivate:
void cUISaveRestore::DeActivate(){
    m_pSaveStatMain->changeZOrder(0);
    m_pSaveDynMain->changeZOrder(0);
}

//***********************************************************************************
// class cUISave
//***********************************************************************************

// -------------------------------------------------------------------------- 
// Activate: Activates the small parameter views if they are initialized.
void cUISave::Activate(){
    cUISaveRestore::Activate();
    m_pSaveStatMain->setFont(m_pFont_L);
    uint16_t TextWidth = m_pSaveStatMain->getTextWidth("Save");
    m_pSaveStatMain->setCursor((m_pSaveDynMain->getWith() - TextWidth)/2, 10);
    m_pSaveStatMain->drawText("Save");
    m_pSaveStatMain->setFont(m_pFont_M);
    m_pSaveStatMain->setCursor(10,100);
    m_pSaveStatMain->drawText("Slot : ");
 /*   
    m_pSaveStatMain->setCursor(10,100);
    
    m_pSaveStatMain->eraseLayer();
    m_pSaveStatMain->setFont(m_pFont1);
    //TextWidth = m_pSaveDynMain->getTextWidth("Save");
    //m_pSaveDynMain->setCursor(TextWidth + 20,100);
    char Buff[4];
    sprintf(Buff, "%d", m_MemorySlot);
    m_pSaveStatMain->drawText(Buff);
    */
}

// -------------------------------------------------------------------------- 
// Proceed: Handles user interaction by updating parameters based on encoder inputs
// and refreshing the views accordingly.
void cUISave::Proceed(){
    if(m_Encoder1Increment != 0){
        m_MemorySlot += m_Encoder1Increment;
        if(m_Encoder1Increment > 9){
            m_Encoder1Increment = 9;
        }else if(m_Encoder1Increment < 0){
            m_Encoder1Increment = 0;
        }
        char Buff[4];
        sprintf(Buff, "%d", m_MemorySlot);
        m_pSaveDynMain->setCursor(60 + 20,100);
        m_pSaveStatMain->drawText(Buff);
    }
}


//***********************************************************************************
// class cUIRestore
//***********************************************************************************
// -------------------------------------------------------------------------- 
// Activate: Activates the small parameter views if they are initialized.
void cUIRestore::Activate(){
    
    cUISaveRestore::Activate();
    m_pSaveStatMain->setFont(m_pFont_L);
    uint16_t TextWidth = m_pSaveStatMain->getTextWidth("Restore");
    m_pSaveStatMain->setCursor((m_pSaveDynMain->getWith() - TextWidth)/2, 10);
    m_pSaveStatMain->drawText("Restore");
    m_pSaveStatMain->setFont(m_pFont_M);
    m_pSaveStatMain->setCursor(10,100);
    m_pSaveStatMain->drawText("Slot : ");
    Proceed();
}

// -------------------------------------------------------------------------- 
// Proceed: Handles user interaction by updating parameters based on encoder inputs
// and refreshing the views accordingly.
void cUIRestore::Proceed(){
    uint8_t test = 10;
} 

//***********************************************************************************
// class cUIMainParameter
// This class manages a tabbed UI interface.
//***********************************************************************************
// Display layers
DadGFX::cLayer* cUIMain::m_pStaticLayer=nullptr;   // Static layer for persistent elements
DadGFX::cLayer* cUIMain::m_pDynamicLayer=nullptr;  // Dynamic layer for real-time updates
    

// -------------------------------------------------------------------------- 
// InitMainParameter: Initializes the UI components including fonts and layers
// Sets up both static and dynamic layers with appropriate properties
void cUIMain::InitMain(){
    if(m_pStaticLayer == nullptr){

        // Create dynamic layer for interactive elements
        m_pDynamicLayer = ADD_LAYER(ParamMainUIDyn, 0, 0, 0);

        // Create static layer for fixed elements and configure its properties
        m_pStaticLayer = ADD_LAYER(ParamMainUIStat, 0, 0, 0);
        m_pStaticLayer->setTextFrontColor(LAYER_PARAMETER_MAIN_COLOR);         
        m_pStaticLayer->setFont(m_pFont_S);
    }
}

// -------------------------------------------------------------------------- 
// Activate: Prepares and displays the UI interface
// - Arranges layers in correct Z-order
// - Draws parameter names in the static layer
// - Initializes selection state if parameters exist
void cUIMain::Activate(){
    m_pStaticLayer->changeZOrder(21);
    m_pStaticLayer->eraseLayer();
    m_pDynamicLayer->changeZOrder(20);

    // Draw parameter names centered in their respective slots
    uint16_t xItem=0;
    for(const auto& Chlid : m_pTabChilds){
        uint16_t TextWidth = m_pStaticLayer->getTextWidth(Chlid.m_Name.c_str());
        m_pStaticLayer->setCursor(xItem + (MENU_ITEM_WitDH-TextWidth)/2, 4);
        m_pStaticLayer->drawText(Chlid.m_Name.c_str());
        xItem += MENU_ITEM_WitDH;
    }
    
    // Initialize selection state if parameters exist
    if(m_pTabChilds.size() != 0){
        m_ActiveSubUI = 0;
        m_SelectedSubUI = 0;
        m_pDynamicLayer->eraseLayer(MENU_BACK_COLOR);
        m_pDynamicLayer->drawRect(m_SelectedSubUI*MENU_ITEM_WitDH, 0, MENU_ITEM_WitDH, MENU_HEIGHT, 2, MENU_SELETED_ITEM_COLOR);
        m_pDynamicLayer->drawFillRect(m_ActiveSubUI*MENU_ITEM_WitDH, 0, MENU_ITEM_WitDH, MENU_HEIGHT, MENU_ACTIVE_ITEM_COLOR);
        m_pTabChilds[m_ActiveSubUI].m_pChild->Activate();
    }
}

// -------------------------------------------------------------------------- 
// DeActivate: Cleans up the UI interface when it's no longer active
// - Resets layer Z-order
// - Deactivates active parameter if one exists
void cUIMain::DeActivate(){
    m_pStaticLayer->changeZOrder(0);
    m_pDynamicLayer->changeZOrder(0);
    if(m_ActiveSubUI >= 0){
        m_pTabChilds[m_ActiveSubUI].m_pChild->DeActivate();
    }
}

// -------------------------------------------------------------------------- 
// Proceed: Handles UI updates and user interaction
// - Processes encoder input for parameter selection
// - Manages parameter activation/deactivation
// - Updates visual feedback
void cUIMain::Proceed(){
    if(m_ActiveSubUI >= 0){
        // Handle encoder rotation for parameter selection
        if(m_Encoder0Increment != 0){
        	m_SelectedSubUI += m_Encoder0Increment;
            // Wrap selection around bounds
        	uint8_t NbChild = static_cast<int8_t>(m_pTabChilds.size());
            if(m_SelectedSubUI < 0){
            	m_SelectedSubUI = 0;
            } else if (m_SelectedSubUI >= NbChild){
            	m_SelectedSubUI = NbChild-1;
            }
            m_Encoder0Increment = 0;                
        }

        // Handle encoder press to activate selected parameter
        if(m_Encoder0.getSwitchState() && (m_ActiveSubUI != m_SelectedSubUI)){
            if(m_pTabChilds[m_SelectedSubUI].m_pChild->isMain() == false){
                m_pTabChilds[m_ActiveSubUI].m_pChild->DeActivate();
                m_ActiveSubUI = m_SelectedSubUI;
                m_pTabChilds[m_ActiveSubUI].m_pChild->Activate();
            }else{
                m_pTabChilds[m_ActiveSubUI].m_pChild->DeActivate();
                DeActivate();
                m_pActiveUI = m_pTabChilds[m_SelectedSubUI].m_pChild;
                m_pTabChilds[m_SelectedSubUI].m_pChild->Activate();
            }
        }
        
        // Update visual feedback for selection state
        m_pDynamicLayer->eraseLayer(MENU_BACK_COLOR);
        m_pDynamicLayer->drawFillRect(m_ActiveSubUI*MENU_ITEM_WitDH, 0, MENU_ITEM_WitDH, MENU_HEIGHT, MENU_ACTIVE_ITEM_COLOR);
        m_pDynamicLayer->drawRect(m_SelectedSubUI*MENU_ITEM_WitDH, 0, MENU_ITEM_WitDH, MENU_HEIGHT, 2, MENU_SELETED_ITEM_COLOR);
        
        // Update active parameter
        m_pTabChilds[m_ActiveSubUI].m_pChild->Proceed();
    }
}

} // DadUI
