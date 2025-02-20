#pragma once
//====================================================================================
// GUI
//
// Management of PENDA Graphical User Interface 
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include <cstring>
#include <vector>
#include <cstdint>
#include "main.h"
#include "cDisplay.h"
#include "QSPI.h"
#include "Serialize.h"
#include "cEncoder.h"

//***********************************************************************************
// Global variables
extern DadGFX::cDisplay __Display;
extern DadQSPI::cQSPI_FlasherStorage __FlashStorage;

namespace DadUI {

//***********************************************************************************
// Defines
//-----------------------------------------------------------------------------------

// Font definitions, retrieving font data from QSPI storage
#define FONTS (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Arial_9p.bin")
#define FONTM (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Arial_12p.bin")
#define FONTL (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Arial_20p.bin")
#define FONTXL (DadGFX::GFXBinFont *) __FlashStorage.GetFilePtr("Arial_36p.bin")


//***********************************************************************************
// Enum
enum class PARAM_TYPE_VIEW{
    Normal,
    LeftRight,
    Discret
};
//***********************************************************************************
// sDiscretValues
struct sDiscretValues{
    std::string m_ShortValue;
    std::string m_LongValue;
};


//***********************************************************************************
// class cParameter
// This class represents a parameter with a value constrained within a specified range.
// It provides functionality to initialize, modify, and retrieve the value with bounds checking.
// Additionally, the class supports rapid and slow increments, as well as normalized value handling
// for ease of scaling within the range.
//***********************************************************************************
class cParameter {
public:
    // --------------------------------------------------------------------------
    // Constructor and destructor
    cParameter(){} // Default constructor
    ~cParameter() {} // Destructor

    // --------------------------------------------------------------------------
    // Initialize the parameter with given attributes
    void Init(const std::string& ShortName, const std::string& LongName, float InitValue, float Min, float Max, float RapidIncrement, float SlowIncrement, PARAM_TYPE_VIEW TypeView = PARAM_TYPE_VIEW::Normal);

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
    inline void setValue(float value) {
        if(value > m_Max) {
            m_Value = m_Max;
        } else if(value < m_Min){
            m_Value= m_Min;
        } else {
            m_Value = value;
        }
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
    // Get the short name of the parameter
    inline const std::string& getShortName() const {
        return m_ShortName;
    }
      
    // --------------------------------------------------------------------------
    // Get the long name of the parameter
    inline const std::string& getLongName() const {
        return m_LongName;
    }
    
    // --------------------------------------------------------------------------
    // Get the parameter value as a formatted string
    virtual void getStringValue(std::string& strValue) const;

    // --------------------------------------------------------------------------
    // Get the normalized value of the parameter (value scaled between 0 and 1)
    inline float getNormalizedValue() const {
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
        m_Value = m_Min + normalizedValue * (m_Max - m_Min);
    }

    // --------------------------------------------------------------------------
    // Return type of view
    inline PARAM_TYPE_VIEW getTypeView() const{
        return m_TypeView;
    }
    
    // --------------------------------------------------------------------------
    //
    inline uint8_t getDiscretValuesSize() const{
        return static_cast<uint8_t>(m_TabDiscretValues.size());
    }

    // --------------------------------------------------------------------------
    //
    inline const char*getDiscretShortValue(uint8_t NumValue) const {
        return m_TabDiscretValues[NumValue].m_ShortValue.c_str();
    } 

    // --------------------------------------------------------------------------
    //
    inline const char*getDiscretLongValue(uint8_t NumValue) const {
        return m_TabDiscretValues[NumValue].m_LongValue.c_str();
    } 

    // --------------------------------------------------------------------------
    //
    inline void getDiscretShortValue(std::string& strValue) const {
        strValue = m_TabDiscretValues[m_Value].m_ShortValue;
    } 

    // --------------------------------------------------------------------------
    //
    inline void getDiscretLongValue(std::string& strValue) const {
        strValue = m_TabDiscretValues[m_Value].m_LongValue;
    } 
    // --------------------------------------------------------------------------
    //
    void addDiscretValue(const char * ShortValue, const char * LongValue);

    // --------------------------------------------------------------------------
    // Serialize the parameter to a string
    inline void Save(Dad::cSerialize &Serializer) const{
        Serializer.Push(m_Value);
    }

    // --------------------------------------------------------------------------
    // Deserialize the parameter from a string
    inline void Restore(Dad::cSerialize &Serializer){
        Serializer.Pull(m_Value);
    }


protected:
    // --------------------------------------------------------------------------
    // Member variable

    std::string m_ShortName;        // Parameter short name
    std::string m_LongName;         // Parameter long name

    float m_Min = 0.9;              // Minimum value
    float m_Max = 1.0;              // Maximum value
    float m_RapidIncrement = 0.1;   // Increment step size (rapid)
    float m_SlowIncrement = 0.01;   // Increment step size (slow)
    float m_Value = 0;              // Current value
    
    std::vector<sDiscretValues> m_TabDiscretValues;
    
    PARAM_TYPE_VIEW m_TypeView;     // Type of view
};

//***********************************************************************************
// class cUI
//***********************************************************************************
class cUI; // Forward declaration to allow its use in sUIChild.

struct sUIChild {
    std::string m_Name;   // Name of the child component.
    uint8_t     m_Order;  // Order or priority of this child component.
    cUI*        m_pChild; // Pointer to a child component of type cUI.
};

class cUI {
public:
    // --------------------------------------------------------------------------
    // Constructor: Initializes member variables
    cUI() {
        m_pParent = nullptr; // No parent initially.
        m_Order = 0;         // Default order is zero.
    }

