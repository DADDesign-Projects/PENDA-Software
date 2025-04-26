//====================================================================================
// Prameter.cpp
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "Parameter.h"

namespace DadUI {

//***********************************************************************************
// class cParameter
//***********************************************************************************

// --------------------------------------------------------------------------
// Initialize the parameter with given attributes
void cParameter::Init(float InitValue, float Min, float Max,
					  float RapidIncrement, float SlowIncrement,
					  CallbackType Callback, uint32_t CallbackUserData,
					  float Slope,
					  uint8_t Control){
    m_Min = Min;
    m_Max = Max;
    m_RapidIncrement = RapidIncrement;
    m_SlowIncrement = SlowIncrement;
    m_Callback = Callback;
    m_CallbackUserData = CallbackUserData;
    if(Slope == 0){
    	m_Step = (Max-Min);
    }else{
    	m_Step = (Max-Min)/ Slope;
    }
    m_TargetValue = InitValue;
    m_Slope = Slope;
    if(Control != 0xFF){
    	cPendaUI::m_Midi.addControlChangeCallback(Control, (uint32_t) this, MIDIControlChangeCallBack );
    }

    // Ensure the initial value is within bounds
	setValue(InitValue);
}

// --------------------------------------------------------------------------
// Set the parameter value directly with boundary checks
void cParameter::setValue(float value) {
    if(value > m_Max) {
    	m_TargetValue = m_Max;
    } else if(value < m_Min){
    	m_TargetValue= m_Min;
    } else {
    	m_TargetValue = value;
    }
}

// --------------------------------------------------------------------------
// Update the current value smoothly according to the slope
void cParameter::RTProcess() {
    if(m_Value != m_TargetValue){
		if (m_Value < m_TargetValue)
		{
			m_Value += m_Step;
			if (m_Value > m_TargetValue)
				m_Value = m_TargetValue; // Prevent overshoot
		}
		else if (m_Value > m_TargetValue)
		{
			m_Value -= m_Step;
			if (m_Value < m_TargetValue)
				m_Value = m_TargetValue; // Prevent overshoot
		}

	    // Call the callback if it is defined
		if (m_Callback) {
			m_Callback(this, m_CallbackUserData);
		}
    }
}

// --------------------------------------------------------------------------
// Increment the parameter value by a number of steps
void cParameter::Increment(int32_t nbStep, bool Switch) {
    float Value = m_TargetValue;
	if(Switch == false){
        Value += m_RapidIncrement * nbStep; // Use rapid increment
    } else {
        Value += m_SlowIncrement * nbStep; // Use slow increment
    }
	cPendaUI::m_Memory.setDirty();
    setValue(Value);
}

// --------------------------------------------------------------------------
// Function call when this CC is received
void cParameter::MIDIControlChangeCallBack(uint8_t control, uint8_t value, uint32_t userData){
	cParameter *pThis = (cParameter *)userData;
	value = value > 127 ? 127 : value;
	float NewVal = pThis->m_Min + (value * (pThis->m_Max - pThis->m_Min)) / 127.0;
	pThis->setValue(NewVal);
	cPendaUI::m_Memory.setDirty();
	cPendaUI::ReDraw();
}


//***********************************************************************************
// class cParameterView
//***********************************************************************************

// --------------------------------------------------------------------------
// Initialize the parameter with given attributes
void cParameterView::Init(cParameter* pParameter, const std::string& ShortName, const std::string& LongName){
	m_pParameter = pParameter;
	m_ShortName = ShortName;
	m_LongName = LongName;
}

//***********************************************************************************
// class cParameterNumView
//***********************************************************************************
// --------------------------------------------------------------------------

// Initialize the parameter with given attributes
void cParameterNumView::Init(cParameter* pParameter, const std::string& ShortName, const std::string& LongName,
		  const std::string& ShortUnit, const std::string& LongUnit, uint8_t StringPrecision){

		cParameterView::Init(pParameter, ShortName, LongName);
		m_ShortUnit = ShortUnit;
		m_LongUnit = LongUnit;
		m_StringPrecision = StringPrecision;
}

// --------------------------------------------------------------------------
// ValueToString: Converts the stored value (m_Value) into a string
std::string cParameterNumView::ValueToString() const{
    const int MAX_SIZE = 20;  // Maximum buffer size for string conversion
    char buffer[MAX_SIZE];
    int index = 0;

    float value  = m_pParameter->getTargetValue();

     // Special case: If the value is exactly zero, return "0"
    if (value == 0.0f) {
    	buffer[0] = '0';
    	buffer[1] = '.';
        for(uint8_t Index=2; Index < m_StringPrecision+1; Index++){
        	buffer[Index] = '0';
        }
        return std::string(buffer);
    }

    // Handle negative values
    if (value < 0) {
        buffer[index++] = '-';
        value = -value; // Work with the absolute value
    }

    // Extract integer and fractional parts
    int integerPart = static_cast<int>(value);
    float fractionalPart = value - integerPart;

    // Convert the integer part to string
    int integerDigits = 0;
    if (integerPart == 0) {
        buffer[index++] = '0';
    } else {
        int temp = integerPart;
        //int integerDigits = 0;

        // Count the number of digits in the integer part
        while (temp > 0) {
            integerDigits++;
            temp /= 10;
        }

        // Fill the buffer with integer digits in correct order
        for (int i = integerDigits - 1; i >= 0; --i) {
            buffer[index + i] = (integerPart % 10) + '0';
            integerPart /= 10;
        }
        index += integerDigits;
    }

    // Process fractional part if required
    int decimalDigits = m_StringPrecision-integerDigits;
	if (m_StringPrecision > 0) {
        buffer[index++] = '.';  // Decimal separator

        for (int i = 0; i < decimalDigits; ++i) {
            fractionalPart *= 10;
            int digit = static_cast<int>(fractionalPart);
            buffer[index++] = digit + '0';
            fractionalPart -= digit;

            // Prevent buffer overflow (edge case, should rarely happen)
            if (index >= MAX_SIZE - 1) {
                break;
            }
        }
    }

    buffer[index] = '\0'; // Null-terminate the string

    return std::string(buffer);
}

//***********************************************************************************
// class cParameterNumNormalView
//***********************************************************************************

// --------------------------------------------------------------------------
// Draw Static Form view
void cParameterNumNormalView::drawStatFormView(DadGFX::cLayer* pStatLayer){
    // Clear the static layer to prepare for new content
    pStatLayer->eraseLayer();

    // Draw the parameter name centered at the top of the layer
    uint16_t NameWidth = pStatLayer->getTextWidth(m_ShortName.c_str()); // Measure the text width
    //pStatLayer->setCursor((PARAM_WIDTH - NameWidth)/2, (PARAM_HEIGHT - pStatLayer->getTextHeight())/2); // Center the text horizontally
    pStatLayer->setCursor((PARAM_WIDTH - NameWidth)/2,(PARAM_NAME_HEIGHT - pStatLayer->getTextHeight())/2 ); // Center the text horizontally
    pStatLayer->drawText(m_ShortName.c_str()); // Render the parameter name

    // Draw the static arcs representing the potentiometer boundaries
    pStatLayer->drawArc(PARAM_WIDTH/2, PARAM_HEIGHT/2,
                            PARAM_POT_RADIUS+1,
                            PARAM_POT_RADIUS+1+180, (PARAM_POT_ALPHA_MAX+180) % 360,
							LAYER_POT_INDEX_COLOR);//LAYER_POT_COLOR); // Outer arc

    pStatLayer->drawArc(PARAM_WIDTH/2, PARAM_HEIGHT/2,
                            PARAM_POT_RADIUS-9,
                            PARAM_POT_ALPHA_MIN+180, (PARAM_POT_ALPHA_MAX+180) % 360,
							LAYER_POT_INDEX_COLOR);//LAYER_POT_COLOR); // Inner arc

    // Calculate the endpoints for the lines delimiting the potentiometer
    float CosAlpha =  std::cos(Deg2Rad(90.0f - (float)PARAM_POT_ALPHA_MIN)); // Cosine of angle
    float SinAlpha =  std::sin(Deg2Rad(90.0f - (float)PARAM_POT_ALPHA_MIN)); // Sine of angle
    float R1 =  static_cast<float>(PARAM_POT_RADIUS+1); // Outer radius
    float R2 =  static_cast<float>(PARAM_POT_RADIUS-9); // Inner radius
    float DeltaX0 = R1 * CosAlpha; // X-offset for outer point
    float DeltaY0 = R1 * SinAlpha; // Y-offset for outer point
    float DeltaX1 = R2 * CosAlpha; // X-offset for inner point
    float DeltaY1 = R2 * SinAlpha; // Y-offset for inner point

    // Calculate and draw the left boundary line
    uint16_t x0 = PARAM_WIDTH/2 + static_cast<uint16_t>(DeltaX0); // Outer point x
    uint16_t y0 = PARAM_HEIGHT/2 + static_cast<uint16_t>(DeltaY0); // Outer point y
    uint16_t x1 = PARAM_WIDTH/2 + static_cast<uint16_t>(DeltaX1); // Inner point x
    uint16_t y1 = PARAM_HEIGHT/2 + static_cast<uint16_t>(DeltaY1); // Inner point y
    pStatLayer->drawLine(x0, y0, x1, y1, LAYER_POT_INDEX_COLOR);//LAYER_POT_COLOR); // Draw the line

    // Calculate and draw the right boundary line (mirrored horizontally)
    x0 = PARAM_WIDTH/2 - static_cast<uint16_t>(DeltaX0); // Outer point x (mirrored)
    x1 = PARAM_WIDTH/2 - static_cast<uint16_t>(DeltaX1); // Inner point x (mirrored)
    pStatLayer->drawLine(x0, y0, x1, y1, LAYER_POT_INDEX_COLOR);//LAYER_POT_COLOR); // Draw the line

}

// --------------------------------------------------------------------------
// Draw Dynamic Form view

void cParameterNumNormalView::drawDynFormView(DadGFX::cLayer* pDynLayer){
    // Clear the dynamic layer to prepare for new content
    pDynLayer->eraseLayer();

    // Render the parameter's current value as text
    char Buffer[30];
	snprintf(Buffer, sizeof(Buffer), "%s %s", ValueToString().c_str(), m_LongUnit.c_str());
    uint16_t TextWitdh = pDynLayer->getTextWidth(Buffer); 	// Measure the text width
    pDynLayer->setCursor((PARAM_WIDTH - TextWitdh)/2,
    					  PARAM_HEIGHT - ((PARAM_VAL_HEIGHT + pDynLayer->getTextHeight())/2)); // Position the text
    pDynLayer->drawText(Buffer); // Render the parameter value

    // Calculate the maximum angle for the graphical representation
    uint16_t AlphaMax = (static_cast<uint16_t>(m_pParameter->getNormalizedTargetValue()
                         * static_cast<float>(PARAM_POT_ALPHA))
                         + 180 + PARAM_POT_ALPHA_MIN) % 360; // Normalize and offset the angle

    // Draw concentric arcs representing the parameter's graphical value
    for (uint8_t i = 0; i < 11; i++) { // Draw 8 layers of arcs for a gradient effect
        pDynLayer->drawArc(PARAM_WIDTH/2,
        				   PARAM_HEIGHT/2,
                           PARAM_POT_RADIUS - i + 1, // Reduce the radius for each arc
                           PARAM_POT_ALPHA_MIN + 180, // Starting angle
                           AlphaMax, // Ending angle
						   //TAB_POT_COLOR[i]);
                           LAYER_POT_INDEX_COLOR); // Color of the arc
    }
}

// --------------------------------------------------------------------------
// Draw Static Form view
#define NAME_OFFSET 1
void cParameterNumNormalView::drawStatMainView(DadGFX::cLayer* pStatLayer){
    // Erase previous layer content and draw the new parameter's name
	pStatLayer->eraseLayer(MAIN_BACK_COLOR);
	pStatLayer->setFont(cPendaUI::m_pFont_L);
    uint16_t NameWidth = pStatLayer->getTextWidth(m_LongName.c_str());
    pStatLayer->setCursor((MAIN_WIDTH - NameWidth)/2, NAME_OFFSET);
    pStatLayer->drawText(m_LongName.c_str());
    m_NameHeight = pStatLayer->getTextHeight() + NAME_OFFSET;
}

// --------------------------------------------------------------------------
// Draw Dynamic Form view

void cParameterNumNormalView::drawDynMainView(DadGFX::cLayer* pDynLayer){
    // Erase previous content from the dynamic layer and draw the updated value
	pDynLayer->eraseLayer();

    char Buffer[30];
	snprintf(Buffer, sizeof(Buffer), "%s %s", ValueToString().c_str(), m_LongUnit.c_str());
	pDynLayer->setFont(cPendaUI::m_pFont_XL);
    uint16_t TextWitdh = pDynLayer->getTextWidth(Buffer);
    pDynLayer->setCursor((MAIN_WIDTH - TextWitdh)/2, m_NameHeight + 1);
    pDynLayer->drawText(Buffer);
}


//***********************************************************************************
// class cParameterNumLeftRightView
//***********************************************************************************

// --------------------------------------------------------------------------
// Draw Dynamic Form view

void cParameterNumLeftRightView::drawDynFormView(DadGFX::cLayer* pDynLayer){
    // Clear the dynamic layer to prepare for new content
    pDynLayer->eraseLayer();

    // Render the parameter's current value as text
    // Render the parameter's current value as text
    char Buffer[30];
	snprintf(Buffer, sizeof(Buffer), "%s %s", ValueToString().c_str(), m_LongUnit.c_str());
    uint16_t TextWitdh = pDynLayer->getTextWidth(Buffer); 	// Measure the text width
    pDynLayer->setCursor((PARAM_WIDTH - TextWitdh)/2,
    					  PARAM_HEIGHT - ((PARAM_VAL_HEIGHT + pDynLayer->getTextHeight())/2)); // Position the text
    pDynLayer->drawText(Buffer); // Render the parameter value

    // Calculate the maximum angle for the graphical representation
    uint16_t AlphaMax = (static_cast<uint16_t>(m_pParameter->getNormalizedTargetValue()
                         * static_cast<float>(PARAM_POT_ALPHA))
                         + 180 + PARAM_POT_ALPHA_MIN) % 360; // Normalize and offset the angle

    // Draw arcs representing the parameter's graphical value
    for (uint8_t i = 0; i < 11; i++) { // Draw 8 layers of arcs for a gradient effect
        if (AlphaMax < 180) { // Case when the angle is in the lower half of the circle
            pDynLayer->drawArc(PARAM_WIDTH/2,
            				   PARAM_HEIGHT/2,
                               PARAM_POT_RADIUS - i + 1, // Reduce the radius for each arc
                               0, // Starting angle
                               AlphaMax, // Ending angle
                               LAYER_POT_INDEX_COLOR); // Color of the arc
        } else { // Case when the angle spans the upper half of the circle
            pDynLayer->drawArc(PARAM_WIDTH/2,
            				   PARAM_HEIGHT/2,
                               PARAM_POT_RADIUS - i + 1, // Reduce the radius for each arc
                               AlphaMax, // Starting angle
                               0, // Ending angle
                               LAYER_POT_INDEX_COLOR); // Color of the arc
        }
    }
}

//***********************************************************************************
// class cParameterDiscretView
//***********************************************************************************


// --------------------------------------------------------------------------
// Add discrete value
void cParameterDiscretView::AddDiscreteValue(const std::string& ShortDiscretValue, const std::string& LongDiscretValue){
	sDiscretValues Values;
    Values.m_LongValue = LongDiscretValue;
    Values.m_ShortValue = ShortDiscretValue;
    m_TabDiscretValues.push_back(Values);
    m_pParameter->setMaxValue((float)m_TabDiscretValues.size()-1);
}

// --------------------------------------------------------------------------
// Draw Static Form view
void cParameterDiscretView::drawStatFormView(DadGFX::cLayer* pStatLayer){
    // Clear the static layer to prepare for new content
    pStatLayer->eraseLayer();
    if(m_TabDiscretValues.size() == 0) return;

    // Draw the parameter name centered at the top of the layer
    uint16_t NameWidth = pStatLayer->getTextWidth(m_ShortName.c_str());   // Measure the text width
    pStatLayer->setCursor((PARAM_WIDTH - NameWidth)/2, (PARAM_NAME_HEIGHT - pStatLayer->getTextHeight())/2);  // Center the text
    pStatLayer->drawText(m_ShortName.c_str());                            // Render the parameter name

    // Draw static point
    pStatLayer->drawArc(PARAM_WIDTH/2, PARAM_HEIGHT/2,
                            PARAM_DISCRET_POT_RADIUS,
                            PARAM_DISCRET_POT_RADIUS+4+180, (PARAM_POT_ALPHA_MAX+180) % 360,
                            LAYER_POT_COLOR);

    uint8_t NbDiscretValues = m_TabDiscretValues.size();
    if(NbDiscretValues != 0){
        float IncAlpha = Deg2Rad(static_cast<float>(PARAM_POT_ALPHA) / static_cast<float>(NbDiscretValues+1));
		float Alpha = Deg2Rad(240.0f) - (IncAlpha);
    	for(uint8_t i = 1; i <= NbDiscretValues; i++){
			float X = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * std::cos(Alpha);
			float Y = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * std::sin(Alpha);
			pStatLayer->drawCircle((PARAM_WIDTH/2) + X, PARAM_HEIGHT/2 - Y, PARAM_DISCRET_RADIUS+1, LAYER_POT_COLOR);
			Alpha -= IncAlpha;
		}
    }
}
// --------------------------------------------------------------------------
// Draw Dynamic Form view

void cParameterDiscretView::drawDynFormView(DadGFX::cLayer* pDynLayer){
    // Clear the dynamic layer to prepare for new content
    pDynLayer->eraseLayer();
    if(m_TabDiscretValues.size() == 0) return;

    // Render the parameter's current value as text
    uint8_t NumValue = (uint8_t) m_pParameter->getTargetValue();

    uint16_t TextWitdh = pDynLayer->getTextWidth(m_TabDiscretValues[NumValue].m_ShortValue.c_str());

    pDynLayer->setCursor((PARAM_WIDTH - TextWitdh)/2,
    							PARAM_HEIGHT - ((PARAM_VAL_HEIGHT + pDynLayer->getTextHeight())/2)); // Position the text
    pDynLayer->drawText(m_TabDiscretValues[NumValue].m_ShortValue.c_str()); // Render the parameter value


    uint8_t NbDiscretValues = m_TabDiscretValues.size();
    if(NbDiscretValues != 0){
        float IncAlpha = Deg2Rad(static_cast<float>(PARAM_POT_ALPHA) / static_cast<float>(NbDiscretValues+1));
		float Alpha = Deg2Rad(240.0f) - (IncAlpha);
    	for(uint8_t i = 0; i < NbDiscretValues; i++){
			float X = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * std::cos(Alpha);
			float Y = static_cast<float>(PARAM_DISCRET_POT_RADIUS) * std::sin(Alpha);
			pDynLayer->drawFillCircle((PARAM_WIDTH/2) + X, PARAM_HEIGHT/2  - Y, PARAM_DISCRET_RADIUS+1, NumValue == i ? LAYER_POT_INDEX_COLOR : BACKCOLOR);
			pDynLayer->drawCircle((PARAM_WIDTH/2) + X, PARAM_HEIGHT/2 - Y, PARAM_DISCRET_RADIUS+1, LAYER_POT_COLOR);
			Alpha -= IncAlpha;
		}
    }
}

// --------------------------------------------------------------------------
// Draw Static Form view

void cParameterDiscretView::drawStatMainView(DadGFX::cLayer* pStatLayer){
    // Erase previous layer content and draw the new parameter's name
    if(m_TabDiscretValues.size() == 0) return;
	pStatLayer->eraseLayer(MAIN_BACK_COLOR);
	pStatLayer->setFont(cPendaUI::m_pFont_L);
    uint16_t NameWidth = pStatLayer->getTextWidth(m_LongName.c_str());
    pStatLayer->setCursor((MAIN_WIDTH - NameWidth)/2, NAME_OFFSET);
    pStatLayer->drawText(m_LongName.c_str());
    m_NameHeight = pStatLayer->getTextHeight() + NAME_OFFSET;
}

// --------------------------------------------------------------------------
// Draw Dynamic Form view
void cParameterDiscretView::drawDynMainView(DadGFX::cLayer* pDynLayer){
    // Erase previous content from the dynamic layer and draw the updated value
    if(m_TabDiscretValues.size() == 0) return;
	pDynLayer->eraseLayer();
	pDynLayer->setFont(cPendaUI::m_pFont_XL);
    uint8_t NumValue = (uint8_t) m_pParameter->getTargetValue();

    uint16_t TextWitdh = pDynLayer->getTextWidth(m_TabDiscretValues[NumValue].m_LongValue.c_str());
    pDynLayer->setCursor((MAIN_WIDTH - TextWitdh)/2, m_NameHeight + 1);
    pDynLayer->drawText(m_TabDiscretValues[NumValue].m_LongValue.c_str());

}

} // DadUI
