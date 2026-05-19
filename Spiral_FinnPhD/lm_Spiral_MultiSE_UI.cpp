// --------------------------------------------------------------------------
// General Includes
// --------------------------------------------------------------------------
#include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
#ifdef WIN32
    #include "MrServers/MrProtSrv/MrProtocol/UILink/MrStdNameTags.h"
#endif

//  --------------------------------------------------------------------------
//  Application includes
//  --------------------------------------------------------------------------
#include "lm_Spiral_MultiSE.h"
#include "lm_Spiral_MultiSE_UI.h"

#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif

using namespace SEQ_NAMESPACE;
    ///  \brief Constructor
    ///
    lm_SpiralUI::lm_SpiralUI(){}


    ///  \brief Destructor
    ///
    lm_SpiralUI::~lm_SpiralUI(){}

	
	
    //  --------------------------------------------------------------------------
    //
    //  Name        : registerUI
    //
    //  Description :
    /// \brief        This method registers all given set / get / Solve - handlers
    ///
    ///               It can be executed on the measuement system, too, but is empty there.
    ///
    ///
    ///               It returns an NLS status
    ///
    //  Return      : long
    //
    //  --------------------------------------------------------------------------

    NLS_STATUS lm_SpiralUI::registerUI (SeqLim &rSeqLim, WIPParameterTool& rTool)
    {
        static const char * const ptModule = {"lm_SpiralUI::registerUI"};
		createWIPParameters(rSeqLim, rTool);
	#ifdef WIN32
		registerUIHandlers(rSeqLim, rTool);
	#endif
		return SEQU__NORMAL;

    };

	void lm_SpiralUI::createWIPParameters(SeqLim &rSeqLim, WIPParameterTool& rTool) {
	//some examples for creating parameters from lm_CoCo
	//checkboxes
		//rTool.createBoolParameter(WIP_bUseGradAmplP_Pos, WIP_bUseGradAmplP, rSeqLim, "User Phase Ampl", false);
		//rTool.createBoolParameter(WIP_bUseGradAmplR_Pos, WIP_bUseGradAmplR, rSeqLim, "User Read Ampl", false);
        //rTool.createBoolParameter(0, WIP_bUseGradAmpl, rSeqLim, "No Readout Grad", false); //qf
		//rTool.createBoolParameter(WIP_bUseNoADCSamples_Pos , WIP_bUseNoADCSamples, rSeqLim, "Set ADC samples", true);
		//rTool.createBoolParameter(WIP_bUseNoGradPoints_Pos , WIP_bUseNoGradPoints, rSeqLim, "User Grad Dur", false);
		//rTool.createBoolParameter(WIP_bUseDwellTime_Pos, WIP_bUseDwellTime, rSeqLim, "User Dwelltime", true);
        //rTool.createBoolParameter(WIP_bSwitchOffGrad_Pos, WIP_bSwitchOffGrad, rSeqLim, "Switch Off Grad", false);

        m_lDefaultSamples = 8192;
        m_lDefaultDwellTime = 4000;
		m_lMaxDelayTime = 300;
        rTool.createBoolParameter(WIP_bVibrationGrad_Pos, WIP_bVibrationGrad, rSeqLim, "Vibration Grad", false);

	//Long values   
		//rTool.createLongParameter(WIP_lNoADCSamples_Pos, WIP_lNoADCSamples, rSeqLim, "No. ADC Samples", NULL, 512, 8192, 2, m_lDefaultSamples);
		//rTool.createLongParameter(WIP_lNoGradPoints_Pos, WIP_lNoGradPoints, rSeqLim, "No. Grad Points", NULL, 128, 10000, 1, 4096);
		rTool.createLongParameter(WIP_lDwellTime_Pos, WIP_lDwellTime, rSeqLim, "Dwelltime", "ns", 2000, 10000, 100, m_lDefaultDwellTime);
        //rTool.createLongParameter(WIP_lBigDelta_Pos, WIP_lBigDelta, rSeqLim, "Capital Delta", "us", 2000, 30000, 50, 11200);
        //rTool.createLongParameter(WIP_lSmallDelta_Pos, WIP_lSmallDelta, rSeqLim, "small delta", "us", 300, 25000, 50, 4100);
        rTool.createLongParameter(WIP_lNoInterleaves_Pos, WIP_lNoInterleaves, rSeqLim, "No interleaves", NULL, 1, 20, 1, 1);
        //rTool.createLongParameter(WIP_lSliceToTrigger_Pos, WIP_lSliceToTrigger, rSeqLim, "slice to trig", NULL, -1, K_NO_SLI_MAX, 1, 1);

	//Double values	
		//rTool.createDoubleParameter(WIP_dGradAmplP_Pos, WIP_dGradAmplP, rSeqLim, "Phase Ampl", "mT/m", 2, 0, 300, 0.01, 10.);
		//rTool.createDoubleParameter(WIP_dGradAmplR_Pos, WIP_dGradAmplR, rSeqLim, "Read Ampl", "mT/m", 2, 0, 300, 0.01, 10.);
        //rTool.createDoubleParameter(WIP_dDiffGradAmpl_Pos, WIP_dDiffGradAmpl, rSeqLim, "Diff Gr Ampl", "mT/m", 1, 0., 300., 0.1, 10.);
        //rTool.createDoubleParameter(WIP_dDiffRiseTime_Pos, WIP_dDiffRiseTime, rSeqLim, "Diff Rise Time", "us/mT/m", 1, 5., 50., 0.1, 12.);
        //rTool.createDoubleParameter(WIP_dCrushAmpl_Pos, WIP_dCrushAmpl, rSeqLim, "Crush Gr Ampl", "mT/m", 1, 0., 300., 0.1, 40.);
        //rTool.createDoubleParameter(WIP_dCrushRiseTime_Pos, WIP_dCrushRiseTime, rSeqLim, "Crush Rise Time", "us/mT/m", 1, 5., 50., 0.1, 6.);

	//Dropdown menus
		//rTool.createSelectionParameter(WIP_sICEProgram_Pos, WIP_sICEProgram, rSeqLim, "ICE prog.", NULL);
		//	rTool.addDefaultOption(WIP_sICEProgram_Pos, "2d FT");
		//	rTool.addOption(WIP_sICEProgram_Pos, "Spectro");
		//	rTool.addOption(WIP_sICEProgram_Pos, "Empty");
        rTool.createSelectionParameter(WIP_sFileName_Pos, WIP_sFileName, rSeqLim, "Spiral file", NULL);
        rTool.addOption(WIP_sFileName_Pos, "spiral_ms1");
        rTool.addDefaultOption(WIP_sFileName_Pos, "spiral_ms2");
        rTool.addOption(WIP_sFileName_Pos, "spiral_ms3");
        rTool.addOption(WIP_sFileName_Pos, "spiral_ms4");
        rTool.addOption(WIP_sFileName_Pos, "spiral_ms5");
        rTool.addOption(WIP_sFileName_Pos, "spiral_ms6");
        rTool.addOption(WIP_sFileName_Pos, "spiral_ms7");
        rTool.addOption(WIP_sFileName_Pos, "spiral_ms8");
        /*
        rTool.createSelectionParameter(WIP_lSetGradToZero_Pos, WIP_lSetGradToZero, rSeqLim, "Set grad=0",NULL);
        rTool.addDefaultOption(WIP_lSetGradToZero_Pos, "none");
        rTool.addOption(WIP_lSetGradToZero_Pos, "excite");
        rTool.addOption(WIP_lSetGradToZero_Pos, "refoc");
        rTool.addOption(WIP_lSetGradToZero_Pos, "readout");
        rTool.addOption(WIP_lSetGradToZero_Pos, "exc+refoc");
        rTool.addOption(WIP_lSetGradToZero_Pos, "exc+read");
        rTool.addOption(WIP_lSetGradToZero_Pos, "refoc+read");
        rTool.addOption(WIP_lSetGradToZero_Pos, "exc+refoc+read");
        */

        rTool.createSelectionParameter(WIP_bSwitchOffGrad_Pos, WIP_bSwitchOffGrad, rSeqLim, "Switch off", NULL);
        rTool.addDefaultOption(WIP_bSwitchOffGrad_Pos, "none");
        rTool.addOption(WIP_bSwitchOffGrad_Pos, "grad");
        rTool.addOption(WIP_bSwitchOffGrad_Pos, "rf pulse");
        rTool.addOption(WIP_bSwitchOffGrad_Pos, "rf adjust");
        rTool.addOption(WIP_bSwitchOffGrad_Pos, "grad+rf pulse");
        rTool.addOption(WIP_bSwitchOffGrad_Pos, "grad+rf adjust");
        rTool.addOption(WIP_bSwitchOffGrad_Pos, "rf pulse+adjust");
        rTool.addOption(WIP_bSwitchOffGrad_Pos, "grad+pulse+adjust");
        /*
        rTool.createSelectionParameter(WIP_lInvertSliceGrads_Pos, WIP_lInvertSliceGrads, rSeqLim, "Invert Slice Grad", NULL);
        rTool.addDefaultOption(WIP_lInvertSliceGrads_Pos, "none");
        rTool.addOption(WIP_lInvertSliceGrads_Pos, "excite");
        rTool.addOption(WIP_lInvertSliceGrads_Pos, "refoc");
        rTool.addOption(WIP_lInvertSliceGrads_Pos, "exc+refoc");
        */
        rTool.createSelectionParameter(WIP_sDiffType_Pos, WIP_sDiffType, rSeqLim, "Diffusion type", NULL);
        //rTool.addOption(WIP_sDiffPrep_Pos, "No");
        rTool.addOption(WIP_sDiffType_Pos, "linear");
        rTool.addDefaultOption(WIP_sDiffType_Pos, "free_NoRot");
        rTool.addOption(WIP_sDiffType_Pos, "free_NR+Sc");
        rTool.addOption(WIP_sDiffType_Pos, "free_RM+Sc");
        rTool.addOption(WIP_sDiffType_Pos, "free_RM+Rot");
        rTool.addOption(WIP_sDiffType_Pos, "free_AngToRM");

        rTool.createSelectionParameter(WIP_sFreeDiffFileName_Pos, WIP_sFreeDiffFileName, rSeqLim, "Free Diff File", NULL);
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_LTE1");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_LTE2");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_LTE3");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_PTE1");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_PTE2");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_PTE3");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_STE1");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_STE2");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_STE3");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_CTE1");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_CTE2");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_CTE3");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_OTH1");
        rTool.addDefaultOption(WIP_sFreeDiffFileName_Pos, "Diff_OTH2");
        rTool.addOption(WIP_sFreeDiffFileName_Pos, "Diff_OTH3");

	//Double arrays
        //rTool.createDoubleArrayParameter(WIP_dGradAmpls_Pos, WIP_dGradAmplP, 2, rSeqLim, "Grad Ampl(P/R)", "mT/m", 2, 0.0, 300., 0.01, 10.0);
		//(pos on special card, pos in WipMemBlock.alFree[], arraySize, rSeqLim, label, unit label, number digits displayed, min, max, inc, def)
		/*
		rTool.createDoubleArrayParameter(WIP_BValuesDoubleArray_Pos, WIP_BValue0Double, 10, rSeqLim, "b-value", "s/mm^2", 1, 0.0, 10000.0, 0.5, 0.0);
		rTool.setLimits (WIP_BValuesDoubleArray_Pos, 0, 0.0, 10000.0, 0.5, 0.0);
		rTool.setLimits (WIP_BValuesDoubleArray_Pos, 1, 0.0, 10000.0, 0.5, 0.0);
		rTool.setLimits (WIP_BValuesDoubleArray_Pos, 2, 0.0, 10000.0, 0.5, 0.0);
		rTool.setLimits (WIP_BValuesDoubleArray_Pos, 3, 0.0, 10000.0, 0.5, 0.0);
		*/
		rTool.createDoubleArrayParameter(WIP_dDiffGradAmpl_Pos, WIP_dDiffGradAmpl, 3, rSeqLim, "Diff Gr Ampl", "mT/m", 1, 0., 300., 0.5, 10.0);
        rTool.setLimits (WIP_dDiffGradAmpl_Pos, 2, 0., 300.0, 0.5, 40.0);
        rTool.createDoubleArrayParameter(WIP_dDiffRiseTime_Pos, WIP_dDiffRiseTime, 3, rSeqLim, "Diff Rise Time", "us/mT/m", 1, 5., 50., 0.1, 12.0);
		rTool.setLimits (WIP_dDiffRiseTime_Pos, 2, 5., 50.0, 0.1, 6.0);
		//rTool.createDoubleArrayParameter(WIP_dReadMultiEcho_Pos, WIP_dNumEcho, 6, rSeqLim, "No. TEs", "", 0, 1, 5, 1, 1);
		//rTool.setLimits (WIP_dReadMultiEcho_Pos, 1, 0.0, 200.0, 0.1, 0.0);
		//rTool.setLimits (WIP_dReadMultiEcho_Pos, 2, 0.0, 200.0, 0.1, 0.0);
		//rTool.setLimits (WIP_dReadMultiEcho_Pos, 3, 0.0, 200.0, 0.1, 0.0);
		//rTool.setLimits (WIP_dReadMultiEcho_Pos, 4, 0.0, 200.0, 0.1, 0.0);
		//rTool.setLimits (WIP_dReadMultiEcho_Pos, 5, 0.0, 200.0, 0.1, 0.0);
		rTool.createDoubleArrayParameter(WIP_dReadBeforeEcho_Pos, WIP_dReadBeforeEcho, 2, rSeqLim, "Readout 'push in' ", "us", 1, 0, 100000, 1, 440);
        rTool.setLimits(WIP_dReadBeforeEcho_Pos, 1, 0.0, 200.0, 0.1, 0.0);
        
	//Long arrays
	    //since there are two (I think) overloads of setLimits (long/int32_t and double) the compiler doesn't know which one to use if you only give it integers
        //if the number doesn't have a decimal point it is treated as int (I think) which could be cast to long or double.
        //for the double version you can solve this by including a decimal point to make it implicitly double values
        //for int32_t we need to make it eplicit        
        rTool.createLongArrayParameter(WIP_lBigDelta_Pos, WIP_lBigDelta, 3, rSeqLim, "Diff duration", "us",  300, 30000, 10, 4100);
        rTool.setLimits(WIP_lBigDelta_Pos, 0, static_cast<int32_t>(2000), static_cast<int32_t>(50000), static_cast<int32_t>(10), static_cast<int32_t>(11200));
		
		rTool.createLongArrayParameter(WIP_lDuration_Pos, WIP_lDuration90, 2, rSeqLim, "Pulse duration", "us", 900, 5200, 10, 960);
        rTool.setLimits(WIP_lDuration_Pos, 1, static_cast<int32_t> (900), static_cast<int32_t> (6200), static_cast<int32_t> (10), static_cast<int32_t>(1540));
        //rTool.createLongArrayParameter(WIP_lPushOutRead_Pos, WIP_lPushOutRead, 2, rSeqLim, "Readout shift", "ms", 0, 100, 1, 0);
        //rTool.setLimits(WIP_lPushOutRead_Pos, 1, static_cast<int32_t> (0), static_cast<int32_t> (2000), static_cast<int32_t> (10), static_cast<int32_t>(800));
        rTool.createLongArrayParameter(WIP_lReadMultiEcho_Pos, WIP_lNumEcho, 6, rSeqLim, "No. TEs", "", 1, 5, 1, 1);
        rTool.setLimits(WIP_lReadMultiEcho_Pos, 1, static_cast<int32_t> (0), m_lMaxDelayTime, static_cast<int32_t> (1), static_cast<int32_t> (0));
        rTool.setLimits(WIP_lReadMultiEcho_Pos, 2, static_cast<int32_t> (0), m_lMaxDelayTime, static_cast<int32_t> (1), static_cast<int32_t> (0));
        rTool.setLimits(WIP_lReadMultiEcho_Pos, 3, static_cast<int32_t> (0), m_lMaxDelayTime, static_cast<int32_t> (1), static_cast<int32_t> (0));
        rTool.setLimits(WIP_lReadMultiEcho_Pos, 4, static_cast<int32_t> (0), m_lMaxDelayTime, static_cast<int32_t> (1), static_cast<int32_t> (0));
        rTool.setLimits(WIP_lReadMultiEcho_Pos, 5, static_cast<int32_t> (0), m_lMaxDelayTime, static_cast<int32_t> (1), static_cast<int32_t> (0));

        rTool.createLongArrayParameter(WIP_lSliceToTrigger_Pos, WIP_lSliceToTrigger, 2, rSeqLim, "slice to trig", NULL, -1, K_NO_SLI_MAX, 1, 1);
        rTool.setLimits(WIP_lSliceToTrigger_Pos, 1, static_cast<int32_t>(0), static_cast<int32_t>(10), static_cast<int32_t>(1), static_cast<int32_t>(0));

