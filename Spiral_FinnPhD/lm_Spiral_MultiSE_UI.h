//    -----------------------------------------------------------------------------
//      Copyright (C) Siemens AG 1998  All Rights Reserved.
//    -----------------------------------------------------------------------------
//
//     Project: NUMARIS/4
//        File: \n4\pkg\MrServers\MrImaging\seq\a_MiniFLASH\a_MiniFLASH_UI.h
//     Version:
//      Author: Clinical
//        Date: n.a.
//
//        Lang: C++
//
//
//
///  \file   a_template.h
///  \brief  File containing declaraion of the UI class
///          - Templ
///          - Templ1
///
///  This file contains the implementation of the class Templ.
///  The sequence Templ and Templ1 use it to generate very nice images.
///
//    -----------------------------------------------------------------------------

//  -------------------------------------------------------------------------- *
//  Application includes                                                       *
//  -------------------------------------------------------------------------- *
#include "lm_Spiral_MultiSE.h"
#include "MrServers/MrImaging/libUICtrl/UICtrl.h"

#ifdef WIN32
    #include "MrServers/MrProtSrv/MrProtocol/UILink/MrStdNameTags.h"
    #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkLimited.h"
    #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkSelection.h"
#endif

#include "ProtBasic/Interfaces/MrWipMemBlock.h" //for old access style

#ifndef lm_Spiral_MultiSEDW_UI_h
#define lm_Spiral_MultiSEDW_UI_h 1



namespace SEQ_NAMESPACE
{
	enum WIPParamArrayPosLong{ //maximal 64 long
		WIP_lNoADCSamples	   = 0,
		WIP_lDwellTime		   , 
        WIP_lBigDelta          ,
        WIP_lSmallDelta1       ,
        WIP_lSmallDelta2       ,
		WIP_bUseNoADCSamples   ,
        WIP_bUseDwellTime      ,
        WIP_bUseGradAmpl       ,
        WIP_sFileName          ,
        WIP_sFreeDiffFileName  ,
        WIP_sDiffType          ,
        WIP_bVibrationGrad     ,
        WIP_lNoInterleaves     ,
        WIP_lDuration90        ,
        WIP_lDuration180       ,
        WIP_lSetGradToZero     ,
        WIP_lInvertSliceGrads  , 
        WIP_bSwitchOffGrad     ,
        WIP_bSwitchOffRf       ,
        WIP_bNoRfAdjust        ,
        WIP_lNumEcho           ,
		WIP_lDelEcho1          ,
		WIP_lDelEcho2          ,
		WIP_lDelEcho3          ,
		WIP_lDelEcho4          ,
		WIP_lDelEcho5          ,
        WIP_lSliceToTrigger    ,
        WIP_lNumSyncScans      ,
		WIP_l90MinDur          ,
		WIP_l180MinDur		   ,
        WIP_lNoAdcSamplesProt  =50,
        WIP_lDwellTimeProt     ,
        WIP_lSpiralDurProt     ,
        WIP_lFileLengthProt    ,
        WIP_lAdcDurProt        ,
        WIP_lNoSegments        ,
        WIP_lBValueDiff        ,
        WIP_lBValueCrusher     ,
        WIP_lCrushMoment       ,
        WIP_lDiffRampTime      ,
        WIP_lNoDiffDir         ,
        WIP_alTEFil0           ,
        WIP_alTEFil3           
	};

	enum WIPParamArrayPosDouble{ //maximal 16 double
		WIP_dGradAmplP         = 0,
		WIP_dGradAmplR		   ,
        WIP_dDiffGradAmpl      ,
        WIP_dDiffGradAmpl2     ,
        WIP_dCrushAmpl         ,
        WIP_dDiffRiseTime      ,
        WIP_dDiffRiseTime2     ,
        WIP_dCrushRiseTime     ,
		WIP_dReadBeforeEcho    ,
		WIP_dSpinEchoTE        ,
		WIP_dReal90Angle       ,
		WIP_dReal180Angle
	};

