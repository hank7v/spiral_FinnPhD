/*! 
***************************************************************************


***************************************************************************
*/



#ifndef AbstractUIConfigurator_h
/// multiple include protection
#define AbstractUIConfigurator_h 1

#include <math.h>                                       // sqrt
#include  "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"
//	x INTERPOLATION												___FF___ 
#include <iostream>
#include <iomanip>
#include <vector>

#include "UIConfig.h"

class AbstractUIConfigurator
{
public:
	AbstractUIConfigurator(void);  
	~AbstractUIConfigurator(void);
	//this is defined pure virtual, as in principle we may want getValue functions to do something
	// different in subclasses, and because in principle this should be a virtual class  
	//virtual long getUIConfigurationDefaultValue()=0; 
	// but for the most of the getValue methods what we need is just to return the corepsonding member
	// member variable value, thus we can save time defining it in the superclass only....
	// (see subclasses, the overriding is commented out)
	//virtual long getUIConfigurationMinValue();
	//virtual long getUIConfigurationMaxValue();
	//virtual long getUIConfigurationIncValue();
	//virtual char* getUIConfigurationLabelID();
	virtual long getUIChoice();
	virtual void setUIChoice(long lChoice);
	virtual UIelementSet& getUIConfig()=0;
	UIelementSet m_UIConfig;

protected:
	//long m_lDefault;
	//long m_lMin;
	//long m_lMax;
	//long m_lInc;
	//char* m_pLabel;
	// UIChoice is used to share an identificative non-hardcoded "label" bewteen Seq and UIConfig. eg in the case we 
	// handle the UIConfig class determination by a Selection Box, sequence uses the Sel Box value to set UIConfig
	// UI Choice value, and retrieves it later for comparison purposes between tha current Sel Box and UIConfig values.
	// In case of discrepancies action is required (refresh)
	long m_lUIChoice;
	UIelementLong m_LongElm1;
	UIelementLong m_LongElm2;
	UIelementDouble m_DoubleElm;
	UIelement m_StdCheckBoxUIelm;
	UIelementSet m_ElmSet;
	UIelementSelectionBox m_UIelmSelBox;



};


class TrapGradUIConfigurator: public AbstractUIConfigurator
{
public:
	TrapGradUIConfigurator(void);  
	~TrapGradUIConfigurator(void);
	//virtual long getUIConfigurationDefaultValue();
	virtual UIelementSet& getUIConfig();
	/*long getUIConfigurationMinValue();
	long getUIConfigurationMaxValue();
	long getUIConfigurationIncValue();
	char* getUIConfigurationLabelID();*/
	//UIelementSet m_UIConfig;
/*private:
	UIelementLong m_LongElm;
	UIelementDouble m_DoubleElm;
	UIelement m_StdCheckBoxUIelm;
	UIelementSet m_ElmSet;*/
	
};


class FWFGradUIConfigurator: public AbstractUIConfigurator
	{
public:
	FWFGradUIConfigurator(void);  
	~FWFGradUIConfigurator(void);
	//virtual long getUIConfigurationDefaultValue();
	virtual UIelementSet& getUIConfig();
	/*long getUIConfigurationMinValue();
	long getUIConfigurationMaxValue();
	long getUIConfigurationIncValue();
	char* getUIConfigurationLabelID();*/
	//UIelementSet m_UIConfig;
/*private:
	UIelementLong m_LongElm;
	UIelementDouble m_DoubleElm;
	UIelement m_StdCheckBoxUIelm;
	UIelementSet m_ElmSet;*/
	
};


class OscGradUIConfigurator: public AbstractUIConfigurator
{
public:
	OscGradUIConfigurator(void);  
	~OscGradUIConfigurator(void);
	//virtual long getUIConfigurationDefaultValue();
	virtual UIelementSet& getUIConfig();
	/*long getUIConfigurationMinValue();
	long getUIConfigurationMaxValue();
	long getUIConfigurationIncValue();
	char* getUIConfigurationLabelID();*/
	//UIelementSet m_UIConfig;
/*private:
	UIelementLong m_LongElm;
	UIelementDouble m_DoubleElm;
	UIelement m_StdCheckBoxUIelm;
	UIelementSet m_ElmSet;*/
	
};



#endif     
