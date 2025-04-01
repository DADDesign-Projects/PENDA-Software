

class UI {
public :

protected:
};

using CallbackType = std::function<void(float)>;

class cParameter : public DadQSPI::cSerializedObject {

class cParameter{
public :


protected:

};

class cParameterNum{
public :

protected:

};

class cParameterDiscret{
public :

protected:

};

class cParameterBool{
public :

protected:

};

cParameterItem{
public:

protected:

};

class cParamView{
public :
virtual drawStaticView(cLayer pLayer) = 0;
virtual drawDynamicView(cLayer pLayer) = 0;

protected:

};

class cParamViewNum : public cParamView{
public :

protected:
	cParameterNum  m_Parameter;
};

class cParamViewDiscret : public cParamView{
public :

protected:
	cParameter

};

class cParamViewBool : public cParamView{
public :

protected:

};


class cMenuItem{
public:

protected:

};

class cMenu {
public:

protected:

};

class cMainView {
public:

protected:

};

class cGUI {
public:

	static void Initialize(){

	}

	static cGUIObject* setActiveObject(cGUIObject* pActiveObject){
		cGUIObject* pOldActiveOject = m_pActiveObject;
		pOldActiveOject->DeActivate();
		m_pActiveObject = pActiveObject;
		pActiveObject->Activate();
		return pOldActiveOject;
	}

	static void Proceed(){
		pActiveObject->Proceed();
	}

	static void Debounce(){
        m_Encoder0.Debounce();
        m_Encoder0Increment += m_Encoder0.getIncrement();

        m_Encoder1.Debounce();
        m_Encoder1Increment += m_Encoder1.getIncrement();

        m_Encoder2.Debounce();
        m_Encoder2Increment += m_Encoder2.getIncrement();

        m_Encoder3.Debounce();
        m_Encoder3Increment += m_Encoder3.getIncrement();

    	m_Switch1.Debounce();
    	m_Switch2.Debounce();

        for(cParameter *pParameter : m_TabParameters){
    		pParameter->Refresh();
    	}
	}

	//--------------------------------------------------------------
	// data member

	// Display Layers
	static cLayer 			*m_pDynMenuLayer;
	static cLayer			*m_pStatMenuLayer;

	static cLayer			*m_pDynParam1Layer;
	static cLayer			*m_pStatParam1Layer;
	static cLayer			*m_pDynParam2Layer;
	static cLayer			*m_pStatParam2Layer;
	static cLayer			*m_pDynParam3Layer;
	static cLayer			*m_pStatParam3Layer;

	static cLayer			*m_pDynMainLayer;
	static cLayer			*m_pStatMainLayer;

    // Font
	static DadGFX::cFont*	m_pFont_S;
    static DadGFX::cFont*	m_pFont_M;
    static DadGFX::cFont*	m_pFont_L;
    static DadGFX::cFont*	m_pFont_XL;
    static DadGFX::cFont*	m_pFont_XXL;

	// Hard UI
    static cEncoder			m_Encoder0;
    static cEncoder			m_Encoder1;
    static cEncoder			m_Encoder2;
    static cEncoder			m_Encoder3;
    static int32_t			m_Encoder0Increment;
    static int32_t			m_Encoder1Increment;
    static int32_t			m_Encoder2Increment;
    static int32_t			m_Encoder3Increment;

    static cSwitch			m_FootSwitch1;
    static cSwitch			m_FootSwitch2;

protected:

	static cGUIObject*	 m_pActiveObject;

    // Array of parameters for serialization and smooth value updates according to the slope
    static std::vector<cParameter*> m_TabParameters; // List of parameters
};


class cGUIObject {

public:
	cGUIObject(cGUI* pGUI){
		m_pGUI = pGUI;
	}

	virtual void Activate()=0;
	virtual void DeActivate()=0;
	virtual void Proceed()=0;

protected:
	cGUI* 	m_pGUI;
};

class cMenu : public  cGUIObject {
public:
	cMenu(){
		m_TabMenuItem.clear();
	}

	void addMenuItem(cMenuItem* pMenuItem);

	void drawStaticLayer();
	void drawDynamicLayer();

	void Activate()override;
	void DeActivate()override;
	void Refresh()override;

protected:
	vector<cMenuItem *> m_TabMenuItem;
	uint8_t				m_ItemSelected;

};


