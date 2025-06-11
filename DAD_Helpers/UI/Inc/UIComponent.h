#pragma once
//====================================================================================
// UIComponent.h
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "main.h"
#include "PendaUI.h"
#include "Parameter.h"
#include "cDisplay.h"
#include "UIDefines.h"

namespace DadUI {

//***********************************************************************************
// Class: cUIParameters
// Description: Manages UI parameters and handles focus interactions.
//              This class is responsible for displaying and updating parameter views
//              based on user input and focus changes.
//***********************************************************************************
#define TIME_FOCUS_MAIN 10 // Time duration (in updates) to maintain focus on the main view

class cUIParameters : public iGUIObject {
public:
    virtual ~cUIParameters() {}

    // --------------------------------------------------------------------------
    // Function: Init
    // Description: Initializes the parameter views and deactivates the component.
    // Parameters:
    //   - paramView1: Pointer to the first parameter view.
    //   - paramView2: Pointer to the second parameter view.
    //   - paramView3: Pointer to the third parameter view.
    void Init(cParameterView* paramView1, cParameterView* paramView2, cParameterView* paramView3);

    // --------------------------------------------------------------------------
    // Function: Activate
    // Description: Activates the UI component by bringing its layers to the foreground
    //              and drawing the parameter views.
    void Activate() override;

    // --------------------------------------------------------------------------
    // Function: DeActivate
    // Description: Deactivates the UI component by releasing focus and resetting
    //              the Z-order of its layers.
    void DeActivate() override;

    // --------------------------------------------------------------------------
    // Function: Update
    // Description: Updates the UI component logic based on user input and focus changes.
    void Update() override;

    // --------------------------------------------------------------------------
    // Function: ReDraw
    // Description: Force to draw the dynamic view
    void ReDraw()override;

    // --------------------------------------------------------------------------
    // Function: OnMainFocusLost
    // Description: Called when the main focus is lost. Resets the Z-order of the main layers.
    void OnMainFocusLost() override;

    // --------------------------------------------------------------------------
    // Function: OnMainFocusGained
    // Description: Called when the main focus is gained. Updates the main view with the focused parameter.
    void OnMainFocusGained() override;

    void RTProcess()override {}

protected:
    // --------------------------------------------------------------------------
    // Member variables

    cParameterView* m_parameterViews[NB_PARAM_ITEM];		// Array of parameter views (up to NB_PARAM_ITEM)
    bool 			m_isActive; 							// Indicates if the component is active
    uint8_t 		m_currentFocus; 						// Currently focused parameter index (1, 2, 3, or 0 if none)
    uint16_t 		m_focusTimer; 							// Timer to maintain focus on the main view
};

//***********************************************************************************
// Class: cUIMemory
// Description: This class manages the UI for memory operations such as saving, loading,
//              and deleting memory slots. It inherits from iGUIObject and handles
//              user interactions via encoders and buttons.
//***********************************************************************************

#define MIDI_OFF		 52		// CC#52 Off
#define MIDI_ON		 	 51		// CC#51 On
#define MIDI_ON_OFF		 50		// CC#50 On/Off
#define MIDI_PRESET_UP 	 49		// CC#49 Preset Up
#define MIDI_PRESET_DOWN 48		// CC#48 Preset Down

// Enumeration for memory operation states
enum class eMemState {
    Save,   // State for saving data to a memory slot
    Load,   // State for loading data from a memory slot
    Delete  // State for deleting data from a memory slot
};

// Enumeration for user confirmation choices
enum class eMemChoice {
    No = 0, // User selects "No"
    Yes = 1 // User selects "Yes"
};

// Class definition for cUIMemory
class cUIMemory : public iGUIObject {
public:
    virtual ~cUIMemory() {}

    // --------------------------------------------------------------------------
    // Function: Init
    // Description: Initializes the memory UI. Sets default values for member variables,
    //              restores the active memory slot, and requests UI focus.
    void Init(uint32_t SerializeID);

    // --------------------------------------------------------------------------
    // Function: Activate
    // Description: Activates the memory UI menu. Changes the Z-order of UI layers
    //              to bring the menu to the foreground and redraws the UI.
    void Activate() override;

    // --------------------------------------------------------------------------
    // Function: DeActivate
    // Description: Deactivates the memory UI menu. Resets the Z-order of UI layers
    //              and updates the memory slot display.
    void DeActivate() override;

