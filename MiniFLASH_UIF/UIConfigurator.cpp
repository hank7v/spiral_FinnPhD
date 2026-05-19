/** 
***************************************************************************


***************************************************************************



***************************************************************************
*/


//#include <fstream.h>                                  // stream.open
#include <stdio.h>
#include <ctype.h>                                      // tolower, isspace
#include "UIConfigurator.h"	
#include "MrServers/MrImaging/seq/SeqDebug.h"           // mPrintTrace
#include "MrServers/MrMeasSrv/SeqIF/libRT/libRTDefines.h"

#include "UIF_UILink.h"

// ***************************************************************************
// class AbstractUIConfigurator
// ***************************************************************************


AbstractUIConfigurator::AbstractUIConfigurator (void)
	:m_UIConfig(40)//we give an ID to our UIelementSet
	,m_LongElm1		(UIF_UI::REG_LongElement1)//id=12
	,m_LongElm2		(UIF_UI::REG_LongElement2)//id=13
	,m_DoubleElm	(UIF_UI::REG_Selection_Box)//id=10 
	,m_UIelmSelBox	(UIF_UI::REG_Selection_Box)//id=10 
	,m_StdCheckBoxUIelm	(UIF_UI::REG_CheckBox)//id=2
	,m_ElmSet		(20)
{
	// Check box. just a test, it will go in GRAD STD UIelementset
	m_StdCheckBoxUIelm.m_pLabel="b0";
	m_StdCheckBoxUIelm.m_lWMBindex=UIF_UI::WMB_b0CheckBox;

}

AbstractUIConfigurator::~AbstractUIConfigurator(void)
{
}
void AbstractUIConfigurator::setUIChoice(long lChoice)
{
	m_lUIChoice=lChoice;
}
long AbstractUIConfigurator::getUIChoice(void)
{
	return m_lUIChoice;
}

// ***************************************************************************
// class TrapGradUIConfigurator
// ***************************************************************************


TrapGradUIConfigurator::TrapGradUIConfigurator (void)
{

	// UI LONG 1 element initialisaion 
	std::vector<long> lVectMin1, lVectMax1, lVectInc1, lVectDefault1;
	lVectMin1.push_back(1);
	lVectMax1.push_back(10);
	lVectInc1.push_back(1);
	lVectDefault1.push_back(5);
	m_LongElm1.init(lVectMin1, lVectMax1, lVectInc1, lVectDefault1);
	m_LongElm1.m_pLabel="TrapGrad Long 1";
	m_LongElm1.m_lWMBindex=UIF_UI::WMB_LongElementForGradScheme1;
	// UI LONG 2 element initialisaion 
	std::vector<long> lVectMin2, lVectMax2, lVectInc2, lVectDefault2;
	lVectMin2.push_back(1);
	lVectMax2.push_back(10);
	lVectInc2.push_back(1);
	lVectDefault2.push_back(5);
	m_LongElm2.init(lVectMin2, lVectMax2, lVectInc2, lVectDefault2);
	m_LongElm2.m_pLabel="TrapGrad Long 2";
	m_LongElm2.m_lWMBindex=UIF_UI::WMB_LongElementForGradScheme2;
	// SELECTION BOX
	m_UIelmSelBox.m_sOptions.push_back("Do I need this?");
	m_UIelmSelBox.m_sOptions.push_back("I dont think so.");
	m_UIelmSelBox.m_sOptions.push_back("Sure I don't.");
	m_UIelmSelBox.m_sOptions.push_back("But, who knows...");
	m_UIelmSelBox.m_pLabel="dynamical SEL BOX";
	m_UIelmSelBox.m_lWMBindex=UIF_UI::WMB_GradSchemeSelBox; 
	m_UIelmSelBox.m_lNofOptions=4;
	m_UIelmSelBox.m_lDefault=0;//Do I need this?
	//composing them 
	m_ElmSet.addElement(&m_UIelmSelBox);
	m_UIConfig.addElement(&m_ElmSet);
	m_UIConfig.addElement(&m_LongElm1);
	m_UIConfig.addElement(&m_LongElm2);
	bool b6=m_UIConfig.search(6);
}


TrapGradUIConfigurator::~TrapGradUIConfigurator(void)
{
}

UIelementSet&  TrapGradUIConfigurator::getUIConfig()
{
	return m_UIConfig;	
};

// ***************************************************************************
// class FWFGradUIConfigurator
// ***************************************************************************


