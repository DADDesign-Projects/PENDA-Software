//====================================================================================
// UIComponent.cpp
//
// Copyright (c) 2025 Dad Design. All rights reserved.
//====================================================================================
#include "UIComponent.h"


namespace DadUI {

//***********************************************************************************
// Class: cUIParameters
// Description: Manages UI parameters and handles focus interactions.
//              This class is responsible for displaying and updating parameter views
//              based on user input and focus changes.
//***********************************************************************************

// --------------------------------------------------------------------------
// Function: Init
// Description: Initializes the parameter views and deactivates the component.
// Parameters:
//   - paramView1: Pointer to the first parameter view.
//   - paramView2: Pointer to the second parameter view.
//   - paramView3: Pointer to the third parameter view.
void cUIParameters::Init(cParameterView* paramView1, cParameterView* paramView2, cParameterView* paramView3) {
	m_parameterViews[0] = paramView1; 	// Assign the first parameter view
	m_parameterViews[1] = paramView2; 	// Assign the second parameter view
	m_parameterViews[2] = paramView3; 	// Assign the third parameter view
	DeActivate(); 						// Deactivate the component initially
}

// --------------------------------------------------------------------------
// Function: Activate
// Description: Activates the UI component by bringing its layers to the foreground
//              and drawing the parameter views.
void cUIParameters::Activate(){
	m_isActive = true; 					// Mark the component as active

	// Bring parameter layers to the foreground
	cPendaUI::m_pStatParam1Layer->changeZOrder(10);
	cPendaUI::m_pDynParam1Layer->changeZOrder(11);
	cPendaUI::m_pStatParam2Layer->changeZOrder(10);
	cPendaUI::m_pDynParam2Layer->changeZOrder(11);
	cPendaUI::m_pStatParam3Layer->changeZOrder(10);
	cPendaUI::m_pDynParam3Layer->changeZOrder(11);

	// Draw the first parameter view if it exists, otherwise clear the layers
	if (m_parameterViews[0]) {
		m_parameterViews[0]->drawStatFormView(cPendaUI::m_pStatParam1Layer);
		m_parameterViews[0]->drawDynFormView(cPendaUI::m_pDynParam1Layer);
	} else {
		DadUI::cPendaUI::m_pDynParam1Layer->eraseLayer();
		DadUI::cPendaUI::m_pStatParam1Layer->eraseLayer();
	}

	// Draw the second parameter view if it exists, otherwise clear the layers
	if (m_parameterViews[1]) {
		m_parameterViews[1]->drawStatFormView(cPendaUI::m_pStatParam2Layer);
		m_parameterViews[1]->drawDynFormView(cPendaUI::m_pDynParam2Layer);
	} else {
		DadUI::cPendaUI::m_pDynParam2Layer->eraseLayer();
		DadUI::cPendaUI::m_pStatParam2Layer->eraseLayer();
	}

	// Draw the third parameter view if it exists, otherwise clear the layers
	if (m_parameterViews[2]) {
		m_parameterViews[2]->drawStatFormView(cPendaUI::m_pStatParam3Layer);
		m_parameterViews[2]->drawDynFormView(cPendaUI::m_pDynParam3Layer);
	} else {
		DadUI::cPendaUI::m_pDynParam3Layer->eraseLayer();
		DadUI::cPendaUI::m_pStatParam3Layer->eraseLayer();
	}
}

// --------------------------------------------------------------------------
// Function: DeActivate
// Description: Deactivates the UI component by releasing focus and resetting
//              the Z-order of its layers.
void cUIParameters::DeActivate() {
	if (cPendaUI::HasFocus(this)) {
		cPendaUI::ReleaseFocus(); // Release focus if this component has it
	}

	// Reset the Z-order of parameter layers
	cPendaUI::m_pStatParam1Layer->changeZOrder(0);
	cPendaUI::m_pDynParam1Layer->changeZOrder(0);
	cPendaUI::m_pStatParam2Layer->changeZOrder(0);
	cPendaUI::m_pDynParam2Layer->changeZOrder(0);
	cPendaUI::m_pStatParam3Layer->changeZOrder(0);
	cPendaUI::m_pDynParam3Layer->changeZOrder(0);

	// Reset focus and timer
	m_currentFocus = 0; 	// No parameter is focused
	m_focusTimer = 0; 		// Reset the focus timer
	m_isActive = false; 	// Mark the component as inactive
}

// --------------------------------------------------------------------------
// Function: Update
// Description: Updates the UI component logic based on user input and focus changes.
void cUIParameters::Update(){
	if (!m_isActive) return; // Skip update if the component is inactive

	uint8_t previousFocus = m_currentFocus; // Store the previous focus

	// Check encoder 1 input and update the first parameter view if applicable
	if (m_parameterViews[0] && (cPendaUI::m_Encoder1Increment != 0 || cPendaUI::m_Encoder1.getSwitchState() != 0)) {
		m_parameterViews[0]->getParameter()->Increment(cPendaUI::m_Encoder1Increment, cPendaUI::m_Encoder1.getSwitchState());
		cPendaUI::m_Encoder1Increment = 0; 									// Reset encoder input
		m_parameterViews[0]->drawDynFormView(cPendaUI::m_pDynParam1Layer); 	// Redraw dynamic view
		m_currentFocus = 1; 												// Set focus to the first parameter
		m_focusTimer = TIME_FOCUS_MAIN; 									// Start the focus timer
	}

	// Check encoder 2 input and update the second parameter view if applicable
	if (m_parameterViews[1] && (cPendaUI::m_Encoder2Increment != 0 || cPendaUI::m_Encoder2.getSwitchState() != 0)) {
		m_parameterViews[1]->getParameter()->Increment(cPendaUI::m_Encoder2Increment, cPendaUI::m_Encoder2.getSwitchState());
		cPendaUI::m_Encoder2Increment = 0; 									// Reset encoder input
		m_parameterViews[1]->drawDynFormView(cPendaUI::m_pDynParam2Layer); 	// Redraw dynamic view
		m_currentFocus = 2; 												// Set focus to the second parameter
		m_focusTimer = TIME_FOCUS_MAIN; 									// Start the focus timer
	}

	// Check encoder 3 input and update the third parameter view if applicable
	if (m_parameterViews[2] && (cPendaUI::m_Encoder3Increment != 0 || cPendaUI::m_Encoder3.getSwitchState() != 0)) {
		m_parameterViews[2]->getParameter()->Increment(cPendaUI::m_Encoder3Increment, cPendaUI::m_Encoder3.getSwitchState());
		cPendaUI::m_Encoder3Increment = 0; 									// Reset encoder input
		m_parameterViews[2]->drawDynFormView(cPendaUI::m_pDynParam3Layer); 	// Redraw dynamic view
		m_currentFocus = 3; 												// Set focus to the third parameter
		m_focusTimer = TIME_FOCUS_MAIN; 									// Start the focus timer
	}

	// If focus has changed, request focus and update the main view
	if (((previousFocus == 0) && (m_currentFocus != 0)) || ((m_currentFocus != previousFocus) && (m_currentFocus != 0))) {
		if (cPendaUI::HasFocus(this)) {
			m_parameterViews[m_currentFocus - 1]->drawStatMainView(cPendaUI::m_pStatMainDownLayer); // Redraw main view
		} else {
			cPendaUI::RequestFocus(this); 									// Request focus for this component
		}
	}

	// If focus timer is active, update the dynamic main view
	if (m_focusTimer != 0) {
		m_parameterViews[m_currentFocus - 1]->drawDynMainView(cPendaUI::m_pDynMainDownLayer); // Redraw dynamic main view
		m_focusTimer--; 													// Decrement the focus timer

		// If timer reaches zero, release focus
		if (m_focusTimer == 0) {
			cPendaUI::ReleaseFocus(); 										// Release focus
			m_currentFocus = 0; 											// Reset focus
		}
	}
}

// --------------------------------------------------------------------------
// Function: ReDraw
// Description: Force to draw the dynamic view
void cUIParameters::ReDraw(){
	if (!m_isActive) return;
	if(m_parameterViews[0])	m_parameterViews[0]->drawDynFormView(cPendaUI::m_pDynParam1Layer);
	if(m_parameterViews[1]) m_parameterViews[1]->drawDynFormView(cPendaUI::m_pDynParam2Layer);
	if(m_parameterViews[2]) m_parameterViews[2]->drawDynFormView(cPendaUI::m_pDynParam3Layer);
}

// --------------------------------------------------------------------------
// Function: OnMainFocusLost
// Description: Called when the main focus is lost. Resets the Z-order of the main layers.
void cUIParameters::OnMainFocusLost(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(0); 	// Reset Z-order of static main layer
	cPendaUI::m_pDynMainDownLayer->changeZOrder(0); 	// Reset Z-order of dynamic main layer
	m_currentFocus = 0; 								// Reset focus
	m_focusTimer = 0; 									// Reset focus timer
}

// --------------------------------------------------------------------------
// Function: OnMainFocusGained
// Description: Called when the main focus is gained. Updates the main view with the focused parameter.
void cUIParameters::OnMainFocusGained(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(40); 	// Bring static main layer to the foreground
	cPendaUI::m_pDynMainDownLayer->changeZOrder(41); 	// Bring dynamic main layer to the foreground
	m_parameterViews[m_currentFocus - 1]->drawStatMainView(cPendaUI::m_pStatMainDownLayer); // Redraw main view
}

//***********************************************************************************
// Class: cUIMemory
// Description: This class manages the UI for memory operations such as saving, loading,
//              and deleting memory slots. It inherits from iGUIObject and handles
//              user interactions via encoders and buttons.
//***********************************************************************************

// --------------------------------------------------------------------------
// Function: Init
// Description: Initializes the memory UI. Sets default values for member variables,
//              restores the active memory slot, and requests UI focus.
void cUIMemory::Init() {
	m_isActive = false;               					// Set UI as inactive initially
	m_MemorySlot = 0;                 					// Default memory slot
	m_MemChoice = eMemChoice::No;     					// Default user choice
	m_MemState = eMemState::Save;     					// Default memory operation state
	m_ActionExec = 0;                 					// No action executed initially
	m_MemorySlot = cPendaUI::m_Memory.getActiveSlot(); 	// Get the active memory slot
	RestoreSlot();                    					// Restore data from the active slot
	cPendaUI::RequestFocus(this);     					// Request UI focus
	m_PressCount = 0;
}

// --------------------------------------------------------------------------
// Function: Activate
// Description: Activates the memory UI menu. Changes the Z-order of UI layers
//              to bring the menu to the foreground and redraws the UI.
void cUIMemory::Activate(){
	m_isActive = true; 									// Set UI as active
	cPendaUI::m_pStatMainUpLayer->changeZOrder(41); 	// Bring static layer to front
	cPendaUI::m_pDynMainUpLayer->changeZOrder(40);  	// Bring dynamic layer to front
	drawMainUpStat();  									// Redraw static UI elements
	drawMainUpDyn();   									// Redraw dynamic UI elements
}

// --------------------------------------------------------------------------
// Function: DeActivate
// Description: Deactivates the memory UI menu. Resets the Z-order of UI layers
//              and updates the memory slot display.
void cUIMemory::DeActivate(){
	m_isActive = false; 								// Set UI as inactive
	cPendaUI::m_pStatMainUpLayer->changeZOrder(0); 		// Reset static layer Z-order
	cPendaUI::m_pDynMainUpLayer->changeZOrder(0); 		// Reset dynamic layer Z-order
	m_MemorySlot = cPendaUI::m_Memory.getActiveSlot(); 	// Update active memory slot
	drawMainDownDyn(); 									// Redraw dynamic UI elements
}

// --------------------------------------------------------------------------
// Function: Update
// Description: Handles user input and updates the UI state based on encoder
//              movements and button presses.
void cUIMemory::Update(){
	float PressDuration=0;
	uint64_t PressCount = cPendaUI::m_FootSwitch1.getPressCount();
	uint8_t SwitchSate = cPendaUI::m_FootSwitch1.getState(PressDuration);

	if((PressDuration > 1.0f) && (m_PressCount != PressCount)){
		m_PressCount = PressCount;
		if(cPendaUI::m_AudioState){
			cPendaUI::m_AudioState = false;
		}else{
			cPendaUI::m_AudioState = true;
		}
		drawMainDownDyn(); 	// Redraw dynamic UI elements

	}else if((SwitchSate == 0) && (PressDuration < 0.5f) && (m_PressCount != PressCount)){
		m_PressCount = PressCount;
		uint8_t activeSlot = cPendaUI::m_Memory.getActiveSlot();
		uint8_t targetSlot = activeSlot;

		do {
		    targetSlot = (targetSlot + 1) % NB_MEM_SLOT;

		    if (targetSlot == activeSlot) {
		        // No loadable slot found
		        break;
		    }

		    if (isLoadable(targetSlot)) {
		        m_MemorySlot = targetSlot;
		        RestoreSlot();
		        drawMainDownDyn();  // Redraw dynamic UI elements
		        cPendaUI::ReDraw();
		        break;
		    }
		} while (true);
	}

	if (!m_isActive) return; // Skip update if UI is inactive

	// Handle encoder 1 input (changes memory operation state)
	if (cPendaUI::m_Encoder1Increment != 0) {
		if (cPendaUI::m_Encoder1Increment > 0) {
			// Rotate through states: Save -> Delete -> Load -> Save
			switch (m_MemState) {
				case eMemState::Save: m_MemState = eMemState::Delete; break;
				case eMemState::Delete: m_MemState = eMemState::Load; break;
				case eMemState::Load: m_MemState = eMemState::Save; break;
			}
		} else {
			// Rotate through states in reverse: Save -> Load -> Delete -> Save
			switch (m_MemState) {
				case eMemState::Save: m_MemState = eMemState::Load; break;
				case eMemState::Delete: m_MemState = eMemState::Save; break;
				case eMemState::Load: m_MemState = eMemState::Delete; break;
			}
		}
		m_MemChoice = eMemChoice::No; 					// Reset user choice
		cPendaUI::m_Encoder1Increment = 0; 				// Reset encoder input
		drawMainUpStat(); 								// Redraw static UI elements
		drawMainUpDyn();  								// Redraw dynamic UI elements
	}

	// Handle encoder 2 input (changes memory slot)
	if (cPendaUI::m_Encoder2Increment != 0) {
		m_MemorySlot += cPendaUI::m_Encoder2Increment; 	// Update memory slot
		if (m_MemorySlot > 9) m_MemorySlot = 9; 		// Clamp to maximum slot
		if (m_MemorySlot < 0) m_MemorySlot = 0; 		// Clamp to minimum slot
		cPendaUI::m_Encoder2Increment = 0; 				// Reset encoder input
		m_MemChoice = eMemChoice::No; 					// Reset user choice
		drawMainUpDyn();  								// Redraw dynamic UI elements
		drawMainDownDyn(); 								// Redraw dynamic UI elements
	}

	// Handle encoder 3 input (toggles user choice between Yes and No)
	if (cPendaUI::m_Encoder3Increment != 0) {
		m_MemChoice = (eMemChoice)(((uint32_t)m_MemChoice + cPendaUI::m_Encoder3Increment) & 0x01);
		cPendaUI::m_Encoder3Increment = 0; 				// Reset encoder input
		drawMainUpDyn(); 								// Redraw dynamic UI elements
	}

	// Handle encoder 3 button press (executes selected memory operation)
	if (cPendaUI::m_Encoder3.getSwitchState() == 1) {
		if ((m_ActionExec == 0) && (m_MemChoice == eMemChoice::Yes)) {
			switch (m_MemState) {
				case eMemState::Save: SaveSlot(); break; // Save data to slot
				case eMemState::Delete:
					if (1 == isErasable(m_MemorySlot)) {
						cPendaUI::m_Memory.Erase(m_MemorySlot); // Delete data from slot
					}
					break;
				case eMemState::Load:
					if (1 == isLoadable(m_MemorySlot)) {
						RestoreSlot(); 					// Load data from slot
					}
					break;
			}
			m_MemChoice = eMemChoice::No; 				// Reset user choice
			drawMainUpDyn(); 							// Redraw dynamic UI elements
		}
		m_ActionExec = 1; 								// Mark action as executed
	} else {
		m_ActionExec = 0; 								// Reset action execution flag
	}
}

// --------------------------------------------------------------------------
// Function: OnMainFocusLost
// Description: Called when the UI loses focus. Resets the Z-order of UI layers.
void cUIMemory::OnMainFocusLost(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(0); 	// Reset static layer Z-order
	cPendaUI::m_pDynMainDownLayer->changeZOrder(0);  	// Reset dynamic layer Z-order
}

// --------------------------------------------------------------------------
// Function: OnMainFocusGained
// Description: Called when the UI gains focus. Updates the Z-order of UI layers
//              and redraws the UI.
void cUIMemory::OnMainFocusGained(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(40); 	// Bring static layer to front
	cPendaUI::m_pDynMainDownLayer->changeZOrder(41); 	// Bring dynamic layer to front
	drawMainDownStat(); 								// Redraw static UI elements
	drawMainDownDyn();  								// Redraw dynamic UI elements
}

// --------------------------------------------------------------------------
// Function: drawMainUpStat
// Description: Draws static UI elements for the memory menu.
//              This includes the text indicating the current state (Save, Delete, Load) and the "Yes" and "No" buttons.
void cUIMemory::drawMainUpStat(){
	char Buffer[10]; // Buffer to store the current state text
	switch (m_MemState) {
		case eMemState::Save: snprintf(Buffer, sizeof(Buffer), "Save"); break; 		// State: Save
		case eMemState::Delete: snprintf(Buffer, sizeof(Buffer), "Delete"); break; 	// State: Delete
		case eMemState::Load: snprintf(Buffer, sizeof(Buffer), "Load"); break; 		// State: Load
	}
	cPendaUI::m_pStatMainUpLayer->eraseLayer(); 									// Clear the static layer before redrawing

	// Draw the current state text (Save, Delete, Load)
	cPendaUI::m_pStatMainUpLayer->setFont(cPendaUI::m_pFont_XL); 					// Set XL font
	uint16_t TextHeight = cPendaUI::m_pStatMainUpLayer->getTextHeight(); 			// Get text height
	cPendaUI::m_pStatMainUpLayer->setCursor(25, (PARAM_HEIGHT - TextHeight) / 2); 	// Position cursor
	cPendaUI::m_pStatMainUpLayer->drawText(Buffer); 								// Draw the text

	// Draw rectangles and text for the "Yes" and "No" buttons
	cPendaUI::m_pStatMainUpLayer->setFont(cPendaUI::m_pFont_L); 					// Set L font
	TextHeight = cPendaUI::m_pStatMainUpLayer->getTextHeight(); 					// Get text height

	cPendaUI::m_pStatMainUpLayer->drawRect(218, (PARAM_HEIGHT / 2) - 42, 72, 39, 1, LAYER_PARAMETER_MAIN_COLOR);// Draw "Yes" rectangle
	uint16_t TextWidth = cPendaUI::m_pStatMainUpLayer->getTextWidth("Yes"); 		// Get text width
	cPendaUI::m_pStatMainUpLayer->setCursor(255 - (TextWidth / 2), ((PARAM_HEIGHT - TextHeight) / 2) - 22); // Position cursor
	cPendaUI::m_pStatMainUpLayer->drawText("Yes"); 									// Draw "Yes" text

	cPendaUI::m_pStatMainUpLayer->drawRect(218, (PARAM_HEIGHT / 2) + 3, 72, 39, 1, LAYER_PARAMETER_MAIN_COLOR); // Draw "No" rectangle
	TextWidth = cPendaUI::m_pStatMainUpLayer->getTextWidth("No"); 					// Get text width
	cPendaUI::m_pStatMainUpLayer->setCursor(255 - (TextWidth / 2), ((PARAM_HEIGHT - TextHeight) / 2) + 22); // Position cursor
	cPendaUI::m_pStatMainUpLayer->drawText("No"); 									// Draw "No" text
}

// --------------------------------------------------------------------------
// Function: drawMainUpDyn
// Description: Draws dynamic UI elements for the memory menu based on the current state.
//              This includes highlighting the selected choice (Yes or No) if applicable.
void cUIMemory::drawMainUpDyn(){
	cPendaUI::m_pDynMainUpLayer->eraseLayer(); // Clear the dynamic layer before redrawing

	uint8_t drawYesNoRect = 0; // Flag to determine if "Yes" or "No" rectangles should be drawn
	switch (m_MemState) {
		case eMemState::Save: drawYesNoRect = 1; break; 							// Always draw for Save state
		case eMemState::Delete: drawYesNoRect = isErasable(m_MemorySlot); break; 	// Draw if slot is erasable
		case eMemState::Load: drawYesNoRect = isLoadable(m_MemorySlot); break; 		// Draw if slot is loadable
	}
	if (1 == drawYesNoRect) {
		if (m_MemChoice == eMemChoice::Yes) {
			// Highlight the "Yes" rectangle if selected
			cPendaUI::m_pDynMainUpLayer->drawFillRect(218, (PARAM_HEIGHT / 2) - 42, 72, 39, MENU_ACTIVE_ITEM_COLOR);
		} else {
			// Highlight the "No" rectangle if selected
			cPendaUI::m_pDynMainUpLayer->drawFillRect(218, (PARAM_HEIGHT / 2) + 3, 72, 39, MENU_ACTIVE_ITEM_COLOR);
		}
	}
}

// --------------------------------------------------------------------------
// Function: drawMainDownStat
// Description: Draws static UI elements for the bottom layer of the memory menu.
//              This includes the label "MEM : " to indicate the memory section.
#define CENTER_MEM 58
#define CENTER_ONOFF 180
void cUIMemory::drawMainDownStat(){
	cPendaUI::m_pStatMainDownLayer->eraseLayer(MENU_BACK_COLOR); 			// Clear the static layer with background color
	cPendaUI::m_pStatMainDownLayer->setFont(cPendaUI::m_pFont_LB); 			// Set L font
	uint16_t TextWidth = cPendaUI::m_pStatMainDownLayer->getTextWidth("MEM.");
	cPendaUI::m_pStatMainDownLayer->setCursor(CENTER_MEM - (TextWidth/2), 2); // Position cursor
	cPendaUI::m_pStatMainDownLayer->drawText("MEM."); 					// Draw the label
}

// --------------------------------------------------------------------------
// Function: drawMainDownDyn
// Description: Draws dynamic UI elements for the bottom layer of the memory menu.
//              This includes the current memory slot number and a "Modified" indicator if applicable.
void cUIMemory::drawMainDownDyn(){
	if(0 == cPendaUI::HasFocus(this)) return;
	cPendaUI::m_pDynMainDownLayer->eraseLayer(); 							// Clear the dynamic layer before redrawing

	// Draw the current memory slot number
	cPendaUI::m_pDynMainDownLayer->setFont(cPendaUI::m_pFont_XLB); 			// Set XXL font

	char Buff[8]; 															// Buffer to store the slot number text
	snprintf(Buff, sizeof(Buff), "%d", m_MemorySlot + 1); 					// Format the slot number
	uint16_t TextWidth = cPendaUI::m_pDynMainDownLayer->getTextWidth(Buff); // Get text width
	cPendaUI::m_pDynMainDownLayer->setCursor(CENTER_MEM - (TextWidth/2), 25); // Position cursor
	cPendaUI::m_pDynMainDownLayer->drawText(Buff); 							// Draw the slot number

	// Draw "Modified" indicator if the memory is dirty
	if (1 == cPendaUI::m_Memory.isDirty()) {
		cPendaUI::m_pStatMainDownLayer->setFont(cPendaUI::m_pFont_L); 			// Set L font
		uint16_t TextWidth = cPendaUI::m_pStatMainDownLayer->getTextWidth("MEM.");
		cPendaUI::m_pDynMainDownLayer->setFont(cPendaUI::m_pFont_M); 		// Set M font
		cPendaUI::m_pDynMainDownLayer->setCursor(CENTER_MEM + (TextWidth/2) + 5, 2); // Position cursor
		cPendaUI::m_pDynMainDownLayer->drawText("Modif."); 				// Draw the text
	}

	cPendaUI::m_pDynMainDownLayer->setFont(cPendaUI::m_pFont_XXL); 			// Set XXL font
	if (cPendaUI::m_AudioState){
		uint16_t TextWidth = cPendaUI::m_pStatMainDownLayer->getTextWidth("ON");
		cPendaUI::m_pDynMainDownLayer->setCursor(CENTER_ONOFF - (TextWidth / 2), 6); // Position cursor
		cPendaUI::m_pDynMainDownLayer->drawText("ON");
	}else{
		uint16_t TextWidth = cPendaUI::m_pStatMainDownLayer->getTextWidth("OFF");
		cPendaUI::m_pDynMainDownLayer->setCursor(CENTER_ONOFF - (TextWidth / 2), 6); // Position cursor
		cPendaUI::m_pDynMainDownLayer->drawText("OFF");
	}
}

// --------------------------------------------------------------------------
// Function: SaveSlot
// Description: Saves the current state to the selected memory slot.
void cUIMemory::SaveSlot() {
	DadQSPI::cSerialize Serializer; 										// Create a serializer object
	cPendaUI::Save(Serializer); 											// Serialize the current state
	const uint8_t* pBuffer = nullptr; 										// Pointer to the serialized data
	uint32_t Size = Serializer.getBuffer(&pBuffer);							// Get the size of the serialized data
	cPendaUI::m_Memory.Save(m_MemorySlot, pBuffer, Size); 					// Save the data to the memory slot
}

// --------------------------------------------------------------------------
// Function: RestoreSlot
// Description: Restores the state from the selected memory slot.
void cUIMemory::RestoreSlot() {
	uint32_t Size = cPendaUI::m_Memory.getSize(m_MemorySlot); 				// Get the size of the data in the slot
	if (Size != 0) {
		uint8_t* pBuffer = new uint8_t[Size]; 								// Allocate memory for the data
		if (pBuffer != nullptr) {
			if (0 != cPendaUI::m_Memory.Restore(m_MemorySlot, pBuffer, Size)) { // Restore the data
				DadQSPI::cSerialize Serializer; 							// Create a serializer object
				Serializer.setBuffer(pBuffer, Size); 						// Set the buffer with the restored data
				cPendaUI::Restore(Serializer); 								// Deserialize and restore the state
			}
			delete[] pBuffer; 												// Free the allocated memory
		}
	}
}

//***********************************************************************************
// Class: cTapTempo
// Description: This class handles the Tap Tempo functionality, updating a parameter
//              based on the timing of a footswitch press. It also manages UI focus
//              and refreshes the display accordingly.
//***********************************************************************************

// --------------------------------------------------------------------------
// Function: Init
// Description: Initializes the Tap Tempo object with a footswitch and a parameter view.
//
// Parameters:
//   pFootSwitch     - Pointer to the switch used for tap tempo detection.
//   pParameterView  - Pointer to the UI parameter view that will be updated.
//
void cTapTempo::Init(cSwitch* pFootSwitch, cParameterView* pParameterView, eTempoType TempoType) {
	m_pFootSwitch = pFootSwitch;
	m_PeriodUpdateCount = 0;
	m_focusTimer = 0;
	m_pParameterView = pParameterView;
}

// --------------------------------------------------------------------------
// Function: Update
// Description: Periodically updates the tap tempo parameter and manages UI focus.
//
void cTapTempo::Update(){
	uint32_t PeriodUpdateCount = m_pFootSwitch->getPeriodUpdateCount();

	// Check if the average period has been updated
	if ((0 != PeriodUpdateCount) && (m_PeriodUpdateCount != PeriodUpdateCount)) {
		// Update the parameter value with the latest press period
		if(m_TempoType == eTempoType::frequency){
			m_pParameterView->getParameter()->setValue(m_pFootSwitch->getPressPeriod());
		}else{
			m_pParameterView->getParameter()->setValue(1.0f/m_pFootSwitch->getPressPeriod());
		}

		m_PeriodUpdateCount = PeriodUpdateCount;

		// Request UI focus if not already focused
		if (0 == cPendaUI::HasFocus(this)) {
			cPendaUI::RequestFocus(this);
		}

		// Redraw the dynamic main view layer
		m_pParameterView->drawDynMainView(cPendaUI::m_pDynMainDownLayer);

		// Set focus timer to keep UI active for a defined duration
		m_focusTimer = TIME_FOCUS_MAIN;
	}
	else {
		// If focus timer is active, count down and handle UI focus release
		if (m_focusTimer != 0) {
			m_focusTimer--;  // Decrement the focus timer

			// If the timer reaches zero, release focus and trigger a redraw
			if (m_focusTimer == 0) {
				cPendaUI::m_Memory.setDirty();
				cPendaUI::ReleaseFocus();
				cPendaUI::ReDraw();
			}
		}
	}
}

// --------------------------------------------------------------------------
// Function: OnMainFocusLost
// Description: Called when the UI focus is lost, adjusting layer visibility.
//
void cTapTempo::OnMainFocusLost(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(0);  // Reset static layer priority
	cPendaUI::m_pDynMainDownLayer->changeZOrder(0);   // Reset dynamic layer priority
}

// --------------------------------------------------------------------------
// Function: OnMainFocusGained
// Description: Called when the UI gains focus, bringing relevant layers to the foreground.
//
void cTapTempo::OnMainFocusGained(){
	cPendaUI::m_pStatMainDownLayer->changeZOrder(40); // Bring static layer forward
	cPendaUI::m_pDynMainDownLayer->changeZOrder(41);  // Bring dynamic layer forward
	m_pParameterView->drawStatMainView(cPendaUI::m_pStatMainDownLayer);
}

//***********************************************************************************
// Class: cUIMenu
// Description: Handles the graphical menu and navigation logic.
//              This class manages the display and interaction with a menu composed of items.
//***********************************************************************************

// --------------------------------------------------------------------------
// Function: Init
// Description: Initializes the menu by clearing existing items and deactivating the menu.
void cUIMenu::Init() {
	m_TabMenuItem.clear(); 	// Clear the list of menu items
	m_ActiveItem = 0;    	// Reset the active item index
	m_MenuShift = 0;      	// Reset the menu shift index
	m_isActive = 0;       	// Mark the menu as inactive
	DeActivate();         	// Deactivate the menu
}

// --------------------------------------------------------------------------
// Function: addMenuItem
// Description: Adds a new menu item to the list.
// Parameters:
//   - pItem: Pointer to the GUI object associated with the menu item.
//   - Name: The name of the menu item to display.
void cUIMenu::addMenuItem(iGUIObject* pItem, const std::string& Name) {
	MenuItem Item;
	Item.Name = Name;     			// Set the name of the menu item
	Item.pItem = pItem;   			// Set the associated GUI object
	m_TabMenuItem.push_back(Item); 	// Add the item to the list
}

// --------------------------------------------------------------------------
// Function: Activate
// Description: Activates the menu and the currently selected item.
//              Brings the menu layers to the foreground and draws the menu.
void cUIMenu::Activate(){
	cPendaUI::m_pDynMenuLayer->changeZOrder(10); 		// Bring dynamic menu layer to the foreground
	cPendaUI::m_pStatMenuLayer->changeZOrder(11); 		// Bring static menu layer to the foreground

	m_isActive = true; 									// Mark the menu as active
	drawTab(); 											// Draw the menu tabs

	if (!m_TabMenuItem.empty()) {
		m_TabMenuItem[m_ActiveItem].pItem->Activate(); 	// Activate the currently selected item
	}
}

// --------------------------------------------------------------------------
// Function: DeActivate
// Description: Deactivates the menu and the currently selected item.
//              Resets the Z-order of the menu layers and marks the menu as inactive.
void cUIMenu::DeActivate(){
	cPendaUI::m_pDynMenuLayer->changeZOrder(0); 		// Reset dynamic menu layer Z-order
	cPendaUI::m_pStatMenuLayer->changeZOrder(0); 		// Reset static menu layer Z-order

	m_isActive = false; 								// Mark the menu as inactive
	if (!m_TabMenuItem.empty()) {
		m_TabMenuItem[m_ActiveItem].pItem->DeActivate();// Deactivate the currently selected item
	}
}

// --------------------------------------------------------------------------
// Function: Update
// Description: Updates the menu logic based on user input.
//              Handles navigation through the menu items using encoder input.
void cUIMenu::Update(){
	if (!m_isActive || m_TabMenuItem.empty()) return; 	// Skip update if the menu is inactive or empty

	int8_t OldActiveItem = m_ActiveItem; 				// Store the previously active item

	// Handle encoder rotation for menu navigation
	if (cPendaUI::m_Encoder0Increment != 0) {
		m_ActiveItem += cPendaUI::m_Encoder0Increment; 	// Update the active item index
		cPendaUI::m_Encoder0Increment = 0; 				// Reset encoder input

		// Ensure the active item is within valid range
		if (m_ActiveItem >= (int8_t)m_TabMenuItem.size()) {
			m_ActiveItem = m_TabMenuItem.size() - 1; 	// Clamp to the last item
		}
		if (m_ActiveItem < 0) {
			m_ActiveItem = 0; 							// Clamp to the first item
		}

		// Adjust menu shift to ensure the active item is visible
		while (m_ActiveItem >= (NB_MENU_ITEM + m_MenuShift)) {
			m_MenuShift++; 								// Shift the menu to the right
		}
		while (m_ActiveItem < m_MenuShift) {
			m_MenuShift--; 								// Shift the menu to the left
		}

		// Deactivate the previous item and activate the new one
		m_TabMenuItem[OldActiveItem].pItem->DeActivate();
		m_TabMenuItem[m_ActiveItem].pItem->Activate();
		drawTab(); 										// Redraw the menu tabs
	}
}

// --------------------------------------------------------------------------
// Function: drawTab
// Description: Draws the menu tabs, including the active item and navigation indicators.
void cUIMenu::drawTab(){
	cPendaUI::m_pDynMenuLayer->eraseLayer(MENU_BACK_COLOR); // Clear the dynamic menu layer
	uint8_t LastTab = m_MenuShift + NB_MENU_ITEM;			// Calculate the last visible tab
	uint16_t xTab = SCREEN_EDGE; 							// Starting X position for the first tab

	// Draw each visible tab
	for (uint8_t Index = m_MenuShift; Index < LastTab; Index++) {
		if (Index == m_ActiveItem) {
			// Highlight the active tab
			cPendaUI::m_pDynMenuLayer->drawFillRect(xTab, 0, MENU_ITEM_WIDTH, MENU_HEIGHT, MENU_ACTIVE_ITEM_COLOR);
		}

		if (Index < m_TabMenuItem.size()) {
			// Draw the tab name
			uint16_t TextWidth = cPendaUI::m_pDynMenuLayer->getTextWidth(m_TabMenuItem[Index].Name.c_str());
			cPendaUI::m_pDynMenuLayer->setCursor(xTab + (MENU_ITEM_WIDTH - TextWidth) / 2, 2);
			if (Index == m_ActiveItem) {
				cPendaUI::m_pDynMenuLayer->setTextFrontColor(MENU_SELETED_ITEM_COLOR);
			}else{
				cPendaUI::m_pDynMenuLayer->setTextFrontColor(MENU_UNSELETED_ITEM_COLOR);
			}
			cPendaUI::m_pDynMenuLayer->drawText(m_TabMenuItem[Index].Name.c_str());
			xTab += MENU_ITEM_WIDTH; // Move to the next tab position
		}
	}

	// Draw left arrow indicator if there are hidden items to the left
	if (m_MenuShift > 0) {
		drawArrowIndicator(true);
	}

	// Draw right arrow indicator if there are hidden items to the right
	if (m_MenuShift < (int8_t)(m_TabMenuItem.size() - NB_MENU_ITEM)) {
		drawArrowIndicator(false);
	}
}
// --------------------------------------------------------------------------
// Function: drawArrowIndicator
// Description: Draws an arrow indicator on the left or right side of the menu.
// Parameters:
//   - isLeft: If true, draw a left arrow; otherwise, draw a right arrow.
void cUIMenu::drawArrowIndicator(bool isLeft) {
	uint16_t YMin = SCREEN_EDGE;
	uint16_t YMax = SCREEN_EDGE + 1;
	uint16_t XStart = isLeft ? 0 : 319; 		// Starting X position for the arrow
	int16_t XOffset = isLeft ? 1 : -1;			// Direction of the arrow

	// Draw the arrow line by line
	for (uint16_t Index = 0; Index < SCREEN_EDGE; Index++) {
		cPendaUI::m_pDynMenuLayer->drawLine(XStart + Index * XOffset, YMin, XStart + Index * XOffset, YMax, LAYER_PARAMETER_MAIN_COLOR);
		YMin--; 								// Adjust Y position for the next line
		YMax++;
	}
}
} // namespace DadUI
