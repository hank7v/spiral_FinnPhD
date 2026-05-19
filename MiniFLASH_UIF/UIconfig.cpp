/*! 
***************************************************************************


***************************************************************************
*/
#include "UIConfig.h"
#include <typeinfo>

// ***************************************************************************
// class UIConfig
// ***************************************************************************
UIConfig::UIConfig(int id):	m_lREGindex	(id) {}
UIConfig::~UIConfig() {}
bool UIConfig::search(int id)
{
	if (m_lREGindex==id)
	{
		return true;
	}
	return false;
}
UIConfig* UIConfig::returnElementByID(int id)
{
	return this;
}
int UIConfig::getREGindex()
{
	return m_lREGindex;
}

// ***************************************************************************
// class UIelementSet
// ***************************************************************************
UIelementSet::UIelementSet(int id) :UIConfig(id) {}
UIelementSet::~UIelementSet() {}
void UIelementSet::addElement(UIConfig* a)
{
	myElements.push_back(a); 
}

// this method return TRUE if any element aong the obes in the UIConfig object has a given id
// search iteratively goes through sub-commponents
bool UIelementSet::search(int id) 
{
	int myID=this->getREGindex();
	int mySize=myElements.size();
	//for (it = myElements.begin(); it != myElements.end(); ++it) 
	for (unsigned i=0; i < myElements.size(); i++) 
	{
		int a= myElements[i]->getREGindex();
		if (a>14)	// we set 14 as the highest REGindex for UIelements, above it we have only UIelementSets
		{
			bool b=myElements[i]->search(id);
			if (b==true)
			{
				return true;
			}
		}
		else if(a==id)
		{
			return true;
		}

	int c=1;
	}
	return false;
}


UIConfig* UIelementSet::returnElementByID(int id) 
{
	int myID=this->getREGindex();
	int mySize=myElements.size();
	//for (it = myElements.begin(); it != myElements.end(); ++it) 
	for (unsigned i=0; i < myElements.size(); i++) 
	{
		int a= myElements[i]->getREGindex();
		if (a>14)	// we set 14 as the highest REGindex for UIelements, above it we have only UIelementSets
		//if (typeid(&myElements[i]) == typeid(UIelementSet))
		{
			UIConfig* myAuxUIConfig=myElements[i]->returnElementByID(id);
			if (myAuxUIConfig)
			{
				return myAuxUIConfig;
			}
			else
			{
				// do nothing
			}
		}
		else if(a==id)
		{
			//UIelementSet *temp=static_cast<UIelementSet*>(myElements[i]);
			//UIConfig *temp=myElements[i];
			//temp->returnElementByID(8);
			myElements[i]->search(id);
			return myElements[i];
		}

	int c=1;
	}
	return NULL;
 }

bool UIelementSet::clearElements(void)
{
	myElements.clear();
	return true;
}



// ***************************************************************************
// class UIelement
// ***************************************************************************
UIelement::UIelement(int id) : UIConfig(id) 
{
	m_bIsAvailable=true;
};
UIelement::~UIelement() {};
/*UIConfig* UIelement::returnElementByID(int id) 
{
	int c=1;
	return this;
 }*/


// ***************************************************************************
// class UIelementLong (handling both Long and Long Arrays
// ***************************************************************************
UIelementLong::UIelementLong(int id):UIelement	(id){};
UIelementLong::~UIelementLong(void){};
void UIelementLong::init(std::vector<long> lMin,std::vector<long> lMax, std::vector<long> lInc, std::vector<long> lDefault)
{
	m_lMin=lMin;
	m_lMax=lMax;
	m_lInc=lInc;
	m_lDefault=lDefault;
};

// ***************************************************************************
// class UIelementDouble (handling both Double and Double Arrays
// ***************************************************************************
UIelementDouble::UIelementDouble(int id):UIelement	(id){};
UIelementDouble::~UIelementDouble(void){};
void UIelementDouble::init(std::vector<double> dMin,std::vector<double> dMax, std::vector<double> dInc, std::vector<double> dDefault)
{
	m_dMin=dMin;
	m_dMax=dMax;
	m_dInc=dInc;
	m_dDefault=dDefault;
	//m_dMin.push_back(1.0);//it works
	//m_dMin={1.0,2.0,3.0};	// it doesn't work
};

// ***************************************************************************
// class UIelementSelectionBox 
// ***************************************************************************
UIelementSelectionBox::UIelementSelectionBox(int id):UIelement	(id){};
UIelementSelectionBox::~UIelementSelectionBox(void){};
void UIelementSelectionBox::init(std::vector<std::string> lOptions)
{
	m_sOptions=lOptions;
};