FWFGradUIConfigurator::FWFGradUIConfigurator (void)
{
	// UI LONG 1 element initialisaion 
	std::vector<long> lVectMin1, lVectMax1, lVectInc1, lVectDefault1;
	lVectMin1.push_back(4);
	lVectMax1.push_back(50);
	lVectInc1.push_back(2);
	lVectDefault1.push_back(28);
	m_LongElm1.init(lVectMin1, lVectMax1, lVectInc1, lVectDefault1);
	m_LongElm1.m_pLabel="FWFGrad Long 1";
	m_LongElm1.m_lWMBindex=UIF_UI::WMB_LongElementForGradScheme1; 
	// UI LONG 2 element initialisaion 
	std::vector<long> lVectMin2, lVectMax2, lVectInc2, lVectDefault2;
	lVectMin2.push_back(1);
	lVectMax2.push_back(10);
	lVectInc2.push_back(1);
	lVectDefault2.push_back(5);
	m_LongElm2.init(lVectMin2, lVectMax2, lVectInc2, lVectDefault2);
	m_LongElm2.m_pLabel="FWFGrad Long 2";
	m_LongElm2.m_lWMBindex=UIF_UI::WMB_LongElementForGradScheme2;
	// SELECTION BOX
	m_UIelmSelBox.m_sOptions.push_back("my nice FWF");
	m_UIelmSelBox.m_sOptions.push_back("my nicer FWF");
	m_UIelmSelBox.m_sOptions.push_back("my best FWF");
	m_UIelmSelBox.m_sOptions.push_back("my even better one");
	m_UIelmSelBox.m_sOptions.push_back("just a decent FWF");
	m_UIelmSelBox.m_pLabel="FWF filename";
	m_UIelmSelBox.m_lWMBindex=UIF_UI::WMB_GradSchemeSelBox; 
	m_UIelmSelBox.m_lNofOptions=5;
	m_UIelmSelBox.m_lDefault=4;//just a decent FWF
	//composing them 
	m_ElmSet.addElement(&m_UIelmSelBox);
	m_UIConfig.addElement(&m_ElmSet);
	m_UIConfig.addElement(&m_LongElm1);
	m_UIConfig.addElement(&m_LongElm2);
	bool b6=m_UIConfig.search(6);
}


FWFGradUIConfigurator::~FWFGradUIConfigurator(void)
{
}

UIelementSet&  FWFGradUIConfigurator::getUIConfig()
{
	bool b7=m_UIConfig.search(6);
	return m_UIConfig;
};

// ***************************************************************************
// class OscGradUIConfigurator
// ***************************************************************************


OscGradUIConfigurator::OscGradUIConfigurator (void)
{
	// UI LONG element initialisaion 
	std::vector<long> lVectMin1, lVectMax1, lVectInc1, lVectDefault1;
	lVectMin1.push_back(10);
	lVectMax1.push_back(1000);
	lVectInc1.push_back(10);
	lVectDefault1.push_back(500);
	m_LongElm1.init(lVectMin1, lVectMax1, lVectInc1, lVectDefault1);
	m_LongElm1.m_pLabel="OscGrad Long 1";
	m_LongElm1.m_lWMBindex=UIF_UI::WMB_LongElementForGradScheme1;
	// UI LONG 2 element initialisaion 
	std::vector<long> lVectMin2, lVectMax2, lVectInc2, lVectDefault2;
	lVectMin2.push_back(1);
	lVectMax2.push_back(10);
	lVectInc2.push_back(1);
	lVectDefault2.push_back(5);
	m_LongElm2.init(lVectMin2, lVectMax2, lVectInc2, lVectDefault2);
	m_LongElm2.m_pLabel="OscGrad Long 2";
	m_LongElm2.m_lWMBindex=UIF_UI::WMB_LongElementForGradScheme2;
	// SELECTION BOX
	m_UIelmSelBox.m_sOptions.push_back("Osc Grad Opt 1");
	m_UIelmSelBox.m_sOptions.push_back("Osc Grad Opt 2");
	m_UIelmSelBox.m_sOptions.push_back("Osc Grad Opt 3");
	m_UIelmSelBox.m_pLabel="Osc Grad Options";
	m_UIelmSelBox.m_lWMBindex=UIF_UI::WMB_GradSchemeSelBox; 
	m_UIelmSelBox.m_lNofOptions=3;
	m_UIelmSelBox.m_lDefault=2;//Osc Grad Opt 2
	//composing them 
	m_ElmSet.addElement(&m_UIelmSelBox);
	m_UIConfig.addElement(&m_ElmSet);
	m_UIConfig.addElement(&m_LongElm1);
	m_UIConfig.addElement(&m_LongElm2);
	bool b6=m_UIConfig.search(6);
}


OscGradUIConfigurator::~OscGradUIConfigurator(void)
{
}

UIelementSet& OscGradUIConfigurator::getUIConfig()
{
	return m_UIConfig;
};