#ifdef WIN32
      //  createBoolArray(rSeqLim); //qf
#endif
    }

#ifdef WIN32

    unsigned _WIP_DOUBLE_ARRAY_GetLabelId(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "Grad Ampl P";
        static const char* const pszLong1 = "Grad Ampl R";
        static char tLine[100];
        if(lIndex == 0) {
            sprintf(tLine, "%s", pszLong0);
        } else {
            sprintf(tLine, "%s", pszLong1);
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

    unsigned GetLabelId_Ampl(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "Diff Ampl 1";
        static const char* const pszLong1 = "Diff Ampl 2";
        static const char* const pszLong2 = "Crush Ampl";
        static char tLine[100];
        switch(lIndex) {
            case 0:
            sprintf(tLine, "%s", pszLong0);
            break;
            case 1:
            sprintf(tLine, "%s", pszLong1);
            break;
            case 2:
                sprintf(tLine, "%s", pszLong2);
                break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

    unsigned GetLabelId_Rise(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "Diff risetime1";
        static const char* const pszLong1 = "Diff risetime2";
        static const char* const pszLong2 = "Crush risetime";
        static char tLine[100];
        switch(lIndex) {
            case 0:
            sprintf(tLine, "%s", pszLong0);
            break;
            case 1:
            sprintf(tLine, "%s", pszLong1);
            break;
            case 2:
            sprintf(tLine, "%s", pszLong2);
            break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }
	
	unsigned _WIP_DOUBLE_ARRAY_GetLabelId_Delays(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "Read before echo";
        static const char* const pszLong1 = "Spin echo TE";
        static char tLine[100];
        switch(lIndex) { //if(lIndex == 0) {
			case 0:
            sprintf(tLine, "%s", pszLong0);
			break;
			case 1:
            sprintf(tLine, "%s", pszLong1);
			break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

	unsigned _WIP_LONG_ARRAY_GetLabelId_MultiEchos(LINK_LONG_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "No. tot. TEs";
        static const char* const pszLong1 = "Delay 1";
		static const char* const pszLong2 = "Delay 2";
		static const char* const pszLong3 = "Delay 3";
		static const char* const pszLong4 = "Delay 4";
		static const char* const pszLong5 = "Delay 5";
        static char tLine[100];
        switch(lIndex) { //if(lIndex == 0) {
			case 0:
            sprintf(tLine, "%s", pszLong0);
			break;
			case 1:
            sprintf(tLine, "%s", pszLong1);
			break;
			case 2:
			sprintf(tLine, "%s", pszLong2);
			break;
			case 3:
			sprintf(tLine, "%s", pszLong3);
			break;
			case 4:
			sprintf(tLine, "%s", pszLong4);
			break;
			case 5:
			sprintf(tLine, "%s", pszLong5);
			break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

	unsigned _WIP_DOUBLE_ARRAY_GetUnitId_Delays(LINK_DOUBLE_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "us";
        static const char* const pszLong1 = "ms";
        static char tLine[100];
        switch(lIndex) { //if(lIndex == 0) {
		case 0:
            sprintf(tLine, "%s", pszLong0);
			break;
		case 1:
            sprintf(tLine, "%s", pszLong1);
			break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

	unsigned _WIP_LONG_ARRAY_GetUnitId_MultiEchos(LINK_LONG_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "";
        static const char* const pszLong1 = "ms";
        static const char* const pszLong2 = "ms";
        static const char* const pszLong3 = "ms";
        static const char* const pszLong4 = "ms";
        static const char* const pszLong5 = "ms";
		static char tLine[100];
        switch(lIndex) { //if(lIndex == 0) {
			case 0:
            sprintf(tLine, "%s", pszLong0);
			break;
			case 1:
            sprintf(tLine, "%s", pszLong1);
			break;
			case 2:
			sprintf(tLine, "%s", pszLong2);
			break;
			case 3:
			sprintf(tLine, "%s", pszLong3);
			break;
			case 4:
			sprintf(tLine, "%s", pszLong4);
			break;
			case 5:
			sprintf(tLine, "%s", pszLong5);
			break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

    // here we set the delta and DELTA user's entries. ___FF___DOCS
    unsigned GetLabelId_DiffTimes(LINK_LONG_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "Diff time";
        static const char* const pszLong1 = "Grad Dur1";
        static const char* const pszLong2 = "Grad Dur2";
        static char tLine[100];
        switch(lIndex) {
            case 0:
                sprintf(tLine, "%s", pszLong0);
                break;
            case 1:
                sprintf(tLine, "%s", pszLong1);
                break;
            case 2:
                sprintf(tLine, "%s", pszLong2);
                break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

	long CurrentArraySize_MultiEchos(MrUILinkArray* const pUILink, long) {
		//acces the first value to know the size of the array
		return pUILink -> prot().getsWipMemBlock().getalFree()[WIP_lNumEcho]+1;
	}

    unsigned GetLabelId_Syncing(LINK_LONG_TYPE* const, char* arg_list[], long lIndex) {  //labels davor
        static const char* const pszLong0 = "slice to trig";
        static const char* const pszLong1 = "num sync scans";
        static char tLine[100];
        switch (lIndex) {
        case 0:
            sprintf(tLine, "%s", pszLong0);
            break;
        case 1:
            sprintf(tLine, "%s", pszLong1);
            break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

//********************************************************* tool tip handlers *********************************************************
	unsigned int toolTipHandlerLong (LINK_LONG_TYPE* const pUILink, char* arg_list[], long) {
        //accesing the protocol parameters directly over MrProt lead to inconsistent toolTips
        //toolTips from different fields using the same toolTip function showed different values
		std::ostringstream TmpStream;
        TmpStream << "actual gradient duration can differ from chosen one\n    if gradient doesn't end at zero." << std::endl;
		TmpStream << "Gradient duration in file :\t" <<  10*pUILink->prot().getsWipMemBlock().getalFree()[WIP_lFileLengthProt] << " us" << std::endl;
        TmpStream << "Gradient duration: \t" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lSpiralDurProt] << "us" << std::endl;
		TmpStream << "ADC duration:\t" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lAdcDurProt] << " us "<< std::endl;
		TmpStream << "Dwell time:\t" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDwellTimeProt] << " us" << std::endl;
        TmpStream << "No of ADC samples:\t" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lNoAdcSamplesProt] << std::endl;
        TmpStream << "No of ADC segments:\t" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lNoSegments] << std::endl;
        TmpStream << "Time between excite block and diffusion block:\n\t" << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_alTEFil0] << " us" << std::endl;
        TmpStream << "Time between diffusion block and readout:\n\t" << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_alTEFil3] << " us" << std::endl;

        static char sToolTip [2048];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
	}

    unsigned int toolTipHandler_DiffGradAmpl (LINK_DOUBLE_TYPE* const pUILink, char* arg_list[], long) {

		std::ostringstream TmpStream;
        TmpStream << "The two diffusion amplitudes and slew rates can be used \n"
            "to optimize the gradient duration. This is achieved by using\n"
            "slew rate 1 to ramp to amplitude 1 and then use slew rate 2\n"
            "to go amplitude 2. For a single slew rate set amplitude 1 to 0"
            "or set amplitude 2 to the value of 1.\n"<< std::endl;;
        TmpStream << "Diffusion gradient ramp time: " << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDiffRampTime] << std::endl;
        TmpStream << "Number of diffusion directions: " << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lNoDiffDir] << std::endl;
        TmpStream << "B-value produced by the diffusion gradients: " << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lBValueDiff]/1000. << " s/mm^2" << std::endl;
        TmpStream << "B-value produced by the crusher gradients ('b=0'): " << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lBValueCrusher]/1000. << std::endl;

        if(pUILink->prot().getsWipMemBlock().getalFree()[WIP_lCrushMoment]/1000. >
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lSmallDelta1]*pUILink->prot().getsWipMemBlock().getadFree()[WIP_dDiffGradAmpl] &&
            pUILink->prot().getsWipMemBlock().getadFree()[WIP_dDiffGradAmpl] > 0.005) {
                TmpStream << "!!! Warning crushing around the refoc pulse might be insufficient !!!" << std::endl;
                TmpStream << "   Normally used moment (Diff Grad Ampl = 0): \t" << 
                    pUILink->prot().getsWipMemBlock().getalFree()[WIP_lCrushMoment]/1000. << "\n";
                TmpStream << "  Here only \t" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lSmallDelta1]*
                    pUILink->prot().getsWipMemBlock().getadFree()[WIP_dDiffGradAmpl] << " is reached" << std::endl;
        }

        static char sToolTip [2048];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;

    }

     unsigned int toolTipHandler_Times (LINK_LONG_TYPE* const pUILink, char* arg_list[], long) {

		std::ostringstream TmpStream;
        TmpStream << "Time between excite block and diffusion block:\n\t" << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_alTEFil0] << " us" << std::endl;
        TmpStream << "Time between diffusion block and readout:\n\t" << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_alTEFil3] << " us" << std::endl;
        TmpStream << "Total gradient time (ramp + flat + ramp):\n\t" << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDiffRampTime] +
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lSmallDelta1]<< " us" << std::endl;
        TmpStream << "Time between diffusion gradients (PauseDur):\n\t" << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lBigDelta] - 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDiffRampTime] - 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lSmallDelta1] << "us" << std::endl;
        TmpStream << "B-value produced by the diffusion gradients:\n\t" << 
            pUILink->prot().getsWipMemBlock().getalFree()[WIP_lBValueDiff]/1000. << " s/mm^2" << std::endl;

        static char sToolTip [2048];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
    }

     unsigned int toolTipHandler_LongTrig (LINK_LONG_TYPE* const pUILink, char* arg_list[], long) {

		std::ostringstream TmpStream;
        TmpStream << "Choose the slice for which the trigger is send.\n"
            "The number correspond to the temporal position\n"
            "(not spatial), starting at 1. For triggering\n"
            "every slice choose 0., for no trigger choose -1.\n" <<  std::endl;
        /*TmpStream << "Little anecdote: the upper hard limit is the \n"
            "maximum slice number and no soft limit is set,\n"
            "if a value larger than the current number of slices\n"
            "is set, the value is just changed to the number\n"
            "of slices. The reason for this is that otherwise\n"
            "you might be prevented from reducing the number\n"
            "of slices below the value here. It might be \n"
            "possible to write a solve handler for this\n"
            "but I thought it not worth the effort." << std::endl;*/
        TmpStream << "The second option allows you to chose the number of prep scans\n"
            "which can be used to sync field camera measurements with raw data." << std::endl;

        static char sToolTip [1024];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
     }

    unsigned int toolTipHandler_ReadBeforeEcho (LINK_DOUBLE_TYPE* const pUILink, char* arg_list[], long) {

		std::ostringstream TmpStream;
        TmpStream << "This is the time in us by which the spin echo TE \n "
			"is 'shifted in' for the read-out start" <<  std::endl;

        static char sToolTip [1024];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
     }

	unsigned int toolTipHandler_MultiEchos (LINK_LONG_TYPE* const pUILink, char* arg_list[], long) { 
		//should probably look slightly different than it is now

		std::ostringstream TmpStream;
        TmpStream << "This is the declaration of the total number of echos to be acquired.\n"
			"The default No. tot. TEs = 1, and Delay 1 = 0, means that one readout with 0 delay will be played-out. \n"
			"To declare multiple readouts and/or shift the spin-echo TE change values respectively. \n" 
			"Delays: provide values of actual delays in ms, that would be added to the spin-echo TE."
			"(For actual spin echo TE value check 'Spin echo TE', available alongside 'Read before echo', \n"
			"that is populated with TE time from sequence Routine card). \n" <<  std::endl;

        static char sToolTip [1024];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
     }

    unsigned int toolTipHandler_NoInterleaves (LINK_LONG_TYPE* const pUILink, char* arg_list[], long) {

		std::ostringstream TmpStream;
        TmpStream << "For multishot applications, this defines the number of shots."<<  std::endl;
        TmpStream << "The readout trajectory is rotated by 360�/NoInterleaves for each shot." << std::endl;

        static char sToolTip [1024];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
     }

    unsigned int toolTipHandler_PulseDurations (LINK_LONG_TYPE* const pUILink, char* arg_list[], long lIndex) {

		std::ostringstream TmpStream;

        TmpStream << "Duration of excitation (90deg) pulse:" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDuration90] << "us." << std::endl;
		
		if(pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDuration90] < pUILink->prot().getsWipMemBlock().getalFree()[WIP_l90MinDur]) {
			TmpStream << "!!!!! WARNING: The 90deg pulse is clipped !!!!!" << std::endl;
			TmpStream << "Minimum required 90deg pulse duration is: " << pUILink->prot().getsWipMemBlock().getalFree()[WIP_l90MinDur] << "us." <<std::endl; 
		}

		TmpStream << "90deg real flip angle is: " << pUILink->prot().getsWipMemBlock().getadFree()[WIP_dReal90Angle] << std::endl;
		TmpStream << " " << std::endl;

		TmpStream << "Duration of refocusing (180deg) pulse:" << pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDuration180] << "us." << std::endl;
		
		if(pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDuration180] < pUILink->prot().getsWipMemBlock().getalFree()[WIP_l180MinDur]) {
			TmpStream << "!!!!! WARNING: The 180deg pulse is clipped !!!!!" << std::endl;
			TmpStream << "Minimum required 180deg pulse duration is: " << pUILink->prot().getsWipMemBlock().getalFree()[WIP_l180MinDur] << "us." <<std::endl; 
		}

		TmpStream << "180deg real flip angle is: " << pUILink->prot().getsWipMemBlock().getadFree()[WIP_dReal180Angle] << std::endl;
		TmpStream << " " << std::endl;
			
		
        static char sToolTip [2048];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
     }

    unsigned int toolTipHandlerLong_DiffType(LINK_SELECTION_TYPE* const pUILink, char* arg_list[], long lI) {
        std::ostringstream TmpStream;
        TmpStream << "Choose what kind of diffusion encoding you want: \n" 
            << "Linear: Monopolar block as in classical diffusion encodings\n"
            << "free_xxx: read in 3d gradient waveform and rotate according to diffDir file\n" 
            << "  NoRot: No rotation is performed, but still number of diffDir repetitions\n"
            << "  NR+Sc: No rotation but gradient amplitude scaled by norm of target\n"
            << "  RM+Sc: rotate [1,0,0] to target and scale amplitude according to norm\n" 
            << "  RM+Rot: rotate ||target||*2pi around target direction\n" 
            << "  AngToRM: each element*2pi is taken as angle in x-y-z rotation (euler angles)" << std::endl;
                    
        static char sToolTip [2048];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
    }

