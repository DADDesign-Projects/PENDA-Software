#pragma once
//====================================================================================
// UIParameter.h
//  
// Management of a parameter-based graphical interface.
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//
//====================================================================================
#include <cstring>
#include <vector>
#include <cmath>
#include <cstdint>
#include "main.h"
#include "cDisplay.h"
#include "QSPI.h"
#include "GUI.h"

//***********************************************************************************
// Global variables
//***********************************************************************************
extern DadGFX::cDisplay __Display; // Display object for graphical rendering
extern DadQSPI::cQSPI_FlasherStorage __QSPI_FlasherStorage; // QSPI object for accessing files in memory


namespace DadUI{

//***********************************************************************************
// Definitions
//***********************************************************************************
// PI
#define __PI 3.14159265358979
// Screen dimensions
#define SCREEN_WITDH    320
#define SCREEN_HEIGHT   240

// Font definitions, retrieving font data from QSPI storage
#define FONT09 (DadGFX::GFXBinFont *) __QSPI_FlasherStorage.GetFilePtr("Arial_9p.bin")
#define FONT12 (DadGFX::GFXBinFont *) __QSPI_FlasherStorage.GetFilePtr("Arial_12p.bin")
#define FONT20 (DadGFX::GFXBinFont *) __QSPI_FlasherStorage.GetFilePtr("Arial_20p.bin")
#define FONT36 (DadGFX::GFXBinFont *) __QSPI_FlasherStorage.GetFilePtr("Arial_36p.bin")

// UI layout parameters
#define MENU_HEIGHT 22
#define MENU_NB_ITEM 4
#define MENU_ITEM_WitDH SCREEN_WITDH / MENU_NB_ITEM

#define PARAM_NB_ITEM 3
#define PARAM_DYN_LAYER_WITDH SCREEN_WITDH/PARAM_NB_ITEM
#define PARAM_DYN_LAYER_HEIGHT 85
#define PARAM_STAT_LAYER_WITDH SCREEN_WITDH/PARAM_NB_ITEM
#define PARAM_STAT_LAYER_HEIGHT 90
#define PARAM_LAYER_MAIN_WITDH SCREEN_WITDH
#define PARAM_STAT_DYN_OFFSET 30
#define PARAM_LAYER_MAIN_HEIGHT SCREEN_HEIGHT-PARAM_DYN_LAYER_HEIGHT-PARAM_STAT_DYN_OFFSET-MENU_HEIGHT

// Parameters for potentiometer graphical representation
#define PARAM_POT_RADIUS 28
#define PARAM_POT_ALPHA_MIN 30
#define PARAM_POT_ALPHA_MAX 360-PARAM_POT_ALPHA_MIN
#define PARAM_POT_ALPHA PARAM_POT_ALPHA_MAX - PARAM_POT_ALPHA_MIN

// Parameters for potentiometer discret graphical representation
#define PARAM_DISCRET_RADIUS 5
#define PARAM_DISCRET_POT_RADIUS 26


// Timing constants
#define TIME_FOCUS_MAIN 15;

// Layer colors
#define LAYER_PARAMETER_NAME_COLOR DadGFX::sColor(255,255,255,255)
#define LAYER_PARAMETER_VALUE_COLOR DadGFX::sColor(255,255,255,255)
#define LAYER_PARAMETER_MAIN_COLOR DadGFX::sColor(255,255,255,255)
#define LAYER_POT_COLOR DadGFX::sColor(255,255,255,255)
#define LAYER_POT_INDEX_COLOR DadGFX::sColor(255,100,50,255)
#define MENU_ACTIVE_ITEM_COLOR DadGFX::sColor(110,110,120,255)
#define MENU_SELETED_ITEM_COLOR DadGFX::sColor(100,100,200,255)
#define MENU_BACK_COLOR  DadGFX::sColor(40,40,40,255)


// Declare graphical layers for the interface
DECLARE_LAYER(ParamDyn1, PARAM_DYN_LAYER_WITDH, PARAM_DYN_LAYER_HEIGHT)
DECLARE_LAYER(ParamDyn2, PARAM_DYN_LAYER_WITDH, PARAM_DYN_LAYER_HEIGHT)
DECLARE_LAYER(ParamDyn3, PARAM_DYN_LAYER_WITDH, PARAM_DYN_LAYER_HEIGHT)

DECLARE_LAYER(ParamStat1, PARAM_STAT_LAYER_WITDH, PARAM_STAT_LAYER_HEIGHT)
DECLARE_LAYER(ParamStat2, PARAM_STAT_LAYER_WITDH, PARAM_STAT_LAYER_HEIGHT)
DECLARE_LAYER(ParamStat3, PARAM_STAT_LAYER_WITDH, PARAM_STAT_LAYER_HEIGHT)

DECLARE_LAYER(ParamStatMain, PARAM_LAYER_MAIN_WITDH, PARAM_LAYER_MAIN_HEIGHT)
DECLARE_LAYER(ParamDynMain, PARAM_LAYER_MAIN_WITDH, PARAM_LAYER_MAIN_HEIGHT)

DECLARE_LAYER(ParamMainUIDyn, SCREEN_WITDH, MENU_HEIGHT)
DECLARE_LAYER(ParamMainUIStat, SCREEN_WITDH, MENU_HEIGHT)

DECLARE_LAYER(SaveRestoreUIDyn, SCREEN_WITDH, SCREEN_HEIGHT-MENU_HEIGHT)
DECLARE_LAYER(SaveRestoreUIStat, SCREEN_WITDH, SCREEN_HEIGHT-MENU_HEIGHT)

//***********************************************************************************
// class cParameterSmallView
//***********************************************************************************

// Class for displaying and managing a small parameter view
class cParameterSmallView {
public:
    // --------------------------------------------------------------------------
    // Constructor and destructor
    cParameterSmallView(){}
    ~cParameterSmallView(){}