	enum WIP_SpecCardPos{
		WIP_lDwellTime_Pos				= 0,
        WIP_dReadBeforeEcho_Pos         = 1, // This position on SSC: delay readout & read before echo
        WIP_lReadMultiEcho_Pos          = 2, // This position on SSC: define echos numbers & actual echo times
		WIP_lDuration_Pos               = 3,
        WIP_lNoInterleaves_Pos          = 5,
		WIP_sFileName_Pos               = 6,
        WIP_sDiffType_Pos               = 7,
        WIP_lBigDelta_Pos               = 8,
        WIP_dDiffGradAmpl_Pos           = 9,
        WIP_dDiffRiseTime_Pos           = 10,
        WIP_sFreeDiffFileName_Pos       = 11,
        WIP_bSwitchOffGrad_Pos          = 12,
		WIP_lSliceToTrigger_Pos         = 13,
        WIP_bVibrationGrad_Pos          = 4
	};

    enum WIP_CheckBoxValues{
        WIP_CheckBoxOff = 1, 
        WIP_CheckBoxOn
    };

    enum GradsToZero{ //prime factors for each gradient
    WIP_zNoGrad = 1, WIP_zExc=2, WIP_zRefoc=3, WIP_zRead=5, WIP_zExcRefoc=6, WIP_zExcRead=10, WIP_zRefocRead=15, WIP_zAll=30
    };

    //  --------------------------------------------------------------------------
    //
    //  Name        : TemplUI
    //
    //  Description :
    /// \brief        This class basically is a storage for the pointers to the
    ///                original setValue / getValue / solve - handlers.
    ///
    ///               The sequence registers new UI handlers, which usually do
    ///                something, then call the original UI handler, and then
    ///                do something else. To keep the information of the original
    ///                UI handlers, the TemplUI class stores the pointers
    ///
    ///               It also provides the method registerUI to execute the
    ///                registration of all new handlers (and the storage of
    ///                 the original pointers)
    ///
    //  --------------------------------------------------------------------------

    class lm_SpiralUI {

    public:

        //  --------------------------------------------------------------
        //
        //  Name        :  TemplUI::TemplUI
        //
        //  Description :
        /// \brief         Initialization of class members
        //
        //  Return      :
        //
        //  --------------------------------------------------------------
        lm_SpiralUI();


        //  --------------------------------------------------------------
        //
        //  Name        :  TemplUI::~TemplUI
        //
        //  Description :
        /// \brief         Destructor
        //
        //  Return      :
        //
        //  --------------------------------------------------------------
        virtual ~lm_SpiralUI();



        //  --------------------------------------------------------------------------
        //
        //  Name        : TemplUI::registerUI
        //
        //  Description :
        /// \brief        This function initializes the UI functions and
        ///                registers all given set / get / Solve - handlers
        ///
        ///               It can be executed on the measuement system, too, but is empty there.
        ///
        ///               On the host, it executes these steps
        ///               - Declaration of pointers to UI classes
        ///               - Registration of overloaded set value handlers
        ///
        ///               It returns an NLS status
        ///
        virtual NLS_STATUS registerUI (SeqLim &rSeqLim, WIPParameterTool& rTool);

		long getMaxDelayTime () { return m_lMaxDelayTime; }


    #ifdef WIN32
		

        //  --------------------------------------------------------------
        ///  \brief Helper class instances for UI handlers
        ///         - register new handler functions
        ///         - save pointer to original handler function
        ///         These classes exit only on the host.
        ///
        ///  The following line is an example which can be removed for
        ///  other sequences.
        //  --------------------------------------------------------------
        //UI_ELEMENT_LONG       m_Contrast;   // Example for set-handler

        //UI_ELEMENT_SELECTION  m_GradMode;   // Example for solve-handler

    #endif

	protected:

		//create all WIP parameters
		void createWIPParameters(SeqLim &rSeqLim, WIPParameterTool& rTool);

        long m_lDefaultSamples;
        long m_lDefaultDwellTime;
		long m_lMaxDelayTime;

	#ifdef WIN32
        //create an array of checkboxes
        void createBoolArray(SeqLim &rSeqLim);
		//in case additional handlers are needed for UI control (Only on host computer)
		virtual void registerUIHandlers(SeqLim &rSeqLim, WIPParameterTool& rTool);
	#endif

	private:

		lm_SpiralUI(const lm_SpiralUI &rRight);
		lm_SpiralUI& operator= (const lm_SpiralUI &rRight);

    };

	

};

#endif
