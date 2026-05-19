// -------------------------------------------------------------------------
// file: UIF_UILink.h
//
// FF 29-January-2021
// -------------------------------------------------------------------------

#include "ProtBasic/Interfaces/SeqLim.h" //SeqLim
#include "MrServers/MrImaging/libSeqSysProp/SysProperties.h"  //MrProt     
#include "UIF_UILink.h"
#include "UIConfigurator.h"

UIelementSet GLOBAL_UI_ELEMENTSET(UIF_AN_INDEX_FOR_THE_GLOBAL_UIELMSET); 

// we assign a WIPMEMBLOCK memory index here, just to make it more flexible than just by using enums
// although using enum is the most elegant choice, easily readable, we have to note that in this way we 
// can also use the 0 index in WMB, 
const long UIF_UI::WMB_DoubleElementForGradScheme=UIF_JUST_AN_INDEX_FOR_A_DOBLE_ELEMENT;



#ifdef WIN32 // only necessary for non-MPCU- processors, since this UI stuff

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    LONG  FIELDS  UIF        **************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+ ***************************************************************************
// return the label of the long box
//+ ***************************************************************************
unsigned _UIF_LONG_GetLabelId(LINK_LONG_TYPE* const _this, char* arg_list[], long lIndex)
{
	static const char* const pszLabel		= "Long STD (->a card)";
	static const char* const pszLabelHaeh	= "NotSupported";
	
	if(lIndex==UIF_UI::REG_LongElement3)
    {
		arg_list[0] = (char*)pszLabel;	
	 }
	else
	{
		arg_list[0] =	static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_pLabel;	
	}
	return MRI_STD_STRING;
}
//+ ***************************************************************************
// return the unit of the long box
//+ ***************************************************************************
unsigned _UIF_LONG_GetUnitId(LINK_LONG_TYPE* const _this, char* arg_list[], long lIndex)
{
	static const char* const pszLabel		= "[a.u.]";
	static const char* const pszLabelHaeh	= "NotSupported";

	if(lIndex==UIF_UI::REG_LongElement3)
    {
		arg_list[0] = (char*)pszLabel;	
	 }
	else
	{
		arg_list[0] = (char*)pszLabel;	
	}
	return MRI_STD_STRING;
}
//+ ***************************************************************************
// return whether long box is visible
//+ ***************************************************************************
bool _UIF_LONG_IsAvailable(LINK_LONG_TYPE* const _this, long lIndex)
{
	if(lIndex==UIF_UI::REG_LongElement3)
    {
		 return true; 
	 }
	else
	{
		return static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_bIsAvailable; 
	}
}
//+ ***************************************************************************
// define the limits for the long parameters
//+ ***************************************************************************
bool _UIF_LONG_GetLimits(LINK_LONG_TYPE* const /*_this*/, std::vector<MrLimitLong>& rLimitVector, unsigned long& rulVerify, long lIndex)
{
    long dMin, dMax, dInc;
     
	if(lIndex==UIF_UI::REG_LongElement3)
    {
		dMin = 10;	
		dMax = 20;	
		dInc = 1;	
	 }
	else
	{
		dMin = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lMin[0]);
		dMax = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lMax[0]);
		dInc = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lInc[0]);
	}

    rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
    
    rLimitVector.resize(1);
    rLimitVector[0].setEqualSpaced(dMin,dMax,dInc);
    return true;
}
//+ ***************************************************************************
// return the current value in the long box
//+ ***************************************************************************
long _UIF_LONG_GetValue(LINK_LONG_TYPE* const _this, long lIndex)
{
     if(lIndex==UIF_UI::REG_LongElement3)
    {
		 return _this->prot().getsWipMemBlock().getalFree()[UIF_UI::WMB_LongElementForGradScheme3];
	 }
	 else
	 {
		 return _this->prot().getsWipMemBlock().getalFree()[ (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)];
	 }
}
//+ ***************************************************************************
// set the current value in the long box
//+ ***************************************************************************
long _UIF_LONG_SetValue(LINK_LONG_TYPE* const _this, long value, long lIndex)
{
     if(lIndex==UIF_UI::REG_LongElement3)
    {
		 return _this->prot().getsWipMemBlock().getalFree()[UIF_UI::WMB_LongElementForGradScheme3] = value; 
	 }
	else
	{
		return _this->prot().getsWipMemBlock().getalFree()[(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)] = value; 
	}
}
//+ ***************************************************************************
// give the ToolTip text for the long box
//+ ***************************************************************************
unsigned _UIF_LONG_GetToolTipId(LINK_LONG_TYPE* const /*pThis*/, char* arg_list[], long lIndex)
{
	static char tLine[100];
	static char tToolTip[1000];
	tToolTip[0] = '\0';

	switch(lIndex)
	{
	case UIF_UI::REG_LongElement1 :
		sprintf(tLine, "MrUILink<LONG> Elements in WipMemBlock are 64");
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
		break;
	case UIF_UI::REG_LongElement2 :
		sprintf(tLine, "MrUILink<LONG> Elements in WipMemBlock are 64");
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
		break;
	case UIF_UI::REG_LongElement3 :
		sprintf(tLine, "MrUILink<LONG> Elements in WipMemBlock are 64");
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
		break;
	default : break;
	}
	return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    LONG  FIELDS FRANCESCA   **************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+ ***************************************************************************
// return the label of the long box
//+ ***************************************************************************
unsigned _FRA_LONG_GetLabelId(LINK_LONG_TYPE* const _this, char* arg_list[], long lIndex)
{
	static const char* const pszLabel1		= "Grad Duration";
	static const char* const pszLabel2		= "# Grad Directions";
	static const char* const pszLabel3		= "# Grad Amplitudes";
	static const char* const pszLabelHaeh	= "NotSupported";
	
	arg_list[0] =	static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_pLabel;
  
	return MRI_STD_STRING;
}
//+ ***************************************************************************
// return the unit of the long box
//+ ***************************************************************************
unsigned _FRA_LONG_GetUnitId(LINK_LONG_TYPE* const _this, char* arg_list[], long lIndex)
{
	static const char* const pszLabel		= "[a.u.]";
	static const char* const pszLabelHaeh	= "NotSupported";

	arg_list[0] = (char*)pszLabel;	

	return MRI_STD_STRING;
}
//+ ***************************************************************************
// return whether long box is visible
//+ ***************************************************************************
bool _FRA_LONG_IsAvailable(LINK_LONG_TYPE* const _this, long lIndex)
{
  return true;
}
//+ ***************************************************************************
// define the limits for the long parameters
//+ ***************************************************************************
bool _FRA_LONG_GetLimits(LINK_LONG_TYPE* const /*_this*/, std::vector<MrLimitLong>& rLimitVector, unsigned long& rulVerify, long lIndex)
{
    long dMin, dMax, dInc;
    
	dMin = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lMin[0]);
	dMax = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lMax[0]);
	dInc = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lInc[0]);

    rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
    
    rLimitVector.resize(1);
    rLimitVector[0].setEqualSpaced(dMin,dMax,dInc);
    return true;
}
//+ ***************************************************************************
// return the current value in the long box
//+ ***************************************************************************
long _FRA_LONG_GetValue(LINK_LONG_TYPE* const _this, long lIndex)
{
    return _this->prot().getsWipMemBlock().getalFree()[(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)];
}

