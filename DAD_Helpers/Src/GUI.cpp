//====================================================================================
// GUI.cpp
//
// Management of PENDA Graphical User Interface 
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "GUI.h"
#include <algorithm>

namespace DadUI {

//***********************************************************************************
// 
cUI*           		cUI::m_pActiveUI;
cEncoder 	 		cUI::m_Encoder0;
cEncoder 	 		cUI::m_Encoder1;
cEncoder 	 		cUI::m_Encoder2;
cEncoder       		cUI::m_Encoder3;
int32_t        		cUI::m_Encoder0Increment=0;
int32_t        		cUI::m_Encoder1Increment=0;
int32_t        		cUI::m_Encoder2Increment=0;
int32_t        		cUI::m_Encoder3Increment=0;
DadGFX::cFont*		cUI::m_pFont_S = nullptr;
DadGFX::cFont*		cUI::m_pFont_M = nullptr;
DadGFX::cFont*		cUI::m_pFont_L = nullptr;;
DadGFX::cFont*		cUI::m_pFont_XL = nullptr;;


//***********************************************************************************
// class cParameter
// This class represents a parameter with a value constrained within a specified range.
// It provides functionality to initialize, modify, and retrieve the value with bounds checking.
// Additionally, the class supports rapid and slow increments, as well as normalized value handling
// for ease of scaling within the range.
//***********************************************************************************

// --------------------------------------------------------------------------
// Initialize the parameter with given attributes
void cParameter::Init(const std::string& ShortName, const std::string& LongName, float InitValue, float Min, float Max, float RapidIncrement, float SlowIncrement, PARAM_TYPE_VIEW TypeView) {
    m_ShortName = ShortName;
    m_LongName = LongName;
    m_Min = Min;
    m_Max = Max;
    m_RapidIncrement = RapidIncrement;
    m_SlowIncrement = SlowIncrement;

    // Ensure the initial value is within bounds
    if(InitValue > m_Max) {
        m_Value = m_Max;
    } else if(InitValue < m_Min){
        m_Value= m_Min;
    } else {
        m_Value = InitValue;
    }
    m_TypeView = TypeView;
}

// --------------------------------------------------------------------------
// Increment the parameter value by a number of steps
void cParameter::Increment(int32_t nbStep, bool Switch) {
    if(Switch == true){
        m_Value += m_RapidIncrement * nbStep; // Use rapid increment
    } else {
        m_Value += m_SlowIncrement * nbStep; // Use slow increment
    }
    // Clamp the value to the allowed range
    if(m_Value > m_Max) {
        m_Value = m_Max;
    } else if(m_Value < m_Min){
        m_Value= m_Min;
    }
}

// --------------------------------------------------------------------------
// Get the parameter value as a formatted string
void cParameter::getStringValue(std::string& strValue) const {
    int integerPart = static_cast<int>(m_Value); // Extract integer part
    float fractionalPart = m_Value - integerPart; // Extract fractional part
    char Buffer[20];
    if(fractionalPart < 0){
        fractionalPart = -fractionalPart; // Ensure positive fractional part
        snprintf(Buffer, 20, "-%d.%03d", integerPart, static_cast<int>(fractionalPart*1000));

    }else{
        snprintf(Buffer, 20, "%d.%03d", integerPart, static_cast<int>(fractionalPart*1000));
    }
    strValue = Buffer;
}

// --------------------------------------------------------------------------
//
void cParameter::addDiscretValue(const char * ShortValue, const char * LongValue){
    sDiscretValues Values;
    Values.m_LongValue = LongValue;
    Values.m_ShortValue = ShortValue;
    m_TabDiscretValues.push_back(Values);
    m_Max = static_cast<float>(m_TabDiscretValues.size()-1);
    m_Min = 0;
    m_SlowIncrement = 1;
    m_RapidIncrement = 1;
    m_TypeView = DadUI::PARAM_TYPE_VIEW::Discret;
}

//***********************************************************************************
// cUI class
//***********************************************************************************

// --------------------------------------------------------------------------
// Static method to initialize the encoders.
void cUI::Init() {
	#define EncoderUpdatePeriodMs 10U
	#define SwitchUpdatePeriodMs 100U

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

    m_pFont_S	= new DadGFX::cFont(FONTS);
    m_pFont_M	= new DadGFX::cFont(FONTM);
    m_pFont_L	= new DadGFX::cFont(FONTL);
    m_pFont_XL	= new DadGFX::cFont(FONTXL);
}

// --------------------------------------------------------------------------
// Method to add a child UI component to the current UI component.
void cUI::addChild(cUI *pChild, const char *Name, uint8_t Order) {
    // Create a new child structure.
    sUIChild Child;
    Child.m_Name = Name;      // Set the name of the child.
    Child.m_Order = Order;    // Set the order/priority of the child.
    Child.m_pChild = pChild;  // Link the child to the parent.

    // Add the new child to the list of children.
    m_pTabChilds.push_back(Child);

    // Set the current object as the parent of the child.
    pChild->m_pParent = this;

    // Sort children by their order in ascending order.
    std::sort(m_pTabChilds.begin(), m_pTabChilds.end(), [](const sUIChild& a, const sUIChild& b) {
        return a.m_Order < b.m_Order;
    });
}

} // DadUI
