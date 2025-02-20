#pragma once
//====================================================================================
// Delay.h
//
//
// Copyright(c) 2025 Dad Design.
//====================================================================================
#include <UIComponent.h>
#include "main.h"
#include <cstdint>
#include "GUI.h"

namespace DadDelay {

class cDelayUI{
public:
	cDelayUI(){}
	~cDelayUI(){};

	void Init(){
		DadUI::cUI::Init();
		DadUI::cUIMain::InitMain();

		m_Modulation.Init("Mod", "Mod. Deep", 0, 0, 1, 0.1, 0.02);
		m_Time.Init("Time", "Time", 0.350, 0.1, 1, 0.1, 0.01);
		m_Deep.Init("Deep", "Deep", .5, 0, 1, 0.1, 0.01);
		m_UIDelay.InitParameters(&m_Modulation, &m_Time, &m_Deep);

		m_Bass.Init("Bass", "Bass", .5, 0, 1, 0.1, 0.01);
		m_Treble.Init("Treble", "Treble", .5, 0, 1, 0.1, 0.01);
		m_ModulationSpeed.Init("Speed", "Mod. Speed", .5, 0, 1, 0.1, 0.01);
		m_ModulationShape.Init("Shape", "Mod. Shape", 0, 0, 2, 1, 1 , DadUI::PARAM_TYPE_VIEW::Discret);
		m_ModulationShape.addDiscretValue("Sin", "Sinusoidal");
		m_ModulationShape.addDiscretValue("Rect", "Rectanglar");
		m_ModulationShape.addDiscretValue("Square", "Square");
		m_UIDelay.InitParameters(&m_Modulation, &m_Time, &m_Deep);
		m_UITone.InitParameters(&m_Bass, nullptr, &m_Treble);
		m_UIModulation.InitParameters(&m_ModulationSpeed, &m_ModulationShape, &m_Modulation);

		m_MainUI.addChild(&m_UIDelay, "Delay", 1);
		m_MainUI.addChild(&m_UITone, "Tone", 2);
		m_MainUI.addChild(&m_UIModulation, "Mod.", 3);

		DadUI::cUI::Start(&m_MainUI);
	}


protected :
	// ---------------------------------------------------------------------------
	// Parameters
	DadUI::cParameter m_Modulation;
	DadUI::cParameter m_Time;
	DadUI::cParameter m_Deep;

	DadUI::cParameter m_Bass;
	DadUI::cParameter m_Treble;

	DadUI::cParameter m_ModulationSpeed;
	DadUI::cParameter m_ModulationShape;



	// ---------------------------------------------------------------------------
	// UI
	DadUI::cUIParameter m_UIDelay;
	DadUI::cUIParameter m_UITone;
	DadUI::cUIParameter m_UIModulation;
	DadUI::cUIMain m_MainUI;
};

} // DadDelay