    // --------------------------------------------------------------------------
    // Destructor: Cleans up resources (if necessary).
    ~cUI() {}

    // --------------------------------------------------------------------------
    // Static initialization method for the UI system (implementation missing).
    static void Init();

    // --------------------------------------------------------------------------
    // Static method to start the UI system with a specified active UI component.
    static void Start(cUI* pActiveUI) {
        m_pActiveUI = pActiveUI; // Set the active UI component.
        m_pActiveUI->Activate(); // Activate the specified UI component.
    }

    // --------------------------------------------------------------------------
    // Method to add a child component to the current UI component.
    void addChild(cUI* pChild, const char* Name, uint8_t Order);

    // --------------------------------------------------------------------------
    // Static method to debounce all encoders and update their increments.
    static void Debounce() {
        m_Encoder0.Debounce();
        m_Encoder0Increment += m_Encoder0.getIncrement();
        
        m_Encoder1.Debounce();
        m_Encoder1Increment += m_Encoder1.getIncrement();
        
        m_Encoder2.Debounce();
        m_Encoder2Increment += m_Encoder2.getIncrement();
        
        m_Encoder3.Debounce();
        m_Encoder3Increment += m_Encoder3.getIncrement();
    }

    // --------------------------------------------------------------------------
    // Static method to refresh the current active UI component.
    static void Refresh() {
        m_pActiveUI->Proceed();
    }

    // --------------------------------------------------------------------------
    // Pure virtual methods to be implemented by derived classes.
    virtual bool isMain(){                          // Determine if UI is Main UI (top menu manager)
        return false;
    }; 
    virtual void Proceed() = 0;                     // Logic to execute during refresh.
    virtual void Activate() = 0;                    // Logic to activate the UI component.
    virtual void DeActivate() = 0;                  // Logic to deactivate the UI component.   

//protected:
    cUI*                        m_pParent;          // Pointer to the parent UI component.
    std::string                 m_Name;             // Name of the UI component.
    uint8_t                     m_Order;            // Order or priority of the component.
    std::vector<sUIChild>       m_pTabChilds;       // List of child components.

    // Shared data across all cUI instances.
    static cUI*                 m_pActiveUI;        // Pointer to the active UI component.

    static cEncoder    		   	m_Encoder0;         // Encoder 0 instance.
    static cEncoder       		m_Encoder1;         // Encoder 1 instance.
    static cEncoder       		m_Encoder2;         // Encoder 2 instance.
    static cEncoder       		m_Encoder3;         // Encoder 3 instance.
    static int32_t        		m_Encoder0Increment;// Increment for Encoder 0.
    static int32_t              m_Encoder1Increment;// Increment for Encoder 1.
    static int32_t              m_Encoder2Increment;// Increment for Encoder 2.
    static int32_t              m_Encoder3Increment;// Increment for Encoder 3.

    static DadGFX::cFont*		m_pFont_S;
    static DadGFX::cFont*		m_pFont_M;
    static DadGFX::cFont*		m_pFont_L;
    static DadGFX::cFont*		m_pFont_XL;

};


} // DadUI