//********************************************************* other handlers *********************************************************
     bool isAvailableHandler_NoSamples(LINK_LONG_TYPE* const pUILink, long ) {
        //return pUILink->prot().getsWipMemBlock().getalFree()[WIP_bUseNoADCSamples]; //qf
         return true;
     }


     bool getLimitsHandler_NoSamples(LINK_LONG_TYPE* const pUILink, std::vector<MrLimitLong>& rLimitVector, unsigned long& rulVerify, long) {
        long lMin = 512;
        long lMax = 10000;
        long lInc = 2;
        rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
        rLimitVector.resize(1);
        rLimitVector[0].setEqualSpaced(lMin, lMax, lInc);
        return pUILink->prot().getsWipMemBlock().getalFree()[WIP_bUseNoADCSamples];
     }

     bool getLimitsHandler_DwellTime(LINK_LONG_TYPE* const pUILink, std::vector<MrLimitLong>& rLimitVector, unsigned long& rulVerify, long) {
        long lMin = 100;
        long lMax = 10000;
        long lInc = 100;
        rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
        rLimitVector.resize(1);
        rLimitVector[0].setEqualSpaced(lMin, lMax, lInc);
        return pUILink->prot().getsWipMemBlock().getalFree()[WIP_bUseDwellTime];
     }

    bool getLimitsHandler_BW(LINK_DOUBLE_TYPE* const pUILink, std::vector<MrLimitDouble>& rLimitVector, unsigned long& rulVerify, long) {
        //rLimitVector.resize(1);
        //rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
        //long lL = long(fSDSRoundToInc(1.0e9/pUILink->prot().getsWipMemBlock().getalFree()[WIP_lDwellTimeProt],10));
        //rLimitVector[0].setEqualSpaced(lL, lL, lL);
        return false;
    }

