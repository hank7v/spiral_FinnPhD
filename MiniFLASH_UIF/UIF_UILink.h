// -------------------------------------------------------------------------
// file: UIF_UILink.h
//
// FF 29-January-2021
// -------------------------------------------------------------------------


#ifndef _UIF_UILink
#define _UIF_UILink 1


// includes for UI parameters
#ifdef WIN32
  #include "MrServers/MrProtSrv/MrProtocol/UILink/StdProtRes/StdProtRes.h"     // * Protocol defines *
  #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkLimited.h"
  #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkSelection.h"
  #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkArray.h"
  #include "MrServers/MrProtSrv/MrProtocol/libUILink/StdRoutines.h"
  #include "MrServers/MrProtSrv/MrProtocol/UILink/MrStdNameTags.h"
  #include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
  #include <vector>
#endif

#include "ProtBasic/Interfaces/MrWipMemBlock.h"				//needed to use WMB 		
#include "UIConfig.h"

class MrProt;
class SeqLim;
class SeqExpo;
class Sequence;



//////////////////////////////////////////////////////////////////////////////////
// SPECIAL CARD POSITIONS  (i.e. the TAGs associated with Special Card Positions)
////////////////////////////////////////////////////////////////////////////////////

// FIVE SPECIAL CARD FIXED ELEMENTS
#define UIFPos_Selection_Box_ExtraCard MR_TAG_SEQ_WIP1 // GRAD, VAPOR, TEST
#define UIFPos_Selection_Box_GradientScheme MR_TAG_SEQ_WIP2 
#define UIFPos_grad_dur MR_TAG_SEQ_WIP3
#define UIFPos_ndir MR_TAG_SEQ_WIP4
#define UIFPos_ngval MR_TAG_SEQ_WIP5

// NINE SPECIAL CARD DYNAMIC ELEMENTS
// they are all shared among different extra cards (GRAD, VAPOR, TEST)
// few of them will be, in case of GRAD, affected by GradScheme (Factory)
#define UIFPos_LongElement1 MR_TAG_SEQ_WIP12		// Factory	
#define UIFPos_LongElement2 MR_TAG_SEQ_WIP13		// STD
#define UIFPos_LongElement3 MR_TAG_SEQ_WIP6			// Factory 	
#define UIFPos_LongArray1Element MR_TAG_SEQ_WIP9	// STD
#define UIFPos_LongArray2Element MR_TAG_SEQ_WIP10	// STD
#define UIFPos_CheckBox MR_TAG_SEQ_WIP8				// STD
#define UIFPos_DoubleElement MR_TAG_SEQ_WIP7		// Factory
#define UIFPos_DoubleArrayElement MR_TAG_SEQ_WIP11	// Factory
#define UIFPos_Selection_Box MR_TAG_SEQ_WIP14		// factory

//////////////////////////////////////////////////////////////////////////////////
// WMB special indices 
////////////////////////////////////////////////////////////////////////////////////
#define UIF_JUST_AN_INDEX_FOR_A_DOBLE_ELEMENT 10
#define UIF_JUST_AN_INDEX_FOR_A_LONG_ELEMENT 60			// diffusion
#define UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY1_START 20	// diffusion
#define UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY1_END 29		// diffusion
#define UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY2_START 30	// diffusion
#define UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY2_END 35
#define UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY1_START 40	// VAPOR
#define UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY1_END 47	// VAPOR
#define UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY2_START 50	// VAPOR
#define UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY2_END 51	// VAPOR
#define UIF_JUST_AN_INDEX_FOR_A_DOUBLE_ARRAY_START 1 
#define UIF_JUST_AN_INDEX_FOR_A_DOUBLE_ARRAY_END 2
#define UIF_JUST_AN_INDEX_FOR_ANOTHER_DOUBLE_ARRAY_START 3 
#define UIF_JUST_AN_INDEX_FOR_ANOTHER_DOUBLE_ARRAY_END 4
#define UIF_JUST_AN_INDEX_FOR_YETANOTHER_DOUBLE_ARRAY_START 5 
#define UIF_JUST_AN_INDEX_FOR_YETANOTHER_DOUBLE_ARRAY_END 6
#define UIF_JUST_A_FAKE_INDEX_FOR_INVISIBLE_STUFF 61
#define UIF_LONGARRAY1_MAXSIZE 10
#define UIF_LONGARRAY2_MAXSIZE 10
#define UIF_DOUBLEARRAY_MAXSIZE 2

class AbstractUIConfigurator; // we need to declare it here, because it is used as argument of the _UIF_Init method