    // --------------------------------------------------------------------------
    // Function: Update
    // Description: Handles user input and updates the UI state based on encoder
    //              movements and button presses.
    void Update() override;

    // --------------------------------------------------------------------------
    // Function: OnMainFocusLost
    // Description: Called when the UI loses focus. Resets the Z-order of UI layers.
    void OnMainFocusLost() override;

    // --------------------------------------------------------------------------
    // Function: OnMainFocusGained
    // Description: Called when the UI gains focus. Updates the Z-order of UI layers
    //              and redraws the UI.
    void OnMainFocusGained() override;

    // --------------------------------------------------------------------------
    // Function call when this CC MIDI_PRESET_UP is received
    static void MIDI_PresetUp_CallBack(uint8_t control, uint8_t value, uint32_t userData);

    // --------------------------------------------------------------------------
    // Function call when this CC MIDI_PRESET_DOWN is received
    static void MIDI_PresetDown_CallBack(uint8_t control, uint8_t value, uint32_t userData);

    // --------------------------------------------------------------------------
    // Function call when this CC MIDI_ON_OFF is received
    static void MIDI_OnOff_CallBack(uint8_t control, uint8_t value, uint32_t userData);

    // --------------------------------------------------------------------------
    // Function call when this PC MIDI is received
    static void MIDI_ProgramChange_CallBack(uint8_t program, uint32_t userData);

protected:
    // --------------------------------------------------------------------------
    // Function: isLoadable
    // Description: Checks if a memory slot is loadable.
    // Parameters: memSlot - The memory slot to check.
    // Returns: 1 if the slot is loadable, 0 otherwise.
    inline uint8_t isLoadable(uint8_t memSlot) {
        return m_Memory.isSave(memSlot);
    }

    // --------------------------------------------------------------------------
    // Function: isErasable
    // Description: Checks if a memory slot is erasable.
    // Parameters: memSlot - The memory slot to check.
    // Returns: 1 if the slot is erasable, 0 otherwise.
    inline uint8_t isErasable(uint8_t memSlot) {
        if ((memSlot != m_Memory.getActiveSlot()) && (1 == m_Memory.isSave(memSlot))) {
            return 1;
        } else {
            return 0;
        }
    }

    // --------------------------------------------------------------------------
    // Function: IncrementSlot
    void IncrementSlot(int8_t Increment);

    // --------------------------------------------------------------------------
    // Function: OnOff
    void OnOff();

    // --------------------------------------------------------------------------
    // Function: drawMainUpStat
    // Description: Draws static UI elements for the memory menu.
    //              This includes the text indicating the current state (Save, Delete, Load) and the "Yes" and "No" buttons.
    void drawMainUpStat();

    // --------------------------------------------------------------------------
    // Function: drawMainUpDyn
    // Description: Draws dynamic UI elements for the memory menu based on the current state.
    //              This includes highlighting the selected choice (Yes or No) if applicable.
    void drawMainUpDyn();

    // --------------------------------------------------------------------------
    // Function: drawMainDownStat
    // Description: Draws static UI elements for the bottom layer of the memory menu.
    //              This includes the label "MEM : " to indicate the memory section.
    void drawMainDownStat();

    // --------------------------------------------------------------------------
    // Function: drawMainDownDyn
    // Description: Draws dynamic UI elements for the bottom layer of the memory menu.
    //              This includes the current memory slot number and a "Modified" indicator if applicable.
    void drawMainDownDyn();

    // --------------------------------------------------------------------------
    // Function: SaveSlot
    // Description: Saves the current state to the selected memory slot.
    void SaveSlot();

    // --------------------------------------------------------------------------
    // Function: RestoreSlot
    // Description: Restores the state from the selected memory slot.
    void RestoreSlot();



protected:
	// --------------------------------------------------------------------------
	// Member variables
	bool				m_isActive;     // Indicates if the UI is active
	int16_t 			m_MemorySlot;  	// Current memory slot
	eMemChoice 			m_MemChoice; 	// User's choice (Yes/No)
	eMemState 			m_MemState;   	// Current memory operation state
	uint8_t				m_ActionExec;	// Indicates if an action has been executed
	uint64_t			m_PressCount;
	uint32_t    		m_SerializeID;  // Serialization ID for saving/restoring state