/*
//example for getSize handler for arrays to dynamically change the array size
    long _WIP_LONG_ARRAY_CurrentSize(MrUILinkArray* const, long) { 
        return 3;                               
    }
*/

    
    unsigned getLabelID_MTC(LINK_BOOL_TYPE* const, char* arg_list[], long ) {
        static const char* const pszLabel0 = "Dual Inversion";
        arg_list[0] = (char*) pszLabel0;
        return MRI_STD_STRING;        
    }

     void lm_SpiralUI::registerUIHandlers(SeqLim &rSeqLim, WIPParameterTool& rTool) {
        //rTool.registerToolTipHandler (WIP_bUseNoADCSamples_Pos, toolTipHandler_CheckSamples);
        //rTool.registerToolTipHandler (WIP_bUseDwellTime_Pos, toolTipHandler_CheckDwell);
        //rTool.registerToolTipHandler (WIP_bUseGradAmpl_Pos, toolTipHandler_CheckRead);
	    //rTool.registerToolTipHandler (WIP_lNoADCSamples_Pos, toolTipHandlerLong);
        rTool.registerToolTipHandler (WIP_lDwellTime_Pos   , toolTipHandlerLong);
        rTool.registerToolTipHandler (WIP_lBigDelta_Pos   , toolTipHandler_Times);
        //rTool.registerToolTipHandler (WIP_lSmallDelta_Pos   , toolTipHandler_Times);
        rTool.registerToolTipHandler (WIP_lSliceToTrigger_Pos, toolTipHandler_LongTrig);
        rTool.registerToolTipHandler (WIP_dReadBeforeEcho_Pos, toolTipHandler_ReadBeforeEcho);
        rTool.registerGetLabelIdHandler(WIP_dReadBeforeEcho_Pos, _WIP_DOUBLE_ARRAY_GetLabelId_Delays);
		rTool.registerGetUnitIdHandler (WIP_dReadBeforeEcho_Pos, _WIP_DOUBLE_ARRAY_GetUnitId_Delays);
		rTool.registerToolTipHandler (WIP_lReadMultiEcho_Pos, toolTipHandler_MultiEchos);
        rTool.registerGetLabelIdHandler(WIP_lReadMultiEcho_Pos, _WIP_LONG_ARRAY_GetLabelId_MultiEchos);
		rTool.registerGetUnitIdHandler (WIP_lReadMultiEcho_Pos, _WIP_LONG_ARRAY_GetUnitId_MultiEchos);
		rTool.registerToolTipHandler (WIP_lNoInterleaves_Pos, toolTipHandler_NoInterleaves);
        rTool.registerToolTipHandler (WIP_lDuration_Pos, toolTipHandler_PulseDurations);
        rTool.registerToolTipHandler (WIP_sDiffType_Pos, toolTipHandlerLong_DiffType);

        //rTool.registerGetLabelIdHandler(WIP_dGradAmpls_Pos, _WIP_DOUBLE_ARRAY_GetLabelId);
        rTool.registerGetLabelIdHandler(WIP_dDiffGradAmpl_Pos,GetLabelId_Ampl);
        rTool.registerGetLabelIdHandler(WIP_dDiffRiseTime_Pos,GetLabelId_Rise);
        rTool.registerGetLabelIdHandler(WIP_lBigDelta_Pos   , GetLabelId_DiffTimes);
		rTool.registerGetLabelIdHandler(WIP_lSliceToTrigger_Pos   , GetLabelId_Syncing);
        rTool.registerToolTipHandler (WIP_dDiffGradAmpl_Pos, toolTipHandler_DiffGradAmpl);
        rTool.registerToolTipHandler (WIP_dDiffRiseTime_Pos, toolTipHandler_DiffGradAmpl);

        //rTool.registerIsAvailableHandler (WIP_lNoADCSamples_Pos, isAvailableHandler_NoSamples);
        //rTool.registerGetLimitsHandler (WIP_lNoADCSamples_Pos, getLimitsHandler_NoSamples);
        //rTool.registerGetLimitsHandler (WIP_lDwellTime_Pos, getLimitsHandler_DwellTime);
        LINK_DOUBLE_TYPE* pBandwidth = _searchElm <LINK_DOUBLE_TYPE> (&rSeqLim, MR_TAG_BANDWIDTH);
        if (pBandwidth != NULL) {
            pBandwidth->registerGetLimitsHandler (getLimitsHandler_BW);
        }

        LINK_BOOL_TYPE* pMTC =_search<LINK_BOOL_TYPE>(&rSeqLim,MR_TAG_MTC);
        if (pMTC != NULL) {
            pMTC->registerGetLabelIdHandler(getLabelID_MTC);
        }
        /*
        //This is an example on how to register a getSize handler to dynamically adjust the array size
        MrUILinkArray* plTest = _search <MrUILinkArray> (&rSeqLim, rTool.getUITag(WIP_lTestArray_Pos));
        if (plTest != NULL) {
            plTest->registerSizeHandler(_WIP_LONG_ARRAY_CurrentSize);
        }
        */
		
		MrUILinkArray* pArray = _search<MrUILinkArray>(&rSeqLim, rTool.getUITag(WIP_lReadMultiEcho_Pos));
		if(pArray!=NULL) {
			pArray -> registerSizeHandler(CurrentArraySize_MultiEchos);
		}

    }
    