//+ ***************************************************************************
// set the current value in the long box
//+ ***************************************************************************
long _FRA_LONG_SetValue(LINK_LONG_TYPE* const _this, long value, long lIndex)
{
    return _this->prot().getsWipMemBlock().getalFree()[(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)] = value; 
}
//+ ***************************************************************************
// give the ToolTip text for the long box
//+ ***************************************************************************
unsigned _FRA_LONG_GetToolTipId(LINK_LONG_TYPE* const /*pThis*/, char* arg_list[], long lIndex)
{
	static char tLine[100];
	static char tToolTip[1000];
	tToolTip[0] = '\0';

	switch(lIndex)
	{
	case UIF_UI::REG_grad_dur :
		sprintf(tLine, "MrUILink<LONG> Elements in WipMemBlock are 64");
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
		break;
	case UIF_UI::REG_ndir :
		sprintf(tLine, "MrUILink<LONG> Elements in WipMemBlock are 64");
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
		break;
	case UIF_UI::REG_ngval :
		sprintf(tLine, "MrUILink<LONG> Elements in WipMemBlock are 64");
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
		break;
	default : break;
	}
	return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    DOUBLE  FIELDS  ************************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+ ***************************************************************************
//+ The double field in the Sequence/Special card
//+    return the label of the double box
//+ ***************************************************************************
unsigned _UIF_DOUBLE_GetLabelId(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex)
{
    static const char* const pszLabel1        = "Double Element 1";   
    static const char* const pszLabel2        = "Double Element 2";
	static const char* const pszLabelHaeh     = "NotSupported";
    switch(lIndex) // depending on the Index of available boxes give them a name 
    {
        case UIF_UI::REG_DoubleElement		: arg_list[0] = (char*)pszLabel1     ; break;
		default								: arg_list[0] = (char*)pszLabelHaeh  ; break;
    }
    return MRI_STD_STRING;
}
//+ ***************************************************************************
//+    return the unit of the double box
//+ ***************************************************************************
unsigned _UIF_DOUBLE_GetUnitId(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex)
{
    static const char* const pszLabel1        = "[a.u.]";   //"Presat gap"
    static const char* const pszLabel2        = "[a.u.]";
	static const char* const pszLabelHaeh     = "NotSupported";

    switch(lIndex) // depending on the Index of available double boxes, set the correct text behind them
    {
        case UIF_UI::REG_DoubleElement		: arg_list[0] = (char*)pszLabel1     ; break;
		default						    	: arg_list[0] = (char*)pszLabelHaeh  ; break;
    }
    return MRI_STD_STRING;
}
//+ ***************************************************************************
//+    return, whether double box is visible
//+ ***************************************************************************
bool _UIF_DOUBLE_IsAvailable(LINK_DOUBLE_TYPE* const _this, long lIndex)
{
  return true;
}
//+ ***************************************************************************
//+    Define the limits for the values in the double box 
//+ ***************************************************************************
bool _UIF_DOUBLE_GetLimits(LINK_DOUBLE_TYPE* const /*_this*/, std::vector<MrLimitDouble>& rLimitVector, unsigned long& rulVerify, long lIndex)
{
    double dMin, dMax, dInc;
    
    switch(lIndex)
    {
        case UIF_UI::REG_DoubleElement		: dMin=  1280.0	; dMax=  30720.0; dInc=1280.0;	break;
		default								: dMin= 0.0	    ; dMax=  0.0	; dInc=1.000;	break;
    }
    
    rulVerify = LINK_DOUBLE_TYPE::VERIFY_BINARY_SEARCH;
    
    rLimitVector.resize(1);
    rLimitVector[0].setEqualSpaced(dMin,dMax,dInc);
    return true;
}
//+ ***************************************************************************
//+    return the current value in the double box 
//+ ***************************************************************************
double _UIF_DOUBLE_GetValue(LINK_DOUBLE_TYPE* const _this, long lIndex)
{
    //return _this->prot().getsWipMemBlock().getadFree()[(long)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)];
	return  _this->prot().getsWipMemBlock().getadFree()[UIF_UI::WMB_DoubleElementForGradScheme];
}
//+ ***************************************************************************
//+    set the current value in the double box 
//+ ***************************************************************************
double _UIF_DOUBLE_SetValue(LINK_DOUBLE_TYPE* const _this, double value, long lIndex)
{
    //return _this->prot().getsWipMemBlock().getadFree()[(long)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)] = value; 
    return _this->prot().getsWipMemBlock().getadFree()[UIF_UI::WMB_DoubleElementForGradScheme] = value; 
	
}
//+ ***************************************************************************
//+    give the ToolTip text for the double array box
//+ ***************************************************************************
unsigned _UIF_DOUBLE_GetToolTipId(LINK_DOUBLE_TYPE* const /*pThis*/, char* arg_list[], long lIndex)
{
    static char tLine[100];
    static char tToolTip[1000];
    tToolTip[0] = '\0';
    switch (lIndex)
    {
	case UIF_UI::REG_DoubleElement :
          sprintf(tLine,"MrUILink<DOUBLE> Elements in WipMemBlock are 16"); strcat(tToolTip,tLine);
          arg_list[0] = tToolTip;
          return MRI_STD_STRING;
          break;
	default : break;
    }
    return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    SELECTION BOX 1 ************************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// label ID for selection box
const char* const pszSelection_1 = "TrapGrad";
const char* const pszSelection_2 = "FWFGrad";
const char* const pszSelection_3 = "OscGrad";
const char* const pszInvalid     = "Invalid";
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetLabelId
//+  Description : Selects the text to be placed in front of selection boxes
//+ ***************************************************************************
unsigned _UIF_SELECTION_GetLabelId(LINK_SELECTION_TYPE* const _this, char* arg_list[], long lIndex)
{
	static const char pszLabelSelectionBox[]	= "Grad Scheme";
    static const char pszInvalid[]				= "Invalid";

    switch (lIndex)
    {
		case UIF_UI::REG_Selection_Box_GradScheme:	arg_list[0] = (char*) pszLabelSelectionBox; break;
        default:									arg_list[0] = (char*) pszInvalid           ; break;
    }
    return MRI_STD_STRING;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_Format
//+  Description : Selects the text of the alternatives in the selection boxes
//+ ***************************************************************************
int _UIF_SELECTION_Format(LINK_SELECTION_TYPE* const, unsigned nID, char* arg_list[], long lIndex)
{
    unsigned uVal = GET_MODIFIER(nID);

    if (lIndex == UIF_UI::REG_Selection_Box_GradScheme)
    {
        switch(uVal)
        {
			case UIF_UI::UIF_TrapGrad:			arg_list[0] = (char*) pszSelection_1; break;
			case UIF_UI::UIF_FWFGrad:			arg_list[0] = (char*) pszSelection_2; break;
			case UIF_UI::UIF_OscGrad:			arg_list[0] = (char*) pszSelection_3; break;
		}
    } 
	return 1;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetValue
//+  Description : Get the value of the selection box with ID lIndex
//+ ***************************************************************************
unsigned _UIF_SELECTION_GetValue(LINK_SELECTION_TYPE* const pThis, long lIndex)
{
    unsigned nRet = MRI_STD_STRING;
    SET_MODIFIER(nRet,(unsigned char)(pThis->prot().getsWipMemBlock().getalFree()[lIndex]));
    return nRet;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetOptions
//+  Description : Get the possible options of Selection box with ID lIndex
//+ ***************************************************************************
bool _UIF_SELECTION_GetOptions(LINK_SELECTION_TYPE* const, std::vector<unsigned>& rOptionVector, unsigned long& rulVerify, long lIndex)
{
	// we could manage different choices for the twqo different pulses
	if (lIndex == UIF_UI::REG_Selection_Box_GradScheme)
    {
        rulVerify = LINK_SELECTION_TYPE::VERIFY_ON;
        rOptionVector.resize(3);
        for (long lI=0; lI<3; lI++) {rOptionVector[lI] = MRI_STD_STRING;};
        SET_MODIFIER(rOptionVector[0],UIF_UI::UIF_TrapGrad);
        SET_MODIFIER(rOptionVector[1],UIF_UI::UIF_FWFGrad);
		SET_MODIFIER(rOptionVector[2],UIF_UI::UIF_OscGrad);
		return true;
    } 
	else 
		return false;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_SetValue
//+  Description : Set a specified value to the selection box lIndex
//+ ***************************************************************************
unsigned _UIF_SELECTION_SetValue(LINK_SELECTION_TYPE* const pThis, unsigned nNewVal, long lIndex)
{
	pThis->prot().getsWipMemBlock().getalFree()[lIndex]=GET_MODIFIER(nNewVal);
    return pThis->value(lIndex);
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    SELECTION BOX 2 ************************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// label ID for selection box
const char* const pszSelection_4 = "Grad";
const char* const pszSelection_5 = "VAPOR";
const char* const pszSelection_6 = "Optim";
const char* const pszInvalid2    = "Invalid";
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetLabelId
//+  Description : Selects the text to be placed in front of selection boxes
//+ ***************************************************************************
unsigned _UIF_SELECTION2_GetLabelId(LINK_SELECTION_TYPE* const _this, char* arg_list[], long lIndex)
{
	static const char pszLabelSelectionBox[]	= "Extra Card";
    static const char pszInvalid2[]				= "Invalid";

    switch (lIndex)
    {
		case UIF_UI::REG_Selection_Box_ExtraCard:	arg_list[0] = (char*) pszLabelSelectionBox; break;
        default:									arg_list[0] = (char*) pszInvalid           ; break;
    }
    return MRI_STD_STRING;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_Format
//+  Description : Selects the text of the alternatives in the selection boxes
//+ ***************************************************************************
int _UIF_SELECTION2_Format(LINK_SELECTION_TYPE* const, unsigned nID, char* arg_list[], long lIndex)
{
    unsigned uVal = GET_MODIFIER(nID);

    if (lIndex == UIF_UI::REG_Selection_Box_ExtraCard)
    {
        switch(uVal)
        {
			case UIF_UI::UIF_ExtraCardGrad:		arg_list[0] = (char*) pszSelection_4; break;
			case UIF_UI::UIF_ExtraCardVAPOR:	arg_list[0] = (char*) pszSelection_5; break;
			case UIF_UI::UIF_ExtraCardOptim:	arg_list[0] = (char*) pszSelection_6; break;
		}
    } 
	return 1;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetValue
//+  Description : Get the value of the selection box with ID lIndex
//+ ***************************************************************************
unsigned _UIF_SELECTION2_GetValue(LINK_SELECTION_TYPE* const pThis, long lIndex)
{
    unsigned nRet = MRI_STD_STRING;
    SET_MODIFIER(nRet,(unsigned char)(pThis->prot().getsWipMemBlock().getalFree()[lIndex]));
    return nRet;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetOptions
//+  Description : Get the possible options of Selection box with ID lIndex
//+ ***************************************************************************
bool _UIF_SELECTION2_GetOptions(LINK_SELECTION_TYPE* const, std::vector<unsigned>& rOptionVector, unsigned long& rulVerify, long lIndex)
{
	if (lIndex == UIF_UI::REG_Selection_Box_ExtraCard)
    {
        rulVerify = LINK_SELECTION_TYPE::VERIFY_ON;
        rOptionVector.resize(3);
        for (long lI=0; lI<3; lI++) {rOptionVector[lI] = MRI_STD_STRING;};
        SET_MODIFIER(rOptionVector[0],UIF_UI::UIF_ExtraCardGrad);
        SET_MODIFIER(rOptionVector[1],UIF_UI::UIF_ExtraCardVAPOR);
		SET_MODIFIER(rOptionVector[2],UIF_UI::UIF_ExtraCardOptim);
		return true;
    } 
	else 
		return false;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_SetValue
//+  Description : Set a specified value to the selection box lIndex
//+ ***************************************************************************
unsigned _UIF_SELECTION2_SetValue(LINK_SELECTION_TYPE* const pThis, unsigned nNewVal, long lIndex)
{
	pThis->prot().getsWipMemBlock().getalFree()[lIndex]=GET_MODIFIER(nNewVal);
    return pThis->value(lIndex);
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    ARRAY LONG 1 *****************************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+ ***************************************************************************
//+ The array of long values in the Sequence/Special card

//+  _WIP_LONG_ARRAY_MAXSize
//+  Description : returns the maximum array size of the long array box
long _WIP_LONG_ARRAY1_MAXSize(MrUILinkArray* const, long)
{
	return UIF_LONGARRAY1_MAXSIZE;
}
//+  _WIP_LONG_ARRAY_CurrentSize
//+  Description : returns the current array size of the long array box
long _WIP_LONG_ARRAY1_CurrentSize(MrUILinkArray* const, long)
{
	long c=(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lArraySize);// this one changes depending on what array we show
	return c;

}
//+  return the label of the long array box
unsigned _WIP_LONG_ARRAY1_GetLabelId(LINK_LONG_TYPE* const, char* arg_list[], long lIndex)
{
	// the following one works, instead the std way doesn't, because it uses tatic const char* const pszLong1, that cannot change (!)
	const char* const pszLong1= static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_pLabel;	                 
	static char tLine[100];
	sprintf(tLine,"%s [%ld]",pszLong1,lIndex+1);
	arg_list[0] = tLine;

	return MRI_STD_STRING;
}
//+  return the unit of the long array box
unsigned _WIP_LONG_ARRAY1_GetUnitId(LINK_LONG_TYPE* const, char* arg_list[], long)
{
	static const char* const pszLong1 = "[a.u.]";
	arg_list[0] = (char*) pszLong1;
	return MRI_STD_STRING;
}
//+  return, whether long array box is visible
 bool _WIP_LONG_ARRAY1_IsAvailable(LINK_LONG_TYPE* const /* pThis */, long lIndex)
{
	//return true;
 	return static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_bIsAvailable;
}
 //+  return, whether long array box is visible. IT WORKS FOR THE LABEL. REMEMBER THE ARRAY OBJECTS ARE MADE OF TWO MrUILink Objetcs!!!!
 bool _WIP_LONG_ARRAY1_IsAvailableTheLabel(MrUILinkArray* const /* pThis */, long lIndex)
{
	//return true;
 	return static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_bIsAvailable;
}
//+  Define the limits for the values in the double array box
bool _WIP_LONG_ARRAY1_GetLimits(LINK_LONG_TYPE* const /* pThis */, std::vector<MrLimitLong>& rLimitVector, unsigned long& rulVerify, long lIndex)
{
	long dMin, dMax, dInc;
	dMin = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lMin[lIndex]);
	dMax = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lMax[lIndex]);
	dInc = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lInc[lIndex]);

	rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
	rLimitVector.resize(1);
	rLimitVector[0].setEqualSpaced(dMin,dMax,dInc);
	return true;
}
//+  return the current value in the long array box
long _WIP_LONG_ARRAY1_GetValue(LINK_LONG_TYPE* const pThis, long lIndex)
{
	if (lIndex < (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lArraySize) )
	{
		//return pThis->prot().getsWipMemBlock().getalFree()[      UIF_UI::WMB_GvalueLongArray_Start+lIndex];
		return pThis->prot().getsWipMemBlock().getalFree()[ (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lWMBindex) + lIndex];
	}
    else
    {
		return -1.0;
    }
}
//+  set the current value in the long array box
long _WIP_LONG_ARRAY1_SetValue(LINK_LONG_TYPE* const pThis, long value, long lIndex)
{
	if (lIndex < (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lArraySize))
	{
		//return (pThis->prot().getsWipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+lIndex] = value);
		return pThis->prot().getsWipMemBlock().getalFree()[ (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lWMBindex) + lIndex]=value;
	}
	else
	{
		return -1.0;
	}
}
//+  give the ToolTip text for the long array box
 unsigned _WIP_LONG_ARRAY1_GetToolTipId(LINK_LONG_TYPE* const /* pThis */, char* arg_list[], long lIndex)
{
	static char tLine    [100];
	static char tToolTip[1000];
	tToolTip[0] = '\0';
	
	if (lIndex < (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray1Element))->m_lArraySize))
	{
		sprintf(tLine,"tooltip for long array element %ld",lIndex+1);
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
	}
	return 0;
 }

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    ARRAY LONG 2 *****************************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+ ***************************************************************************
//+ The array of long values in the Sequence/Special card

//+  _WIP_LONG_ARRAY_MAXSize
//+  Description : returns the maximum array size of the long array box
long _WIP_LONG_ARRAY2_MAXSize(MrUILinkArray* const, long)
{
	return UIF_LONGARRAY2_MAXSIZE;// THIS IS FIXED, MAXIMUM VALUE
}
//+  _WIP_LONG_ARRAY_CurrentSize
//+  Description : returns the current array size of the long array box
long _WIP_LONG_ARRAY2_CurrentSize(MrUILinkArray* const, long)
{
	long c=(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lArraySize);// this one changes depending on what array we show
	return c;
}
//+  return the label of the long array box
unsigned _WIP_LONG_ARRAY2_GetLabelId(LINK_LONG_TYPE* const, char* arg_list[], long lIndex)
{
	// the following one works, instead the std way doesn't, because it uses tatic const char* const pszLong1, that cannot change (!)
	const char* const pszLong1= static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_pLabel;	                 
	static char tLine[100];
	sprintf(tLine,"%s [%ld]",pszLong1,lIndex+1);
	arg_list[0] = tLine;
	//arg_list[0]=static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_pLabel;// this does not add the array index
	return MRI_STD_STRING;
}
//+  return the unit of the long array box
unsigned _WIP_LONG_ARRAY2_GetUnitId(LINK_LONG_TYPE* const, char* arg_list[], long)
{
	static const char* const pszLong1 = "[a.u.]";
	arg_list[0] = (char*) pszLong1;
	return MRI_STD_STRING;
}
//+  return, whether long array box is visible
 bool _WIP_LONG_ARRAY2_IsAvailable(LINK_LONG_TYPE* const /* pThis */, long lIndex)
{
	bool b=static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_bIsAvailable;
	return b;
}
  //+  return, whether long array box is visible. IT WORKS FOR THE LABEL. REMEMBER THE ARRAY OBJECTS ARE MADE OF TWO MrUILink Objetcs!!!!
 bool _WIP_LONG_ARRAY2_IsAvailableTheLabel(MrUILinkArray* const /* pThis */, long lIndex)
{
 	return static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_bIsAvailable;
}
//+  Define the limits for the values in the double array box
bool _WIP_LONG_ARRAY2_GetLimits(LINK_LONG_TYPE* const /* pThis */, std::vector<MrLimitLong>& rLimitVector, unsigned long& rulVerify, long lIndex)
{
	long dMin, dMax, dInc;
	dMin = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lMin[lIndex]);
	dMax = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lMax[lIndex]);
	dInc = (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lInc[lIndex]);

	rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
	rLimitVector.resize(1);
	rLimitVector[0].setEqualSpaced(dMin,dMax,dInc);
	return true;
}
//+  return the current value in the long array box
long _WIP_LONG_ARRAY2_GetValue(LINK_LONG_TYPE* const pThis, long lIndex)
{
	if (lIndex < (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lArraySize))
	{
		//return pThis->prot().getsWipMemBlock().getalFree()[UIF_UI::WMB_GdirLongArray_Start+lIndex];
		return pThis->prot().getsWipMemBlock().getalFree()[ (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lWMBindex) + lIndex];
	}
    else
    {
    return -1.0;
    }
}
//+  set the current value in the long array box
long _WIP_LONG_ARRAY2_SetValue(LINK_LONG_TYPE* const pThis, long value, long lIndex)
{
	if (lIndex < (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lArraySize))
	{
		return pThis->prot().getsWipMemBlock().getalFree()[ (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lWMBindex) + lIndex]=value;
	}
	else
	{
		return -1.0;
	}
}
//+  give the ToolTip text for the long array box
 unsigned _WIP_LONG_ARRAY2_GetToolTipId(LINK_LONG_TYPE* const /* pThis */, char* arg_list[], long lIndex)
{
	static char tLine    [100];
	static char tToolTip[1000];
	tToolTip[0] = '\0';
	
	if (lIndex < (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongArray2Element))->m_lArraySize))
	{
		sprintf(tLine,"tooltip for long array element %ld",lIndex+1);
		strcat(tToolTip,tLine);
		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
	}
	return 0;
 }


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    ARRAY DOUBLE *****************************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+  _WIP_DOUBLE_ARRAY_MAXSize
//+  Description : returns the maximum array size of the double array box
long _WIP_DOUBLE_ARRAY_MAXSize(MrUILinkArray* const, long)
{
	return UIF_DOUBLEARRAY_MAXSIZE;
}
//+  _WIP_DOUBLE_ARRAY_CurrentSize
//+  Description : returns the current array size of the double array box
long _WIP_DOUBLE_ARRAY_CurrentSize(MrUILinkArray* const, long)
{
	return (long)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_lArraySize);
}
//+  return the label of the double array box
unsigned _WIP_DOUBLE_ARRAY_GetLabelId(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex)
{
	const char* const pszLong1= static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_pLabel;	                 
	static char tLine[100];
	sprintf(tLine,"%s [%ld]",pszLong1,lIndex+1);
	arg_list[0] = tLine;
	return MRI_STD_STRING;

}
//+  return the unit of the double array box
unsigned _WIP_DOUBLE_ARRAY_GetUnitId(LINK_DOUBLE_TYPE* const, char* arg_list[], long)
{
	static const char* const pszDouble1 = "[a.u.]";
	arg_list[0] = (char*) pszDouble1;
	return MRI_STD_STRING;
}
//+  return, whether double array box is visible
bool _WIP_DOUBLE_ARRAY_IsAvailable(LINK_DOUBLE_TYPE* const /* pThis */, long)
{
	bool b=static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_bIsAvailable;
	return b;
}
//+  Define the limits for the values in the double array box
bool _WIP_DOUBLE_ARRAY_GetLimits(LINK_DOUBLE_TYPE* const /* pThis */, std::vector<MrLimitDouble>& rLimitVector, unsigned long& rulVerify, long lIndex)
{
	double dMin, dMax, dInc;
	dMin = (double)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_dMin[lIndex]);
	dMax = (double)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_dMax[lIndex]);
	dInc = (double)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_dInc[lIndex]);

	rulVerify = LINK_DOUBLE_TYPE::VERIFY_BINARY_SEARCH;

	rLimitVector.resize(1);
	rLimitVector[0].setEqualSpaced(dMin,dMax,dInc);
	return true;
}
//+  return the current value in the double array box
double _WIP_DOUBLE_ARRAY_GetValue(LINK_DOUBLE_TYPE* const pThis, long lIndex)
{
	if (lIndex < UIF_DOUBLEARRAY_MAXSIZE)
    {
		return pThis->prot().getsWipMemBlock().getalFree()[ (long)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_lWMBindex) + lIndex];
	}
	else
	{
		return -1.0;
	}
}
//+  set the current value in the double array box
 double _WIP_DOUBLE_ARRAY_SetValue(LINK_DOUBLE_TYPE* const pThis, double value, long lIndex)
{
	if (lIndex < UIF_DOUBLEARRAY_MAXSIZE)
	{
		return (pThis->prot().getsWipMemBlock().getadFree()[(long)(static_cast<UIelementDouble*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_DoubleArrayElement))->m_lWMBindex)+lIndex] = value);
	
	}
		else
	{
			return -1.0;
	}
}
/*//+  define Solve Handlers (though here not needed) for the double array box   
unsigned _WIP_DOUBLE_ARRAY_Solve(LINK_DOUBLE_TYPE* const pThis, char* arg_list[], const void* pVoid, const MrProtocolData::MrProtData* pOrigProt, long lIndex)
{
	return fUICSolveDoubleParamConflict(pThis,arg_list,pVoid,pOrigProt,lIndex,NULL,NULL,NULL);
}*/
//+  give the ToolTip text for the double array box
unsigned _WIP_DOUBLE_ARRAY_GetToolTipId(LINK_DOUBLE_TYPE* const /* pThis */, char* arg_list[], long lIndex)
{
	static char tLine    [100];
	static char tToolTip[1000];

	tToolTip[0] = '\0';

	if (lIndex < UIF_DOUBLEARRAY_MAXSIZE)
	{
		sprintf(tLine,"tooltip for double array element %ld",lIndex+1);
		strcat(tToolTip,tLine);

		arg_list[0] = tToolTip;
		return MRI_STD_STRING;
	}
	return 0;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    CHECK BOX   ****************************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+ [ Function ****************************************************************
//+  Name        : _WIP_CheckBox_GetLabelId
//+  Description : Selects the text to be placed in front of checkboxes
//+ ***************************************************************************
unsigned _UIF_CHECKBOX_GetLabelId(LINK_BOOL_TYPE* const, char* arg_list[], long lIndex)
{
    static const char* const pszLabelCB    = "Multi Purpose SWITCH";
    static const char* const pszLabelHaeh =  "NotSupported";

    switch(lIndex)
    {
		//case UIF_UI::REG_CheckBox	: arg_list[0] = (char*)pszLabelCB  ; break;
		case UIF_UI::REG_CheckBox	: arg_list[0] = static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_pLabel;break;
        default						: arg_list[0] = (char*)pszLabelHaeh; break;
    }
    return MRI_STD_STRING;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_CheckBox_GetOptions
//+  Description : Defines the alternatives for the check box  (true/false)
//+ ***************************************************************************
bool _UIF_CHECKBOX_GetOptions(LINK_BOOL_TYPE* const _this, std::vector<unsigned>& rOptionVector, unsigned long& rulVerify, long lIndex)
{
    rulVerify = LINK_BOOL_TYPE::VERIFY_ON;
    rOptionVector.resize(2);
    rOptionVector[0] = false;
    rOptionVector[1] = true;
    return true;
}
//+  return, whether check box is visible
 bool _UIF_CHECKBOX_IsAvailable(LINK_BOOL_TYPE* const /* pThis */, long lIndex)
{
	return static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_CheckBox))->m_bIsAvailable;
}
//+ [ Function ****************************************************************
//+  Name        : _CheckBox_GetValue
//+  Description : Get the current value from checkbox lIndex
//+ ***************************************************************************
bool _UIF_CHECKBOX_GetValue(LINK_BOOL_TYPE* const _this, long lIndex)
{
	return _this->prot().getsWipMemBlock().getalFree()[(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)] != UIF_UI::UIF_CheckBoxOff;;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_CheckBox_SetValue
//+  Description : Set a value in checkbox lIndex
//+ ***************************************************************************
bool _UIF_CHECKBOX_SetValue(LINK_BOOL_TYPE* const _this, bool value, long lIndex)
{
    switch(lIndex)
    {
		case UIF_UI::REG_CheckBox  : 
			if (value) 
			{
				_this->prot().getsWipMemBlock().getalFree()[(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)] = UIF_UI::UIF_CheckBoxOn;
			} 
			else 
			{
				_this->prot().getsWipMemBlock().getalFree()[(long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_lWMBindex)] = UIF_UI::UIF_CheckBoxOff;
			}
			return true;
		default : break;
    }
	return false;
}


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// ***************************    SELECTION BOX (DYNAMICAL) **************************
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetLabelId
//+  Description : Selects the text to be placed in front of selection boxes
//+ ***************************************************************************
unsigned _UIF_SELECTION_DYN_GetLabelId(LINK_SELECTION_TYPE* const _this, char* arg_list[], long lIndex)
{
	static const char pszInvalid[]				= "Invalid";

    switch (lIndex)
    {
		case UIF_UI::REG_Selection_Box	:	arg_list[0] = static_cast<UIelement*>(GLOBAL_UI_ELEMENTSET.returnElementByID(lIndex))->m_pLabel;break;

		default:							arg_list[0] = (char*) pszInvalid           ; break;
    }
    return MRI_STD_STRING;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_Format
//+  Description : Selects the text of the alternatives in the selection boxes
//+ ***************************************************************************
int _UIF_SELECTION_DYN_Format(LINK_SELECTION_TYPE* const, unsigned nID, char* arg_list[], long lIndex)
{
    unsigned uVal = GET_MODIFIER(nID);

    if (lIndex == UIF_UI::REG_Selection_Box)
    {
		std::string s= static_cast<UIelementSelectionBox*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_Selection_Box))->m_sOptions[uVal];
		char *cstr = new char[s.length() + 1];
		strcpy(cstr, s.c_str());
		arg_list[0]=cstr;
    } 
	return 1;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetOptions
//+  Description : Get the possible options of Selection box with ID lIndex
//+ ***************************************************************************
bool _UIF_SELECTION_DYN_GetOptions(LINK_SELECTION_TYPE* const, std::vector<unsigned>& rOptionVector, unsigned long& rulVerify, long lIndex)
{
	if (lIndex == UIF_UI::REG_Selection_Box)
    {
        rulVerify = LINK_SELECTION_TYPE::VERIFY_ON;
		long lNofOptions=(long)static_cast<UIelementSelectionBox*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_Selection_Box))->m_lNofOptions;
        rOptionVector.resize(lNofOptions);
		for (long lI=0; lI<lNofOptions; lI++) {rOptionVector[lI] = MRI_STD_STRING;};
		for (lI=0; lI<lNofOptions; lI++) {SET_MODIFIER(rOptionVector[lI],lI);};
   		return true;
    } 
	else 
		return false;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_GetValue
//+  Description : Get the value of the selection box with ID lIndex
//+ ***************************************************************************
unsigned _UIF_SELECTION_DYN_GetValue(LINK_SELECTION_TYPE* const pThis, long lIndex)
{
    unsigned nRet = MRI_STD_STRING;
    long l=(long)static_cast<UIelementSelectionBox*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_Selection_Box))->m_lWMBindex;
    SET_MODIFIER(nRet,(unsigned char)(pThis->prot().getsWipMemBlock().getalFree()[l]));
	return nRet;
}
//+ [ Function ****************************************************************
//+  Name        : _WIP_SELECTION_SetValue
//+  Description : Set a specified value to the selection box lIndex
//+ ***************************************************************************
unsigned _UIF_SELECTION_DYN_SetValue(LINK_SELECTION_TYPE* const pThis, unsigned nNewVal, long lIndex)
{
	long l=(long)static_cast<UIelementSelectionBox*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_Selection_Box))->m_lWMBindex;
	pThis->prot().getsWipMemBlock().getalFree()[l]=GET_MODIFIER(nNewVal);		
    return pThis->value(lIndex);
}
//+ ***************************************************************************
//+    return whether string is visible
//+ ***************************************************************************
bool _UIF_SELECTION_DYN_IsAvailable(LINK_SELECTION_TYPE* const _this, long lIndex)
{
  return (bool)static_cast<UIelementSelectionBox*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_Selection_Box))->m_bIsAvailable;
}