    // --------------------------------------------------------------------------    
    // Initialize the parameter view with associated layers and parameter
    void Init(cParameter* pParameter, DadGFX::cLayer* pStaticLayer, DadGFX::cLayer* pDynamicLayer);

    // --------------------------------------------------------------------------
    // Refresh the dynamic layer based on the parameter's type of view
    void Refresh();

    // --------------------------------------------------------------------------
    // Activate the parameter view, adjusting Z-order and rendering layers
    void Activate();

    // --------------------------------------------------------------------------
    // Deactivate the parameter view, resetting Z-order
    void DeActivate();

    // --------------------------------------------------------------------------
    // DrawStaticLayer: Renders the static portion of the parameter view.
    // This includes the parameter name and fixed graphical elements.
    void DrawStaticLayer();
    
    // --------------------------------------------------------------------------
    // DrawDiscretStaticLayer: Renders the static portion of the parameter view.
    // This includes the parameter name and fixed graphical elements.
    void DrawDiscretStaticLayer();
    
    // --------------------------------------------------------------------------
    // DrawDynamicLayer: Renders the dynamic portion of the parameter view.
    // This includes the current parameter value and the graphical representation of its value.
    void DrawDynamicLayer();

    // --------------------------------------------------------------------------
    // DrawLefRightDynamicLayer: Renders a left-right dynamic visualization for the parameter.
    // This includes the parameter value as text and its graphical representation in a circular range.
    void DrawLefRightDynamicLayer();

    // --------------------------------------------------------------------------
    void DrawDiscretDynamicLayer();

    // -------------------------------------------------------------------------- 
    // isInit: Checks if the ParameterSmallView has been initialized.
    // Returns true if a valid parameter is associated with this view.
    inline bool isInit(){
        return m_pParameter != nullptr;
    }

    // -------------------------------------------------------------------------- 
    // getParameter: Retrieves the parameter associated with this view.
    // Returns a pointer to the current cParameter instance.
    inline cParameter* getParameter(){
        return m_pParameter;
    }

    protected:
        // -------------------------------------------------------------------------- 
        // Member variables

        cParameter*     m_pParameter = nullptr; // Pointer to the parameter being displayed
        DadGFX::cLayer* m_pStaticLayer = nullptr; // Static layer for persistent elements
        DadGFX::cLayer* m_pDynamicLayer = nullptr; // Dynamic layer for real-time updates
};


//***********************************************************************************
// class cUIParameter
//***********************************************************************************
class cUIParameter : public cUI {
public: 
    // -------------------------------------------------------------------------- 
    // Constructor
    cUIParameter(){}

    // -------------------------------------------------------------------------- 
    // Destructor: Ensures proper cleanup of dynamically allocated fonts.
    ~cUIParameter();

    // -------------------------------------------------------------------------- 
    // InitParameters: Initializes the UI with parameters and their layers.
    // Parameter1, Parameter2, Parameter3: pointers to parameters.
    void InitParameters(cParameter* Parameter1, cParameter* Parameter2, cParameter* Parameter3);

    // -------------------------------------------------------------------------- 
    // ActivateMain: Activates the main view for the given parameter by bringing its 
    // layers to the front and displaying the parameter's long name.
    void ActivateMain(cParameter* pParameter);

    // -------------------------------------------------------------------------- 
    // RefreshMain: Refreshes the main view for the given parameter by erasing the 
    // dynamic layer and displaying the updated parameter value.
    void RefreshMain(cParameter* pParameter);

    // -------------------------------------------------------------------------- 
    // DeActivateMain: Deactivates the main view by resetting the Z-order of the layers.
    void DeActivateMain();

    // -------------------------------------------------------------------------- 
    // Activate: Activates the small parameter views if they are initialized.
    virtual void Activate();

    // -------------------------------------------------------------------------- 
    // DeActivate: Deactivates the small parameter views if they are initialized.
    virtual void DeActivate();
    
    // -------------------------------------------------------------------------- 
    // Proceed: Handles user interaction by updating parameters based on encoder inputs
    // and refreshing the views accordingly.
    virtual void Proceed();
    
protected:
    // -------------------------------------------------------------------------- 
    // Member variables
    DadGFX::cLayer*      m_pParamStatMain=nullptr;  
    DadGFX::cLayer*      m_pParamDynMain=nullptr; 

