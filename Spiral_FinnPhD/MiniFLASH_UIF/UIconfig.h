/*! 
***************************************************************************


***************************************************************************
*/
#ifndef UIConfig_h
/// multiple include guard
#define UIConfig_h 1

#include <vector>
#include <string>

// ***************************************************************************
// class UIConfig
// ***************************************************************************
class UIConfig
{
public:
	UIConfig(int id);
	~UIConfig(void);
	virtual bool search(int id);
	virtual UIConfig* returnElementByID(int id);
	int getREGindex();
	int m_lREGindex;
};

// ***************************************************************************
// class UIelementSet
// ***************************************************************************
class UIelementSet: public UIConfig
{
public: 
	UIelementSet(int id);
	~UIelementSet(void);
	void addElement(UIConfig *a);
   	virtual bool search(int id);
	virtual UIConfig* returnElementByID(int id);
	virtual bool clearElements(void);
	std::vector<UIConfig*> myElements;
};

// ***************************************************************************
// class UIelement
// ***************************************************************************
class UIelement: public UIConfig
{
public: 
	UIelement(int id);
	~UIelement(void);
	//virtual UIConfig* returnElementByID(int id);

//protected:
	char* m_pLabel;
	bool m_bIsAvailable;
	long m_lWMBindex;	// will be enumerators defined in UI_Link
	//long m_lREGindex; // it specifies the position in the Card, but also provides an unique ID for the current Config object
	//long m_isType; // long, double, array long, check box, etc... will be an anumerator defined in UI_Link
	long m_lArraySize; // in case the element is an array
 };

// ***************************************************************************
// class UIelementLong
// ***************************************************************************
class UIelementLong: public UIelement
{
public: 
	UIelementLong(int id);
	~UIelementLong(void);
	void init(std::vector<long> lMin,std::vector<long> lMax, std::vector<long> lInc, std::vector<long> lDefault);
//protected:
	std::vector<long> m_lMin, m_lMax, m_lInc, m_lDefault;
 };

// ***************************************************************************
// class UIelementDouble
// ***************************************************************************
class UIelementDouble: public UIelement
{
public: 
	UIelementDouble(int id);
	~UIelementDouble(void);
	void init(std::vector<double> dMin,std::vector<double> dMax, std::vector<double> dInc, std::vector<double> dDefault);
//protected:
	std::vector<double> m_dMin,  m_dMax, m_dInc, m_dDefault;
 };


// ***************************************************************************
// class UIelementSelectionBox
// ***************************************************************************
class UIelementSelectionBox: public UIelement
{
public: 
	UIelementSelectionBox(int id);
	~UIelementSelectionBox(void);
	void init(std::vector<std::string> sOptions);
//protected:
	std::vector<std::string> m_sOptions;
	long m_lNofOptions; 
	long m_lDefault;
 };


#endif     

 