// for the creation of an array of bools
    long BoolArray_MaxSize(MrUILinkArray* const, long) {          //maximale Anzahl an Werten
        return 3;     //MBW
    }

    long BoolArray_CurrentSize(MrUILinkArray* const pThis, long) {  //momentane Anzahl an Werten
        //return pThis->prot().getsWipMemBlock().getalFree()[WIP_NoOfBValuesLong]; 
        return 2;
    }

    unsigned BoolArray_GetLabelId(LINK_BOOL_TYPE* const, char* arg_list[], long lIndex) {
        static const char* const pszDouble0 = "switch off grads";                 //labels davor 
        static const char* const pszDouble1 = "switch off rf";
        static const char* const pszDouble2 = "no rf adjust";
        static char tLine[100];
        switch(lIndex){ 
            case 0:
                sprintf(tLine, "%s", pszDouble0);
                break;
            case 1:
                sprintf(tLine, "%s", pszDouble1);
                break;
            case 2:
                sprintf(tLine, "%s", pszDouble2);
                break;
        }
        arg_list[0] = tLine;
        return MRI_STD_STRING;
    }

    bool BoolArray_GetOptions(LINK_BOOL_TYPE* const, std::vector<unsigned>& rOptionVector, unsigned long& rulVerify, long lIndex) {
        rulVerify = LINK_BOOL_TYPE::VERIFY_ON;
        rOptionVector.resize(2);
        rOptionVector[0] = false;
        rOptionVector[1] = true;
        return true;
    }

    bool BoolArray_GetValue(LINK_BOOL_TYPE* const pThis , long lIndex) {    //Checkbox an oder aus
        return pThis->prot().getsWipMemBlock().getalFree()[WIP_bSwitchOffGrad+lIndex] != WIP_CheckBoxOff;
    }
    
    bool BoolArray_SetValue(LINK_BOOL_TYPE* const pThis, bool value, long lIndex) {
        switch(lIndex) {
            case 0:
            case 1:
            case 2:
        if (value) {
            pThis->prot().getsWipMemBlock().getalFree()[WIP_bSwitchOffGrad+lIndex] = WIP_CheckBoxOn;
        } else {
            pThis->prot().getsWipMemBlock().getalFree()[WIP_bSwitchOffGrad+lIndex] = WIP_CheckBoxOff;
        }
        //pThis->prot().getsWipMemBlock().getalFree()[WIP_bSwitchOffGrad+lIndex] = value;
        return true;
        }
        return false;
    }
     
    unsigned fBSolveBoolConflict(LINK_BOOL_TYPE* const pThis, char** arg_list, const void* pAddMem, const MrProtocolData::MrProtData *pOrigProt, long lIndex) {
        return fUICSolveBoolParamConflict(pThis, arg_list, pAddMem, pOrigProt, lIndex, NULL, NULL, NULL);
    }

	unsigned BoolArray_Solve(LINK_BOOL_TYPE* const pThis, char* arg_list[], const void* pVoid, const MrProtocolData::MrProtData *pMrProt, long lIndex) {
        return fBSolveBoolConflict(pThis, arg_list, pVoid, pMrProt, lIndex);
    }

    unsigned int BoolArray_ToolTipHandler (LINK_BOOL_TYPE* const pUILink, char* arg_list[], long lIndex) {

		std::ostringstream TmpStream;
        switch(lIndex) {
            case 0:
                TmpStream << "Switch off all gradients, e.g. for B0-eddy current correction" <<  std::endl;
                break;
            case 1:
                TmpStream << "Don't use rf-pulses, but keep all timing the same, e.g. for field camera" <<  std::endl;
                break;
            case 2:
                TmpStream << "Don't use the code for forced frequency adjustment of each run of the sequence" <<  std::endl;
                break;
            default:
                TmpStream << "Array of bools. If you see this the tooltips need updating." << std::endl;
                break;
        }
        static char sToolTip [512];
        strcpy (sToolTip, TmpStream.str().c_str());
        
        // fill arg_list
        arg_list [0] = sToolTip;

        // tell UI to display the text in arg_list [0];
        return MRI_STD_STRING;
     }

     void lm_SpiralUI::createBoolArray(SeqLim &rSeqLim) {
        LINK_BOOL_TYPE* pBool = NULL;                       //bvalues

        if (MrUILinkArray* pBoolArray = _createArray<LINK_BOOL_TYPE>(rSeqLim, MR_TAG_SEQ_WIP13, BoolArray_MaxSize, pBool)) {
            pBoolArray->registerSizeHandler(BoolArray_CurrentSize);
            pBool->registerGetLabelIdHandler(BoolArray_GetLabelId);
            pBool->registerGetOptionsHandler(BoolArray_GetOptions);
            pBool->registerGetValueHandler(BoolArray_GetValue);
            pBool->registerSetValueHandler(BoolArray_SetValue);
            pBool->registerGetToolTipIdHandler(BoolArray_ToolTipHandler);
            pBool->registerSolveHandler(BoolArray_Solve);

            pBool->prot().getsWipMemBlock().getalFree()[WIP_bSwitchOffGrad] = WIP_CheckBoxOff;
            pBool->prot().getsWipMemBlock().getalFree()[WIP_bSwitchOffRf] = WIP_CheckBoxOff;
            pBool->prot().getsWipMemBlock().getalFree()[WIP_bNoRfAdjust] = WIP_CheckBoxOn;
        }

     }


	
    lm_Spiral* getSeq (MrUILinkBase* const pThis)
    {
        return ( static_cast<lm_Spiral*>(pThis->sequence().getSeq()) );
    }


    const lm_SpiralUI* getUI (MrUILinkBase* const pThis)
    {
        return ( static_cast<lm_Spiral*>(pThis->sequence().getSeq())->getUI() );
    }

#endif