//. -----------------------------------------------------------------------------
//. Register Sequence/Special card parameters
//. -----------------------------------------------------------------------------
void _UIF_Register (SeqLim* pSeqLim)
{
	if( LINK_DOUBLE_TYPE* pDouble = _create< LINK_DOUBLE_TYPE >(pSeqLim, UIFPos_DoubleElement, UIF_UI::REG_DoubleElement))
  {
		pDouble->registerGetLabelIdHandler    (_UIF_DOUBLE_GetLabelId    );
		pDouble->registerGetUnitIdHandler     (_UIF_DOUBLE_GetUnitId     );
		pDouble->registerIsAvailableHandler   (_UIF_DOUBLE_IsAvailable   );
		pDouble->registerGetLimitsHandler     (_UIF_DOUBLE_GetLimits     );
		pDouble->registerGetValueHandler      (_UIF_DOUBLE_GetValue      );
		pDouble->registerSetValueHandler      (_UIF_DOUBLE_SetValue      );
		pDouble->registerGetToolTipIdHandler  (_UIF_DOUBLE_GetToolTipId  );
  }
	if (LINK_SELECTION_TYPE* pSelection = _create< LINK_SELECTION_TYPE >(pSeqLim,UIFPos_Selection_Box_GradientScheme, UIF_UI::REG_Selection_Box_GradScheme))
  {
		pSelection->registerGetLabelIdHandler  (_UIF_SELECTION_GetLabelId);
		pSelection->registerGetOptionsHandler  (_UIF_SELECTION_GetOptions);
		pSelection->registerGetValueHandler    (_UIF_SELECTION_GetValue);
		pSelection->registerSetValueHandler    (_UIF_SELECTION_SetValue);
		pSelection->registerFormatHandler      (_UIF_SELECTION_Format);
   } 
	if (LINK_SELECTION_TYPE* pSelection = _create< LINK_SELECTION_TYPE >(pSeqLim,UIFPos_Selection_Box_ExtraCard, UIF_UI::REG_Selection_Box_ExtraCard))
  {
		pSelection->registerGetLabelIdHandler  (_UIF_SELECTION2_GetLabelId);
		pSelection->registerGetOptionsHandler  (_UIF_SELECTION2_GetOptions);
		pSelection->registerGetValueHandler    (_UIF_SELECTION2_GetValue);
		pSelection->registerSetValueHandler    (_UIF_SELECTION2_SetValue);
		pSelection->registerFormatHandler      (_UIF_SELECTION2_Format);
   } 
	if (LINK_SELECTION_TYPE* pSelection = _create< LINK_SELECTION_TYPE >(pSeqLim,UIFPos_Selection_Box, UIF_UI::REG_Selection_Box))
  {
		pSelection->registerGetLabelIdHandler  (_UIF_SELECTION_DYN_GetLabelId);
		pSelection->registerGetOptionsHandler  (_UIF_SELECTION_DYN_GetOptions);
		pSelection->registerGetValueHandler    (_UIF_SELECTION_DYN_GetValue);
		pSelection->registerSetValueHandler    (_UIF_SELECTION_DYN_SetValue);
		pSelection->registerFormatHandler      (_UIF_SELECTION_DYN_Format);
		pSelection->registerIsAvailableHandler (_UIF_SELECTION_DYN_IsAvailable);

   } 

    if (LINK_BOOL_TYPE* pCheckBox = _create< LINK_BOOL_TYPE >(pSeqLim, UIFPos_CheckBox, UIF_UI::REG_CheckBox))
  {
		pCheckBox->registerGetLabelIdHandler  (_UIF_CHECKBOX_GetLabelId);
		pCheckBox->registerGetOptionsHandler  (_UIF_CHECKBOX_GetOptions);
		pCheckBox->registerGetValueHandler    (_UIF_CHECKBOX_GetValue);
		pCheckBox->registerSetValueHandler    (_UIF_CHECKBOX_SetValue);
		pCheckBox->registerIsAvailableHandler (_UIF_CHECKBOX_IsAvailable);
  } 
	if( LINK_LONG_TYPE* pLong = _create< LINK_LONG_TYPE > ( pSeqLim, UIFPos_LongElement1, UIF_UI::REG_LongElement1 ) )
	{
		pLong->registerGetLabelIdHandler	(_UIF_LONG_GetLabelId		);
		pLong->registerGetUnitIdHandler		(_UIF_LONG_GetUnitId		);
		pLong->registerIsAvailableHandler	(_UIF_LONG_IsAvailable		);
		pLong->registerGetLimitsHandler		(_UIF_LONG_GetLimits		);
		pLong->registerGetValueHandler		(_UIF_LONG_GetValue		);
		pLong->registerSetValueHandler		(_UIF_LONG_SetValue		);
		pLong->registerGetToolTipIdHandler	(_UIF_LONG_GetToolTipId	);
	}
	if( LINK_LONG_TYPE* pLong = _create< LINK_LONG_TYPE > ( pSeqLim, UIFPos_LongElement2, UIF_UI::REG_LongElement2 ) )
	{
		pLong->registerGetLabelIdHandler	(_UIF_LONG_GetLabelId		);
		pLong->registerGetUnitIdHandler		(_UIF_LONG_GetUnitId		);
		pLong->registerIsAvailableHandler	(_UIF_LONG_IsAvailable		);
		pLong->registerGetLimitsHandler		(_UIF_LONG_GetLimits		);
		pLong->registerGetValueHandler		(_UIF_LONG_GetValue		);
		pLong->registerSetValueHandler		(_UIF_LONG_SetValue		);
		pLong->registerGetToolTipIdHandler	(_UIF_LONG_GetToolTipId	);
	}
	if( LINK_LONG_TYPE* pLong = _create< LINK_LONG_TYPE > ( pSeqLim, UIFPos_LongElement3, UIF_UI::REG_LongElement3 ) )
	{
		pLong->registerGetLabelIdHandler	(_UIF_LONG_GetLabelId		);
		pLong->registerGetUnitIdHandler		(_UIF_LONG_GetUnitId		);
		pLong->registerIsAvailableHandler	(_UIF_LONG_IsAvailable		);
		pLong->registerGetLimitsHandler		(_UIF_LONG_GetLimits		);
		pLong->registerGetValueHandler		(_UIF_LONG_GetValue		);
		pLong->registerSetValueHandler		(_UIF_LONG_SetValue		);
		pLong->registerGetToolTipIdHandler	(_UIF_LONG_GetToolTipId	);
	}
	if( LINK_LONG_TYPE* pLong = _create< LINK_LONG_TYPE > ( pSeqLim, UIFPos_grad_dur, UIF_UI::REG_grad_dur ) )
	{
		pLong->registerGetLabelIdHandler	(_UIF_LONG_GetLabelId		);
		pLong->registerGetUnitIdHandler		(_UIF_LONG_GetUnitId		);
		pLong->registerIsAvailableHandler	(_UIF_LONG_IsAvailable		);
		pLong->registerGetLimitsHandler		(_UIF_LONG_GetLimits		);
		pLong->registerGetValueHandler		(_UIF_LONG_GetValue		);
		pLong->registerSetValueHandler		(_UIF_LONG_SetValue		);
		pLong->registerGetToolTipIdHandler	(_UIF_LONG_GetToolTipId	);
	}
	if( LINK_LONG_TYPE* pLong = _create< LINK_LONG_TYPE > ( pSeqLim, UIFPos_ndir, UIF_UI::REG_ndir ) )
	{
		pLong->registerGetLabelIdHandler	(_UIF_LONG_GetLabelId		);
		pLong->registerGetUnitIdHandler		(_UIF_LONG_GetUnitId		);
		pLong->registerIsAvailableHandler	(_UIF_LONG_IsAvailable		);
		pLong->registerGetLimitsHandler		(_UIF_LONG_GetLimits		);
		pLong->registerGetValueHandler		(_UIF_LONG_GetValue		);
		pLong->registerSetValueHandler		(_UIF_LONG_SetValue		);
		pLong->registerGetToolTipIdHandler	(_UIF_LONG_GetToolTipId	);
	}
	if( LINK_LONG_TYPE* pLong = _create< LINK_LONG_TYPE > ( pSeqLim, UIFPos_ngval, UIF_UI::REG_ngval ) )
	{
		pLong->registerGetLabelIdHandler	(_UIF_LONG_GetLabelId		);
		pLong->registerGetUnitIdHandler		(_UIF_LONG_GetUnitId		);
		pLong->registerIsAvailableHandler	(_UIF_LONG_IsAvailable		);
		pLong->registerGetLimitsHandler		(_UIF_LONG_GetLimits		);
		pLong->registerGetValueHandler		(_UIF_LONG_GetValue		);
		pLong->registerSetValueHandler		(_UIF_LONG_SetValue		);
		pLong->registerGetToolTipIdHandler	(_UIF_LONG_GetToolTipId	);
	}
	// here is the registration of the first array of long parameters
	LINK_LONG_TYPE* pLongElm1 = NULL;
	if( MrUILinkArray* pLongArray = _createArray<LINK_LONG_TYPE>(pSeqLim,UIFPos_LongArray1Element,_WIP_LONG_ARRAY1_MAXSize,pLongElm1) )
	{
		pLongArray->registerSizeHandler(_WIP_LONG_ARRAY1_CurrentSize);
		pLongArray->registerIsAvailableHandler(_WIP_LONG_ARRAY1_IsAvailableTheLabel);// we need 2 functionn handlers for arrays to handle visibility
		
		pLongElm1->registerGetLabelIdHandler  (_WIP_LONG_ARRAY1_GetLabelId  );
		pLongElm1->registerGetUnitIdHandler   (_WIP_LONG_ARRAY1_GetUnitId   );
		pLongElm1->registerIsAvailableHandler (_WIP_LONG_ARRAY1_IsAvailable );		// we need 2 functionn handlers for arrays to handle visibility
		pLongElm1->registerGetLimitsHandler   (_WIP_LONG_ARRAY1_GetLimits   );
		pLongElm1->registerGetValueHandler    (_WIP_LONG_ARRAY1_GetValue    );
		pLongElm1->registerSetValueHandler    (_WIP_LONG_ARRAY1_SetValue    );
		pLongElm1->registerGetToolTipIdHandler(_WIP_LONG_ARRAY1_GetToolTipId);
	}
	
	// here is the registration of the second array of long parameters, we decided to use two separate set of function handlers.
	LINK_LONG_TYPE* pLongElm2 = NULL;
	if( MrUILinkArray* pLongArray = _createArray<LINK_LONG_TYPE>(pSeqLim,UIFPos_LongArray2Element,_WIP_LONG_ARRAY2_MAXSize,pLongElm2) )
	{
		pLongArray->registerSizeHandler(_WIP_LONG_ARRAY2_CurrentSize);
		pLongArray->registerIsAvailableHandler(_WIP_LONG_ARRAY1_IsAvailableTheLabel);// we need 2 functionn handlers for arrays to handle visibility

		pLongElm2->registerGetLabelIdHandler  (_WIP_LONG_ARRAY2_GetLabelId  );
		pLongElm2->registerGetUnitIdHandler   (_WIP_LONG_ARRAY2_GetUnitId   );
		pLongElm2->registerIsAvailableHandler (_WIP_LONG_ARRAY2_IsAvailable );		// we need 2 functionn handlers for arrays to handle visibility
		pLongElm2->registerGetLimitsHandler   (_WIP_LONG_ARRAY2_GetLimits   );
		pLongElm2->registerGetValueHandler    (_WIP_LONG_ARRAY2_GetValue    );
		pLongElm2->registerSetValueHandler    (_WIP_LONG_ARRAY2_SetValue    );
		pLongElm2->registerGetToolTipIdHandler(_WIP_LONG_ARRAY2_GetToolTipId);
	}

    // here is the registration of the array of double parameters
	LINK_DOUBLE_TYPE* pDoubleElm = NULL;
	if( MrUILinkArray* pDoubleArray = _createArray<LINK_DOUBLE_TYPE>(pSeqLim,UIFPos_DoubleArrayElement,_WIP_DOUBLE_ARRAY_MAXSize,pDoubleElm) )
	{
		pDoubleArray->registerSizeHandler(_WIP_DOUBLE_ARRAY_CurrentSize);
		
		pDoubleElm->registerGetLabelIdHandler  (_WIP_DOUBLE_ARRAY_GetLabelId  );
		pDoubleElm->registerGetUnitIdHandler   (_WIP_DOUBLE_ARRAY_GetUnitId   );
		pDoubleElm->registerIsAvailableHandler (_WIP_DOUBLE_ARRAY_IsAvailable );
		pDoubleElm->registerGetLimitsHandler   (_WIP_DOUBLE_ARRAY_GetLimits   );
		pDoubleElm->registerGetValueHandler    (_WIP_DOUBLE_ARRAY_GetValue    );
		pDoubleElm->registerSetValueHandler    (_WIP_DOUBLE_ARRAY_SetValue    );
		//pDoubleElm->registerSolveHandler       (_WIP_DOUBLE_ARRAY_Solve       );
		pDoubleElm->registerGetToolTipIdHandler(_WIP_DOUBLE_ARRAY_GetToolTipId);
	}
}