////////////////////////////////////////////////////////////////////////////////////
//  the structure used to handle WMB elements
////////////////////////////////////////////////////////////////////////////////
struct UIF_UI
{
	
	// these ones are the indices we use during registration. They are just arbitrary indices. The connection between these indices and 
	// the WMB indices will be handled separately (in the function handlers). During registration the REGIndex, the Card Position TAG
	// and the Function Handler sets are "permanently" linked, BUT NOT YET THE WMB elements(!!!) REGIndices and WBMIndices are in fact 
	// two different sets, connected only by the get/set FUNCTION HANDLERS. When an UI element is handled by the user, the system invokes  
	// the function handlers linked with the UI element during registrationpassing to it the REGIndex. We have the responsibility in 
	// our get/set Function Handlers to get/set the right WMB elements. In our implementation, in a function handler, it happens that:
	//
	//	WMBIndex = f ( SELECTIOB_BOX_EXTRA_CARD, SELECTION_BOX_GRAD_SCHEME, REGIndex);
	//
	// in fact what WMBIndex we want to set/get, when the user interacts with a given position UI elemenr, depends on the current card, 
	//  dynamically hosting different objects depending on the choices we did on the two SPECIAL CARD SELECTION BOXES
	//
	// Although the REGindex is an arbitrary index (an unique ID the system will provide to the function handler), we decide to use indices 
	// between 1-14, coresponding to the position TAG numbers. It makes easier to remember them, although it is not necessary in principle.
	//
	// SEL BOX GRAD SEXTRA CARD		(STD CARD)						SEL BOX GRAD SCHEME			(STD CARD)
	//
	// CHECK BOX (e.g. b0)			(EXTRA CARD)					DOUBLE						(EXTRA CARD)
	//
	// LONG x grad dur				(STD CARD)						SEL BOX (or TXTBOX?)		(EXTRA CARD)
	//
	// LONG x #dir					(STD CARD)						LONG						(EXTRA CARD)
	//
	// LONG x #gval					(STD CARD)						LONG						(EXTRA CARD)			
	//
	// LONG ARRAY (e.g. dir)		(EXTRA CARD)					LONG						(EXTRA CARD)
	//
	// LONG ARRAY (e.g. gval)		(EXTRA CARD)					LONG						(EXTRA CARD)
	//
	//
	enum eSeqSpecialParametersREGIndices
	{
		REG_Selection_Box_ExtraCard=1,
		REG_Selection_Box_GradScheme=8,
		REG_grad_dur=3,
		REG_ndir=4,
		REG_ngval=5,
		REG_LongElement1=12,		// Factory
		REG_LongElement2=13,		// Factory
		REG_LongElement3=11,		// Factory 	
		REG_LongArray1Element=6,	
		REG_LongArray2Element=7,
		REG_CheckBox=2,
		REG_DoubleElement=9,		// Factory
		REG_DoubleArrayElement=14,	// Factory
		REG_Selection_Box =10		// factory
	};
	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// WIP MEMORY BLOCK (WMB) indices
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// The following ones are the true WMB indices (from 0 to 63 for LONG elements and from 0 to 15 for double elements)
	// each element of relevance for the sequence behaviour has its own WMBindex. There are many more WMBIindices than REG indices
	// different UI elements will be handled by UI elements on the same Card position, not all of them visible at the same time.
	//
	// NOTE:we decided to set five of them to corespond to the same of the REGIndices, because they are fixed on both CARD and MEMORY. 
	// i.e. they are the ones that will be always visible, handled by what we define the StdUIelementSet.
	enum eSeqSpecialParametersFromFrancesca 
	{
        //
		//	STD CARD ELEMENTS (These ones will always been on card)
		//
		WMB_grad_dur=3,					// NOTE, same as coresponding position<=>REGindex
        WMB_ndir,						// NOTE, same as coresponding position<=>REGindex
        WMB_ngval,						// NOTE, same as coresponding position<=>REGindex
		WMB_Selection_Box_GradScheme=8,	// NOTE, same as coresponding position<=>REGindex
		WMB_Selection_Box_ExtraCard=1,	// NOTE, same as coresponding position<=>REGindex
		//
		// invisible long element (sometime we just need an empty element on the card) 
		//
		WMB_InvisibleStuff=UIF_JUST_A_FAKE_INDEX_FOR_INVISIBLE_STUFF, // the coresponding WMB will host unrelevant invisible stuff LONG values
		//
		//	GRAD SCHEME CARD ELEMENTS (These ones will been on card only when GRAD SCHEME EXTRA CARD IS SELECTED)
		//
		WMB_b0CheckBox= 2,
		WMB_GvalueLongArray_Start=UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY1_START,	// the total WMB indices dedicated to the array are UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY1_END-UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY1_START+1
		WMB_GvalueLongArray_End=UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY1_END,
		WMB_GdirLongArray_Start=UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY2_START,		
		WMB_GdirLongArray_End=UIF_JUST_AN_INDEX_FOR_A_LONG_ARRAY2_END,
		WMB_GradDoubleArray_Start=UIF_JUST_AN_INDEX_FOR_YETANOTHER_DOUBLE_ARRAY_START,		
		WMB_GradDoubleArray_End=UIF_JUST_AN_INDEX_FOR_YETANOTHER_DOUBLE_ARRAY_END,
		//
		//	GRAD SCHEME ELEMENTS (These ones will been on card only when GRAD EXTRA CARD IS SELECTED, and will depend on the GRAD SCHEME choice)
		//
		WMB_LongElementForGradScheme1=UIF_JUST_AN_INDEX_FOR_A_LONG_ELEMENT,
		WMB_LongElementForGradScheme2=6,
		WMB_LongElementForGradScheme3,// =7
		//WMB_DoubleElementForGradScheme // we assign this one as a static cnost long, just as an exercise with C++ types..
		WMB_GradSchemeSelBox=16,
		//
		//	OPTIM CARD ELEMENTS (These ones will been on card only when OPTIM EXTRA CARD IS SELECTED)
		//
		WMB_MaxDiffGrad=12,
		WMB_RiseTime=14,
		WMB_TriggerPosSelBox=13,
		WMB_OptimDoubleArray_Start=UIF_JUST_AN_INDEX_FOR_ANOTHER_DOUBLE_ARRAY_START,		
		WMB_OptimDoubleArray_End=UIF_JUST_AN_INDEX_FOR_ANOTHER_DOUBLE_ARRAY_END,
		//
		//	VAPOR CARD ELEMENTS (These ones will been on card only when VAPOR EXTRA CARD IS SELECTED)
		//
		WMB_VAPOR_FA=17,
		WMB_VAPORCheckBox= 10,
		WMB_VAPORdelaysLongArray_Start=UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY1_START,	// the total WMB indices dedicated to the array are UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY1_END-UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY1_START+1
		WMB_VAPORdelaysLongArray_End=UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY1_END,
		WMB_WSdeoptLongArray_Start=UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY2_START,	
		WMB_WSdeoptArray_End=UIF_JUST_AN_INDEX_FOR_ANOTHER_LONG_ARRAY2_END,
		WMB_WSdeoptFractDoubleArray_Start=UIF_JUST_AN_INDEX_FOR_A_DOUBLE_ARRAY_START,		
		WMB_WSdeoptFractDoubleArray_End=UIF_JUST_AN_INDEX_FOR_A_DOUBLE_ARRAY_END
	};
	// We define the indices of our WMB elements by enum (this is the cleanest way to do it, enum exists for this reason).  
	// BUT, not for any special reason, just as an exercise, instead of assigning all indices in a standard enum way, we may also declare them 
	// as "static const long". See below for example WMB_DoubleElementForGradScheme, The value has to be assigned explicitely in the .cpp file
	// it will be UIF_JUST_AN_INDEX_FOR_A_DOBLE_ELEMENT
	static const long WMB_DoubleElementForGradScheme; //maximum index for double type in WMB is 15. 

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  VALUES for the options of The GradScheme Sel Box, the Check Box and the EXTRA card Sel Box.
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  	enum eSeqSpecialParameterValues
	{
		UIF_TrapGrad=1,
		UIF_FWFGrad,
		UIF_OscGrad,
  		UIF_CheckBoxOn,
		UIF_CheckBoxOff,
		UIF_ExtraCardGrad,
		UIF_ExtraCardVAPOR,
		UIF_ExtraCardOptim
	};
  
};



//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// The global UIelementSet object we use in the function handlers
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
extern UIelementSet GLOBAL_UI_ELEMENTSET; 
#define UIF_AN_INDEX_FOR_THE_GLOBAL_UIELMSET 59 // its ID. 

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// The Refister and Init methods
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32 // only necessary for non-MPCU- processors, since this UI stuff
 
// call this function to register the UI functions
void		_UIF_Register (SeqLim* pSeqLim);

// call this to initialise the WIP protocol members and generate SEQU_ERROR if return value is false
bool		_UIF_Init (MrProt* pMrProt, SeqLim* pSeqLim, AbstractUIConfigurator* pAbstrUIConfig, long lCurrentGradSchemeChoice);

#endif

#endif  