    // Small parameter views for each parameter
    cParameterSmallView  m_ParameterSmallView1;
    cParameterSmallView  m_ParameterSmallView2;
    cParameterSmallView  m_ParameterSmallView3;

    // Font objects for text rendering
    DadGFX::cFont*      m_pFont1=nullptr;
    DadGFX::cFont*      m_pFont2=nullptr;
    DadGFX::cFont*      m_pFont3=nullptr;

    // Parameter focus and activation state
    uint8_t            m_ParamFocusMain=0;
    uint16_t           m_CtMainActivate=0;
};

//***********************************************************************************
// class cUISaveRestore
//***********************************************************************************
class cUISaveRestore : public cUI {
public: 
    // -------------------------------------------------------------------------- 
    // Constructor
    cUISaveRestore(){}

    // -------------------------------------------------------------------------- 
    // Destructor: Ensures proper cleanup of dynamically allocated fonts.
    ~cUISaveRestore();

    // --------------------------------------------------------------------------    
    // Initialize 
    static void InitSaveRestore();

    // -------------------------------------------------------------------------- 
    // AddParameters :
    static void AddSaveParameters(cParameter* Parameter){
        m_TabParameter.push_back(Parameter);
    }

    // -------------------------------------------------------------------------- 
    // Activate:
    virtual void Activate() override;

    // -------------------------------------------------------------------------- 
    // DeActivate:
    virtual void DeActivate();


protected:
    // -------------------------------------------------------------------------- 
    // Member variables
    static DadGFX::cLayer*      m_pSaveStatMain;  
    static DadGFX::cLayer*      m_pSaveDynMain; 

    // Font objects for text rendering
    static DadGFX::cFont*      m_pFont1;
    static DadGFX::cFont*      m_pFont2;

    static Dad::cSerialize                  m_Serializer;
    static std::vector<DadUI::cParameter *> m_TabParameter;

    std::int32_t m_MemorySlot = 0;
};

//***********************************************************************************
// class cUISave
//***********************************************************************************
class cUISave : public cUISaveRestore {
    public:
    cUISave(){};

    // -------------------------------------------------------------------------- 
    // Activate:
    virtual void Activate() override;
    
    // -------------------------------------------------------------------------- 
    // Proceed: Handles user interaction by updating parameters based on encoder inputs
    // and refreshing the views accordingly.
    virtual void Proceed() override ;
};

//***********************************************************************************
// class cUISave
//***********************************************************************************
class cUIRestore : public cUISaveRestore{
    public:
    cUIRestore(){};

    // -------------------------------------------------------------------------- 
    // Activate: Activates the small parameter views if they are initialized.
    virtual void Activate() override;
    
    // -------------------------------------------------------------------------- 
    // Proceed: Handles user interaction by updating parameters based on encoder inputs
    // and refreshing the views accordingly.
    virtual void Proceed() override;
};

//***********************************************************************************
// class cUIMainParameter
// This class manages a tabbed UI interface for parameter settings with both static 
// and dynamic display layers.
//***********************************************************************************
class cUIMainParameter : public cUI {
public: 
    // -------------------------------------------------------------------------- 
    // Constructor: Creates a new instance of the UI parameter manager
    cUIMainParameter(){}

    // -------------------------------------------------------------------------- 
    // Destructor: Ensures proper cleanup of dynamically allocated fonts
    // Prevents memory leaks by deleting the font object if it exists
    ~cUIMainParameter(){
        if (m_pFont != nullptr) {
            delete m_pFont;
            m_pFont = nullptr;
        }
    }

    // -------------------------------------------------------------------------- 
    // InitMainParameter: Initializes the UI components including fonts and layers
    // Sets up both static and dynamic layers with appropriate properties
    static void InitMainParameter();
    
    // --------------------------------------------------------------------------     
    // Determine if UI is Main UI (top menu manager)
    virtual bool isMain(){
        return true;
    };
    // -------------------------------------------------------------------------- 
    // Activate: Prepares and displays the UI interface
    // - Arranges layers in correct Z-order
    // - Draws parameter names in the static layer
    // - Initializes selection state if parameters exist
    virtual void Activate();

    // -------------------------------------------------------------------------- 
    // DeActivate: Cleans up the UI interface when it's no longer active
    // - Resets layer Z-order
    // - Deactivates active parameter if one exists
    virtual void DeActivate();

    // -------------------------------------------------------------------------- 
    // Proceed: Handles UI updates and user interaction
    // - Processes encoder input for parameter selection
    // - Manages parameter activation/deactivation
    // - Updates visual feedback
    virtual void Proceed();

protected:
    // Current state tracking
    int8_t m_ActiveParameter=-1;     // Currently active parameter tab
    int8_t m_SelectedParameter=-1;   // Currently selected parameter tab
    
    // Display layers
    static DadGFX::cLayer* m_pStaticLayer;   // Static layer for persistent elements
    static DadGFX::cLayer* m_pDynamicLayer;  // Dynamic layer for real-time updates
    
    // Font resources
    static DadGFX::cFont* m_pFont; // Font used for parameter names
};
}//DadUI