	DadQSPI::cMemory 	m_Memory;		// Slot memory manager
 };

//***********************************************************************************
// Class: cTapTempo
// Description: This class handles the Tap Tempo functionality, updating a parameter
//              based on the timing of a footswitch press. It also manages UI focus
//              and refreshes the display accordingly.
//***********************************************************************************
enum class eTempoType {
	period,
	frequency
};
class cTapTempo : public iGUIObject {
public:
    virtual ~cTapTempo() {}

    // --------------------------------------------------------------------------
    // Function: Init
    // Description: Initializes the Tap Tempo object with a footswitch and a parameter view.
    //
    // Parameters:
    //   pFootSwitch     - Pointer to the switch used for tap tempo detection.
    //   pParameterView  - Pointer to the UI parameter view that will be updated.
    //
    void Init(cSwitch* pFootSwitch, cParameterView* pParameterView, eTempoType TempoType = eTempoType::period);

    // --------------------------------------------------------------------------
    // Function: Update
    // Description: Periodically updates the tap tempo parameter and manages UI focus.
    //
    void Update() override ;
    // --------------------------------------------------------------------------
    // Function: OnMainFocusLost
    // Description: Called when the UI focus is lost, adjusting layer visibility.
    //
    void OnMainFocusLost() override;

    // --------------------------------------------------------------------------
    // Function: OnMainFocusGained
    // Description: Called when the UI gains focus, bringing relevant layers to the foreground.
    //
    void OnMainFocusGained() override;

protected:

    // --------------------------------------------------------------------------
    // Member variables
    uint32_t        m_PeriodUpdateCount;  	 // Tracks the number of period updates
    cSwitch*        m_pFootSwitch;           // Pointer to the footswitch input
    uint32_t        m_focusTimer;            // Timer to control UI focus duration
    eTempoType		m_TempoType;			 // Type off tempo result
    cParameterView* m_pParameterView;        // Pointer to the parameter view for UI updates
};

//***********************************************************************************
// Class: cUIMenu
// Description: Handles the graphical menu and navigation logic.
//              This class manages the display and interaction with a menu composed of items.
//***********************************************************************************

//-----------------------------------------------------------------------------------
// Struct: MenuItem
// Represents an individual menu item with a name and an associated GUI object.
struct MenuItem {
    std::string Name;  // Name of the menu item
    iGUIObject* pItem; // Pointer to the GUI object associated with this item
};

// Class definition for cUIMenu
class cUIMenu : public iGUIObject {
public:
    virtual ~cUIMenu() {}

    // --------------------------------------------------------------------------
    // Function: Init
    // Description: Initializes the menu by clearing existing items and deactivating the menu.
    void Init();

    // --------------------------------------------------------------------------
    // Function: addMenuItem
    // Description: Adds a new menu item to the list.
    // Parameters:
    //   - pItem: Pointer to the GUI object associated with the menu item.
    //   - Name: The name of the menu item to display.
    void addMenuItem(iGUIObject* pItem, const std::string& Name);

    // --------------------------------------------------------------------------
    // Function: Activate
    // Description: Activates the menu and the currently selected item.
    //              Brings the menu layers to the foreground and draws the menu.
    void Activate() override;

    // --------------------------------------------------------------------------
    // Function: DeActivate
    // Description: Deactivates the menu and the currently selected item.
    //              Resets the Z-order of the menu layers and marks the menu as inactive.
    void DeActivate() override;

    // --------------------------------------------------------------------------
    // Function: Update
    // Description: Updates the menu logic based on user input.
    //              Handles navigation through the menu items using encoder input.
    void Update() override ;

    // --------------------------------------------------------------------------
    // Function: drawTab
    // Description: Draws the menu tabs, including the active item and navigation indicators.
    void drawTab();

protected:
    // --------------------------------------------------------------------------
    // Function: drawArrowIndicator
    // Description: Draws an arrow indicator on the left or right side of the menu.
    // Parameters:
    //   - isLeft: If true, draw a left arrow; otherwise, draw a right arrow.
    void drawArrowIndicator(bool isLeft);

    // --------------------------------------------------------------------------
    // Member variables
    std::vector<MenuItem> m_TabMenuItem; // List of menu items
    int8_t m_ActiveItem;                 // Index of the currently active menu item
    int8_t m_MenuShift;                  // Index for scrolling the menu
    uint8_t m_isActive;                  // Indicates if the menu is active
};
} // namespace DadUI