//. ---------------------------------------------------------------------------
//. Initialize parameters on the Sequence/Special card
//. ---------------------------------------------------------------------------
bool _UIF_Init (MrProt* pMrProt, SeqLim* pSeqLim, AbstractUIConfigurator* pAbstrUIConfig, long lCurrentGradSchemeChoice)
{
  // In this part (only performed on the host) we initialize the values 
  // for the parameters on the Sequence/Special card. 
  // The concept of ContextPrepForBinarySearch and ContextPrepForMrProtUpdate 
  // is used here:
  // When creating the default protocol, fSEQPrep() is called in 
  // ContextPrepForBinarySearch. The parameters are not yet initialized, 
  // hence a SEQU_ERROR is returned. This causes a 2nd call of fSEQPrep(), 
  // this time in ContextPrepForMrProtUpdate,
  // allowing for a modification of the protocol. 
  // Now the parameters can be initialized.
  
  // if called within ContextPrepForMrProtUpdate, we can (and must) modify the protocol. 
  if (pSeqLim->isContextPrepForMrProtUpdate()) 
  {
    if (!pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_Selection_Box_GradScheme])
	{
		// the selection boxes
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_Selection_Box_ExtraCard]		= UIF_UI::UIF_ExtraCardGrad;
		//pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_Selection_Box_GradScheme]	= pAbstrUIConfig->getUIChoice();
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_Selection_Box_GradScheme]	= lCurrentGradSchemeChoice;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GradSchemeSelBox]			= 4;// the default optin for the default Grad Scheme (FWF) UIConfigurator

		// std card WMB elements
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_grad_dur]					= 2200;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_ndir]						= 1;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_ngval]						= 1;		
		
		// PENDING (implemented in the STD way)
		pMrProt->wipMemBlock().getadFree()[UIF_UI::WMB_DoubleElementForGradScheme]	= 7680.0 ; 
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_LongElementForGradScheme3]	= 15;
	
		// Grad Scheme and Grad std EXTRA CARD elements
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_LongElementForGradScheme1]	= (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongElement1))->m_lDefault[0]); 
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_LongElementForGradScheme2]	= (long)(static_cast<UIelementLong*>(GLOBAL_UI_ELEMENTSET.returnElementByID(UIF_UI::REG_LongElement2))->m_lDefault[0]);;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_b0CheckBox]					= UIF_UI::UIF_CheckBoxOff; // THIS WILL GO ON EXTRA CARDS
		pMrProt->wipMemBlock().getadFree()[UIF_UI::WMB_GradDoubleArray_Start+0]		= 1;
		pMrProt->wipMemBlock().getadFree()[UIF_UI::WMB_GradDoubleArray_Start+1]		= 1;//WMB_WSdeoptLongArray_End
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+0]		= 5;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+1]		= 16;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+2]		= 20;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+3]		= 20;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+4]		= 20;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+5]		= 20;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+6]		= 20;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+7]		= 20;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+8]		= 20;//WMB_GvalueLongArray_End
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GvalueLongArray_Start+9]		= 20;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GdirLongArray_Start+0]		= 1;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GdirLongArray_Start+1]		= 2;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GdirLongArray_Start+2]		= 3;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GdirLongArray_Start+3]		= 4;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GdirLongArray_Start+4]		= 5;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_GdirLongArray_Start+5]		= 6;//WMB_GdirLongArray_End
		// OPTIM EXTRA CARD WMB elements
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_MaxDiffGrad]					= 80;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_RiseTime]					= 12;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_InvisibleStuff]				= UIF_UI::UIF_CheckBoxOff;
		pMrProt->wipMemBlock().getadFree()[UIF_UI::WMB_OptimDoubleArray_Start+0]	= 1;
		pMrProt->wipMemBlock().getadFree()[UIF_UI::WMB_OptimDoubleArray_Start+1]	= 1;//WMB_WSdeoptLongArray_End
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_TriggerPosSelBox]			= 0;

		// VAPOR EXTRA CARD WMB elements
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPOR_FA]					= 90;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORCheckBox]				= UIF_UI::UIF_CheckBoxOff;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+0]= 150;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+1]= 100;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+2]= 146;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+3]= 105;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+4]= 106;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+5]= 88;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+6]= 54;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_VAPORdelaysLongArray_Start+7]= 28;//WMB_VAPORdelaysLongArray_End
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_WSdeoptLongArray_Start+0]	= 150;
		pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_WSdeoptLongArray_Start+1]	= 100;//WMB_WSdeoptLongArray_End
		pMrProt->wipMemBlock().getadFree()[UIF_UI::WMB_WSdeoptFractDoubleArray_Start+0]	= 1;
		pMrProt->wipMemBlock().getadFree()[UIF_UI::WMB_WSdeoptFractDoubleArray_Start+1]	= 1;//WMB_WSdeoptLongArray_End
	}
  }
  // If the SelectionBox1 has not yet been initialized, return with error to induce a ContextPrepForMrProtUpdate
  if (!pMrProt->wipMemBlock().getalFree()[UIF_UI::WMB_Selection_Box_GradScheme]) 
    return false; 
  
  return true;
}


#endif