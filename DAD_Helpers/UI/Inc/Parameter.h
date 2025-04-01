#pragma once
//====================================================================================
// Parameter.h
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include <functional>
#include <cmath>
#include "main.h"
#include "cDisplay.h"
#include "PendaUI.h"
#include "Serialize.h"
// PI
#define __PI 3.14159265358979f
#define Deg2Rad(A) (A) * __PI / 180.0f


namespace DadUI {

//***********************************************************************************
// class cParameter
//***********************************************************************************

// Define the callback function type:
// This callback is called whenever the parameter value is modified.
using CallbackType = std::function<void(float)>;

class cParameter : public iGUIObject{

public:
	virtual ~cParameter(){}

    // --------------------------------------------------------------------------
    // Initialize the parameter with given attributes
    void Init(float InitValue, float Min, float Max,
			  float RapidIncrement, float SlowIncrement,
			  CallbackType Callback = nullptr,
			  float Slope = 0);

    // --------------------------------------------------------------------------
    // Increment the parameter value by a number of steps
    void Increment(int32_t nbStep, bool Switch);

    // --------------------------------------------------------------------------
    // Get the current value of the parameter
    inline float getValue() const {
        return m_Value;
    }

    // --------------------------------------------------------------------------
    // Set the parameter value directly with boundary checks
    void setValue(float value);

    // --------------------------------------------------------------------------
    // Get the target value of the parameter
    inline float getTargetValue() const {
        return m_TargetValue;
    }

    // --------------------------------------------------------------------------
    // Assignment operator overload for direct value assignment
    inline cParameter& operator=(float value) {
        setValue(value);
        return *this;
    }

    // --------------------------------------------------------------------------
    // Implicit conversion to float for ease of use
    inline operator float() const {
        return m_Value;
    }

    // --------------------------------------------------------------------------
    // Get the normalized value of the parameter (value scaled between 0 and 1)
    inline float getNormalizedValue() const {
    	if(m_Max == m_Min) return 0;
        return (m_Value - m_Min) / (m_Max - m_Min);
    }

    // --------------------------------------------------------------------------
    // Set the value of the parameter using a normalized value (between 0 and 1)
    inline void setNormalizedValue(float normalizedValue) {
        if (normalizedValue < 0.0f) {
            normalizedValue = 0.0f; // Clamp to minimum
        } else if (normalizedValue > 1.0f) {
            normalizedValue = 1.0f; // Clamp to maximum
        }
        setValue(m_Min + normalizedValue * (m_Max - m_Min));
    }

    // --------------------------------------------------------------------------
    // Get the normalized value of the parameter (value scaled between 0 and 1)
    inline float getNormalizedTargetValue() const {
    	if(m_Max == m_Min) return 0;
        return (m_TargetValue - m_Min) / (m_Max - m_Min);
    }

    // --------------------------------------------------------------------------
    // Set the Max value of the parameter
    inline void setMaxValue(float MaxValue) {
    	m_Max = MaxValue;
        if(m_Slope == 0){
        	m_Step = (m_Max-m_Min);
        }else{
        	m_Step = (m_Max-m_Min)/m_Slope;
        }
    }

    // --------------------------------------------------------------------------
    // Get the Max value of the parameter
    inline float getMaxValue() {
    	return m_Max;
    }

    // --------------------------------------------------------------------------
    // Set the Min value of the parameter
    inline void setMinValue(float MinValue) {
    	m_Min = MinValue;
        if(m_Slope == 0){
        	m_Step = (m_Max-m_Min);
        }else{
        	m_Step = (m_Max-m_Min)/m_Slope;
        }
    }

    // --------------------------------------------------------------------------
    // Get the Min value of the parameter
    inline float getMinValue() {
    	return m_Min;
    }

    // --------------------------------------------------------------------------
    // Refresh the current value smoothly according to the slope
    void RTProcess()override;

    // --------------------------------------------------------------------------
    // Serialize the parameter to a string
    void Save(DadQSPI::cSerialize &Serializer) override{
        Serializer.Push(m_TargetValue);
    }

    // --------------------------------------------------------------------------
    // Deserialize the parameter from a string
    void Restore(DadQSPI::cSerialize &Serializer)override{
        float Value;
    	Serializer.Pull(Value);
    	setValue(Value);
    }


protected:
    // --------------------------------------------------------------------------
    // Member variable
    //
    float m_Min = 0.0f;             // Minimum value
    float m_Max = 1.0f;             // Maximum value
    float m_RapidIncrement = 0.1f;  // Increment step size (rapid)
    float m_SlowIncrement = 0.01f;  // Increment step size (slow)
    float m_Value = 0.0;           // Current value
    float m_Step;        			// Step change value to 1/m_SamplingRate;
    float m_TargetValue;  			// Target parameter value
    float m_Slope;

    CallbackType m_Callback;        // Callback function
};

//***********************************************************************************
// class cParameterView
//***********************************************************************************
class cParameterView {

public:
	virtual ~cParameterView(){}

	// --------------------------------------------------------------------------
	// Initialize the parameter with given attributes
	void Init(cParameter* pParameter, const std::string& ShortName, const std::string& LongName);

	// --------------------------------------------------------------------------
	// Get the parameter
	cParameter* getParameter(){
		return  m_pParameter;
	}

	// --------------------------------------------------------------------------
	// Draw view
	virtual void drawStatFormView(DadGFX::cLayer* pStatLayer) = 0;
	virtual void drawDynFormView(DadGFX::cLayer* pDynLayer) = 0;
	virtual void drawStatMainView(DadGFX::cLayer* pStatLayer)= 0;
	virtual void drawDynMainView(DadGFX::cLayer* pDynLayer) = 0;

protected :

	// --------------------------------------------------------------------------
	// Member variable
	std::string m_ShortName;        // Parameter short name
	std::string m_LongName;         // Parameter long name
	cParameter* m_pParameter;
	uint16_t	m_NameHeight;
};

//***********************************************************************************
// class cParameterView
//***********************************************************************************
class cParameterNumView : public cParameterView {
public:
	// --------------------------------------------------------------------------
	// Initialize the parameter with given attributes
	void Init(cParameter* pParameter, const std::string& ShortName, const std::string& LongName,
			  const std::string& ShortUnit, const std::string& LongUnit, uint8_t StringPrecision = 3);

protected :
	// --------------------------------------------------------------------------
	// ValueToString: Converts the stored value (m_Value) into a string
	std::string ValueToString() const;

	// --------------------------------------------------------------------------
	// Member variable
	std::string 	m_ShortUnit;			// Parameter short unit
	std::string 	m_LongUnit;				// Parameter long unit
	uint8_t 		m_StringPrecision; 		// size of significant digits precision
};

//***********************************************************************************
// class cParameterNumNormalView
//***********************************************************************************
class cParameterNumNormalView : public cParameterNumView {
public:
	// --------------------------------------------------------------------------
	// Draw view
	void drawStatFormView(DadGFX::cLayer* pStatLayer) override;
	void drawDynFormView(DadGFX::cLayer* pDynLayer) override;
	void drawStatMainView(DadGFX::cLayer* pStatLayer) override;
	void drawDynMainView(DadGFX::cLayer* pDynLayer) override;

};

//***********************************************************************************
// class cParameterNumLeftRightView
//***********************************************************************************
class cParameterNumLeftRightView : public cParameterNumNormalView {
public:
	// --------------------------------------------------------------------------
	// Draw view
	void drawDynFormView(DadGFX::cLayer* pDynLayer) override;
};

//***********************************************************************************
// class cParameterDiscretView
//***********************************************************************************

//***********************************************************************************
// sDiscretValues
struct sDiscretValues{
    std::string m_ShortValue;
    std::string m_LongValue;
};

class cParameterDiscretView : public cParameterView {
public:
	// --------------------------------------------------------------------------
	// Add discrete value
	void AddDiscreteValue(const std::string& ShortDiscretValue, const std::string& LongDiscretValue);

	// --------------------------------------------------------------------------
	// Draw view
	void drawStatFormView(DadGFX::cLayer* pStatLayer) override;
	void drawDynFormView(DadGFX::cLayer* pDynLayer) override;
	void drawStatMainView(DadGFX::cLayer* pStatLayer) override;
	void drawDynMainView(DadGFX::cLayer* pDynLayer) override;


protected :
	// --------------------------------------------------------------------------
	// Member variable

	std::vector<sDiscretValues> m_TabDiscretValues;
};

} // DadUI
