//Spiral sequence based on MiniFlash
//phase encoding gradients were removed and readout gradient was replaced
// with spiral gradients on read and phase gradients.
//The gradient form is read from a file (cpp-style reading)
//The sequence special card allows for manipulations of the number of
// samples that are acquired, the dwell time of those.
//Diffusion (or possible crusher) gradient amplitudes and rise times 
//are set on the special card
//The ExtTrigger for the field camera can be send on a single slice
//or on all slices

//Include mutltishot capabilities

//
//possible TODO: check how to influence standard coil choice
//               Hijack resolution and Bandwith per Pixel fields
//               include variable number of prescans
//               implement other slice ordering modes
//               implement STEAM
//               less demanding gradients for diffusion
//               change debugging comments to some macro
//               multishot capabilities

//keep an eye out for:
// - flip angle = 0, this leads to a SAR error in the UT

#include "lm_Spiral_MultiSE.h"
#include "lm_Spiral_MultiSE_UI.h"
#include "Diff.h"
//#include "MrServers/MrImaging/seq/common/Utils/helpfulFunctions.h"
#include "./../common/Utils/helpfulFunctions.h"
#include "MrServers/MrMeasSrv/SeqIF/csequence.h"
#include "MrServers/MrImaging/libSeqSysProp/SysProperties.h"
#include "MrServers/MrImaging/seq/SystemProperties.h"        // Siemens system properties
#include "MrServers/MrImaging/ut/libsequt.h"                 // for mSEQTest
#include "ProtBasic/Interfaces/MrWipMemBlock.h"  
#include "ProtBasic/Interfaces/MrPreparationPulses.h"
#ifdef WIN32
#include "MrServers\MrProtSrv\MrProt\MeasParameter\MrAdjustment.h"
#endif
#include <iostream>
#include <iomanip>
#include <cmath>

#ifdef WIN32
    #include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
#endif


// Macro: return S, if S is an error code
// (applies to functions returning MRRESULT-values)
#ifndef OnErrorReturn
    #define OnErrorReturn(S) if(!MrSucceeded(S)) return(S)
#endif

#define GRT GRAD_RASTER_TIME 

#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif


//The SEQIF_DEFINE macro creates an instance of the sequence
#ifdef SEQUENCE_CLASS_lm_Spiral_D11
    SEQIF_DEFINE (SEQ_NAMESPACE::lm_Spiral)
#endif
using namespace SEQ_NAMESPACE;

    //TODO: maybe sort the entries here and in the header
lm_Spiral::lm_Spiral()
#if defined (WIN32) && defined (DEBUG)
    : bDebug(true)
#else
    : bDebug(false)
#endif
	, m_sSRF01              ("sSRF01")
    , m_sSRF01zSet          ("sSRF01zSet")
    , m_sSRF01zNeg          ("sSRF01zNeg")
    , m_sSRF02              ("sSRF02")
    , m_sSRF02zSet          ("sSRF02zSet")
    , m_sSRF02zNeg          ("sSRF02zNeg")
    , m_sADC01              ("sADC01")
    , m_sADC01zSet          ("sADC01zSet")
    , m_sADC01zNeg          ("sADC01zNeg")
	, m_sADC02              ("sADC02")
    , m_sADC02zSet          ("sADC02zSet")
    , m_sADC02zNeg          ("sADC02zNeg")
	, m_sADC03              ("sADC03")
    , m_sADC03zSet          ("sADC03zSet")
    , m_sADC03zNeg          ("sADC03zNeg")
	, m_sADC04              ("sADC04")
    , m_sADC04zSet          ("sADC04zSet")
    , m_sADC04zNeg          ("sADC04zNeg")
	, m_sADC05              ("sADC05")
    , m_sADC05zSet          ("sADC05zSet")
    , m_sADC05zNeg          ("sADC05zNeg")
    , m_sGSliSel            ("sGSliSel")
    , m_sGSliSelReph        ("sGSliSelReph")
    , m_sGSliRefoc          ("sGSliRefoc")
    , m_sGSpoil             ("sGSpoil")
	, m_sGSpiralP			("sGSpiralP")
	, m_sGSpiralR			("sGSpiralR")
	, m_sGSpiralRefocP		("m_sGSpiralRefocP") //__EK__
	, m_sGSpiralRefocR		("m_sGSpiralRefocR") //__EK__
    , m_sGCrushP            ("sGCrushP")
    , m_sGCrushR            ("sGCrushR")
    , m_sGCrushS            ("sGCrushS")
    , m_lLinesToMeasure     (0)
    , m_dMinRiseTime        (100000)
    , m_dGradMaxAmpl        (0)
    , m_lNoDiffDir          (1)
	, m_lNoReadouts         (1)					 //__EK__
    , m_lNoSegments         (1)
    , m_bCrusherInFwf       (true)
    , m_pUI                 (new lm_SpiralUI()) 
	, m_WIPParamTool		(*this) 
    , m_lSliRampTimeOrig    (0)
    , m_lReadoutBeforeEcho  (800)
    , m_lCrushTime          (0)
    , m_lScanTimeSBBs       (0)
    , m_lReadoutDuration    (0)
    //, m_bNoExcGrad          (false)
    //, m_bNoRefocGrad        (false)
    //, m_bNoReadGrad         (false)
    , m_mySBBList           ()
    , m_CSatSBB		        (&m_mySBBList)
    //, OptfsSBB		        (&mySBBList)
    //, OptfsPrepSBB	        (&mySBBList)
    , m_IRselSBB		        (&m_mySBBList)
    , m_IRnsSBB		        (&m_mySBBList)
    //, NoiseSBB		        (&mySBBList)
    , m_mySeqLoop           ()                  //SeqLoop
{
    // no further instructions...
}


lm_Spiral::~lm_Spiral() {
    if(NULL != m_pUI) {
        // delete the user interface (UI) instance (if a UI instance has been created)
        delete m_pUI;
        m_pUI = NULL;
    }
}



NLSStatus lm_Spiral::initialize(SeqLim &rSeqLim) {
    // This string is intended to store, where we are in the code (used for debugging by the MRTRACE macro).
    static const char *ptModule = {"lm_Spiral::initialize"};
    if(bDebug) std::cout<< "begin initialize" << std::endl;

    // Default return value
    NLS_STATUS lStatus = SEQU__NORMAL;

    // RF frequency is given as a long integer number in Hertz
    // The maximum allowed RF frequency is sufficient for 1H (hydrogen nuclei) in a 3-Tesla-system
    //                         (    min,       max)
    rSeqLim.setAllowedFrequency(8000000, 130850000);

    // The repetition time (TR) and echo time (TE) are given as long integer numbers in micro seconds.
    //                              (index,  min,        max,    increment,  default)
    rSeqLim.setTR                   (    0, 1000,   20000000,         1000,  /*10644000*/5100000); //LM
    rSeqLim.setTE                   (    0,  100,     300000,          100,    60000); //LM
    rSeqLim.setBandWidthPerPixel    (    0,   80,       3000,           10,      260);
    //rSeqLim.getBandWidthPerPixel().setDisplayMode(SEQ::DM_OFF); 

    // The flip angle is given as a double number in degrees.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setFlipAngle            (       00.0,       150.0,         1.0,   90.000);


    // The base resolution tells how many samples in readout direction are acquired in k-space. The increment-value
    // is not set directly. Instead, a key is used which is connected to values of 16, 32, 64, etc. The allowed
    // values can be looked up in "kSpaceDefines.h".
    //                              (        min,        max,    increment,  default)
    rSeqLim.setBaseResolution       (         16,        128,  SEQ::INC_16,      16); //INC_BASE2 also available
    rSeqLim.getBaseResolution().setDisplayMode(SEQ::DM_OFF);
    // The field-of-view (FOV) is given as double numbers in mm.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setReadoutFOV           (        100, SysProperties::getFoVMax(),            1,      300);  //set maximum to maximal allowed value
    rSeqLim.setPhaseFOV             (        100, SysProperties::getFoVMax(),            1,      300);


    // Here, we set the number of lines in phase encoding(PE)-direction.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setPELines              (         1,        128,            1,       1);
    rSeqLim.getPELines().setDisplayMode(SEQ::DM_OFF); //move this to the repetitions loop, where it belongs
    rSeqLim.setRepetitions(0, 511, 1, 0);

    // The MiniFLASH sequence is intended for single-slice measurements only.
    //                              (        min,         max,    increment,  default)
    rSeqLim.setSlices               (          1,K_NO_SLI_MAX,            1,        8);


    // The slice thickness is given as a double number in mm.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setSliceThickness       (      1.000,     10.000,        0.100,    2.500);
    rSeqLim.setSliceDistanceFactor  (      0.000,      8.000,        0.010,    0.100);

    rSeqLim.setMultiSliceMode      (SEQ::MSM_INTERLEAVED, SEQ::MSM_SEQUENTIAL);
    rSeqLim.setSliceSeriesMode     (SEQ::INTERLEAVED, SEQ::ASCENDING,SEQ::DESCENDING);

    rSeqLim.enableSliceShift(); // allow offcenter slice position
    rSeqLim.enableMSMA();
    rSeqLim.enableOffcenter();  // allow an offcenter position of the slab
	//rSeqLim.disableOffcenter();
    rSeqLim.setAllowedSliceOrientation (SEQ::DOUBLE_OBLIQUE);

    
    rSeqLim.setMTC                      (                              SEQ::OFF, SEQ::ON);
    rSeqLim.setInversion(SEQ::SLICE_SELECTIVE,SEQ::INVERSION_OFF,SEQ::VOLUME_SELECTIVE);
    rSeqLim.setTI(0, 10000, 2000000, 100, 520000);
    rSeqLim.setTI(1, 10000, 2000000, 100,  50000);
    
    rSeqLim.setFatSuppression(SEQ::FAT_SUPPRESSION_OFF, SEQ::FAT_SATURATION );//, SEQ::FAT_SUPPRESSION_OPTIMAL);
    rSeqLim.setFatSatMode(SEQ::FAT_SAT_STRONG, SEQ::FAT_SAT_WEAK);

    m_CSatSBB.setIdent("FLCS"); // probably not necessary
    //m_OptfsSBB.setIdent("Optfs");
    //m_OptfsPrepSBB.setIdent("OptfsPrep");
            //add RSATs to the SBBList:
    for (long lr=0; lr<MAXRSATS; lr++) { //RSAT
        m_RSatSBB[lr].addToSBBList (&m_mySBBList);  //RSAT
    }                                            //RSAT
    //RSAT number and thickness
    rSeqLim.setRSats                        (         0,MAXRSATS,        1,        0)  ; //RSAT
    rSeqLim.setRSatThickness                (     3.000, 150.000,    1.000,   50.000)  ; //RSAT
    {
            // Set regional sat identifying names, must be unique
            // naming convention: Sequence identifier + number.
            // maximum of 6 chars are used for intentification!
        char    ptIdentdummy[7] ;                    
        for (long lI = 0; lI < MAXRSATS; lI++) {                                            
            sprintf(ptIdentdummy, "DWV%1d", lI+1);    
            m_RSatSBB[lI].setIdent(ptIdentdummy);        
        }                                            
    }      
	
	rSeqLim.setContrasts(1, 5, 1, 1);
	//rSeqLim.setTE                   (    1,  100,     300000,          100,    60000); //LM
	//rSeqLim.setTE                   (    2,  100,     300000,          100,   100000); //LM
	//rSeqLim.setTE                   (    3,  100,     300000,          100,   140000); //LM
	//rSeqLim.setTE                   (    4,  100,     300000,          100,   180000); //LM
	rSeqLim.getContrasts().setDisplayMode(SEQ::DM_OFF);

/*
    //from flash
    // --------------------------------------------------------------------------------------------
    // Define physiological measurement parameter limmits
    // --------------------------------------------------------------------------------------------
    //
    // In this section, the trigger modes are defined and the number of phases in which each cycle
    // can be subdivided.

    rSeqLim.addPhysioMode (SEQ::SIGNAL_CARDIAC,     SEQ::METHOD_TRIGGERING);
    rSeqLim.addPhysioMode (SEQ::SIGNAL_RESPIRATION, SEQ::METHOD_TRIGGERING);

    rSeqLim.setPhases (1, K_NO_SLI_MAX, 1, 1);

    //from ep2d_diff
     lStatus = PACE::fInit(&rSeqLim,SEQ::RESP_COMP_OFF,SEQ::RESP_COMP_BREATH_HOLD, SEQ::RESP_COMP_TRIGGER);
    if( NLS_SEVERITY(lStatus) != NLS_SUCCESS )
    {
        TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
        return lStatus;
    }
    m_mySeqLoop.setTrigHaltSingleShot   (false);
*/
    //decide which functions should be performed by SeqLoop
    m_mySeqLoop.setPerformOscBit ( false );   //SeqLoop
    m_mySeqLoop.setbHandleTRTIConflict(false); //SeqLoop
    //m_mySeqLoop.setLongTRTrigMode(false);     //SeqLoop
    m_mySeqLoop.setPerformSATs(false);
    m_mySeqLoop.setPerformSATsInCheck(false);
    m_mySeqLoop.setEffectiveTRForR_CSat(false, false);
    m_mySeqLoop.setPerformTRFill(false); //sadly I have to drop this ;-(
    m_mySeqLoop.setPerformTokTokTok(Never);
    m_mySeqLoop.setPerformNoiseMeas (false);
    m_mySeqLoop.setInterleavedIRAllowed(false);
    m_mySeqLoop.setePerformPreparingScans (Never);

	//rSeqLim.setTrajectory(SEQ::TRAJECTORY_CARTESIAN, SEQ::TRAJECTORY_SPIRAL);
    rSeqLim.setTrajectory(SEQ::TRAJECTORY_SPIRAL);

	rSeqLim.setAdjShim (SEQ::ADJSHIM_TUNEUP, SEQ::ADJSHIM_STANDARD, SEQ::ADJSHIM_ADVANCED);

    rSeqLim.setExtSrfFilename  ("%MEASDAT%/extrf.dat")  ;

    // MRRESULT_SEV is a mask for the "severity" bits. This if-statement checks whether the severity bits
    // are set (which indicates that something went wrong).
    if((MRRESULT_SEV & (lStatus = createUI(rSeqLim))) == MRRESULT_SEV) {

        // The MRTRACE macro is used to log information for debugging purposes
        MRTRACE("Instantiation of lm_SpiralUI class failed!");
        return lStatus;
    }
    
    m_mySeqLoop.initRegistryEntries() ;       //SeqLoop
    



    /*fRTSetGPEnable(false);
    fRTSetGREnable(false);
    fRTSetGSEnable(false);
    rSeqLim.disableSAFEConsistencyCheck();*/
    //rSeqLim.setAdjFreProtRelated(SEQ::ON, SEQ::OFF);
    //rSeqLim.setAdjustFreqNonPerm(SEQ::ON, SEQ::OFF);
    //rSeqLim.setAdjustConfFreq(SEQ::ON, SEQ::OFF);
    //MrProt().MrAdjustment().setForAdjFre() ??


    // This compiler directive restricts this code to the HOST version dll.
    // In this way the same source code can be compiled for both the Host and the MARS.
#ifdef WIN32
    if(NULL == m_pUI) {
        MRTRACE("lm_SpiralUI object pointer is null (creation failed probably)!");
        return ( SEQU_ERROR );
    }

#endif
    // ----------------------------------------------
    // Declaration of pointer to UI parameter classes
    // ----------------------------------------------
    //lStatus = m_pUI->registerUI(rSeqLim);
	lStatus = m_pUI->registerUI(rSeqLim, m_WIPParamTool);
	if (!MrSucceeded(lStatus)) return lStatus;

    if(MrSeverity(lStatus) != MRRESULT_SUCCESS) {
        MRTRACE("Registering lm_SpiralUI object failed! Errorcode: %i", lStatus);
        return lStatus;
    }

    // -----------------------------------------------------------------
    // file containing the default postprocessing protocol (EVAProtocol)
    // -----------------------------------------------------------------
#ifdef WIN32
    // The _T(x) macro deals with the unicode conversion of strings.
    rSeqLim.setDefaultEVAProt (_T("%SiemensEvaDefProt%\\Inline\\Inline.evp"));
#endif
    return lStatus;
}



//. -------------------------------------------------------------------------
//.. fSEQPrep: prepare real time elements and calculate energy and timing
//. -------------------------------------------------------------------------
NLSStatus lm_Spiral::prepare(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo)
{
//******************************************************************************************************
// variables needed for later use in several places
//******************************************************************************************************

    if(bDebug) std::cout<< "begin prepare" << std::endl;
    static const char *ptModule = {"lm_Spiral::prepare"};
    NLS_STATUS   lStatus = SEQU__NORMAL;                     // * Return status *
    //double       dMeasureTimeUsec      = 0.0;

    //prepare the seq/special card
	if (!m_WIPParamTool.prepare (rMrProt, rSeqLim)) return SEQU_ERROR;

    long lI; //for loops

    //put WIPParamTool parameters into local variables for easier handling of UI changes
    long wiplNoAdcSamples = 8192;//m_WIPParamTool.getLongValue(rMrProt, WIP_lNoADCSamples_Pos);
    long wiplDwellTime = m_WIPParamTool.getLongValue(rMrProt, WIP_lDwellTime_Pos);
    long wiplRf90Dur = m_WIPParamTool.getLongValue(rMrProt, WIP_lDuration_Pos, 0);
    long wiplRf180Dur = m_WIPParamTool.getLongValue(rMrProt, WIP_lDuration_Pos, 1);
    long wiplNumInterleaves = m_WIPParamTool.getLongValue(rMrProt, WIP_lNoInterleaves_Pos);
    long wiplCapitalDelta = m_WIPParamTool.getLongValue(rMrProt, WIP_lBigDelta_Pos, 0);
    long wiplSmallDelta = m_WIPParamTool.getLongValue(rMrProt, WIP_lBigDelta_Pos, 1);
    long wiplSmallDelta2 = m_WIPParamTool.getLongValue(rMrProt, WIP_lBigDelta_Pos, 2);
    //long wiplReadDelay = m_WIPParamTool.getLongValue(rMrProt, WIP_lPushOutRead_Pos, 0);
    double wipdReadBeforeEcho = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dReadBeforeEcho_Pos, 0);
    long wiplSliToTrig = m_WIPParamTool.getLongValue(rMrProt, WIP_lSliceToTrigger_Pos, 0);
    long wiplSyncScans = m_WIPParamTool.getLongValue(rMrProt, WIP_lSliceToTrigger_Pos, 1);
    double wipdDiffGradAmpl1 = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffGradAmpl_Pos, 0);
    double wipdDiffGradAmpl2 = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffGradAmpl_Pos, 1);
    double wipdCrushGradAmpl = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffGradAmpl_Pos, 2);
    double wipdDiffRiseTime1 = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffRiseTime_Pos, 0);
    double wipdDiffRiseTime2 = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffRiseTime_Pos, 1);
    double wipdCrushRiseTime = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffRiseTime_Pos, 2);
	long wiplNoReadouts = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 0);
	rMrProt.getsWipMemBlock().getadFree()[WIP_dReadBeforeEcho+1] = rMrProt.te()[0]/1000.0;
	long wiplDelEcho1 = 0;
	long wiplDelEcho2 = 0;
	long wiplDelEcho3 = 0;
	long wiplDelEcho4 = 0;
	long wiplDelEcho5 = 0;
	
	switch (wiplNoReadouts) {
		case 5:
			wiplDelEcho5 = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 5);
		case 4:
			wiplDelEcho4 = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 4);
		case 3:
			wiplDelEcho3 = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 3);
		case 2:
			wiplDelEcho2 = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 2);
		case 1:
			wiplDelEcho1 = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 1);
			break;
	}

	std::string wipsSpiralFile = m_WIPParamTool.getSelectionValue(rMrProt, WIP_sFileName_Pos);
    std::string wipsTurnOffStuff = m_WIPParamTool.getSelectionValue(rMrProt, WIP_bSwitchOffGrad_Pos);
    std::string wipsDiffType = m_WIPParamTool.getSelectionValue(rMrProt, WIP_sDiffType_Pos);
    std::string wipsFwfFile = m_WIPParamTool.getSelectionValue(rMrProt, WIP_sFreeDiffFileName_Pos);
    bool wipbVibrationGrad = m_WIPParamTool.getBoolValue(rMrProt, WIP_bVibrationGrad_Pos);


    bool bNoRfAdjust = false;            //std::string::npos is the return value of find if the string could not be found
    if(wipsTurnOffStuff.find("adjust") != std::string::npos) {
        bNoRfAdjust = true;
    }
    if(wipsTurnOffStuff.find("grad") != std::string::npos) {
        m_bNoGradients = true;
    } else {
        m_bNoGradients = false;
    }
    if(wipsTurnOffStuff.find("pulse") != std::string::npos) {
        m_bNoRFpulse = true;
    } else {
        m_bNoRFpulse = false;
    }
                          //search for the key "free" in wipsDiffType and find it somewhere  
    m_bIsFreeDiffusion = (wipsDiffType.find("free") != std::string::npos);
    m_lRotType = NoRot;
    if(wipsDiffType.find("RM+Sc") != std::string::npos) {
        m_lRotType = RotMatScaled;
    }
    if(wipsDiffType.find("RM+Rot") != std::string::npos) {
        m_lRotType = RotMatRot;
    }
    if(wipsDiffType.find("AngToRM") != std::string::npos) {
        m_lRotType = EulerAngles;
    }
    #ifdef WIN32
        // these adjustments are done in a different measurement so this is only known on the host
        // the host then sends the necessary sequence automatically in a seperate command to the scanner
        rMrProt.adjustment().setAdjFreDisablePerformAlways(bNoRfAdjust);
        rMrProt.adjustment().setAdjFreProtRelated(!bNoRfAdjust);
        rMrProt.adjustment().setAdjFreDisabled(bNoRfAdjust);
        if (bDebug) {
            std::cout<< "rMrProt.adjustment(). " << std::endl;
            std::cout << "\t adjFreMode(): " << rMrProt.adjustment().adjFreMode() << std::endl;
            std::cout << "\t getAdjFreConfirmSpec(): " << rMrProt.adjustment().getAdjFreConfirmSpec() << std::endl;
            std::cout << "\t getAdjFreDisablePerformAlways(): " << rMrProt.adjustment().getAdjFreDisablePerformAlways() << std::endl;
            std::cout << "\t getAdjFreProtRelated(): " << rMrProt.adjustment().getAdjFreProtRelated() << std::endl;
            std::cout << "\t getAdjFreDisabled(): " << rMrProt.adjustment().getAdjFreDisabled() << std::endl;
            std::cout << "\t getAdjAllDisabled(): " << rMrProt.adjustment().getAdjAllDisabled() << std::endl;
        }
    #endif

    //this is kept for the possible use in multishot spiral
    m_lLinesToMeasure = wiplNumInterleaves;

	// variable containg information on the number of readouts:
	// *wiplNoReadouts


    //read the file with diffusion directions. Therefore a self written
    //class is used. This is done so that we can finish setting most of 
    //SeqLoop and run its prep function. 
    std::string sDiffDirFile(getenv("CustomerSeq"));
    #ifdef WIN32
	sDiffDirFile += "\\spiral_FWF\\DiffDir_Cha91b.txt"; 
    #else 
	sDiffDirFile += "/spiral_FWF/DiffDir_Cha91b.txt";
    #endif
    //m_DiffDirRead.setFilename(sDiffDirFile);
    m_myDiff.setFilename(sDiffDirFile);
    //if(m_DiffDirRead.readFile() < 0) {
    if(bDebug) {
        long lDiffFileRead = m_myDiff.initDirections(0); //the actual input doesn't matter
        std::cout << "diff dir file read ended with " << lDiffFileRead << std::endl;
        if(lDiffFileRead < 0) return SEQU_ERROR;
    } else {
        if(!m_myDiff.initDirections()) return SEQU_ERROR;
    }
    m_myDiff.setRotationType(m_lRotType);
    m_myDiff.prepRotMats();

    //determine the number of diffusion directions. if no diffusion 
    // gradients are used (amplitude = 0) use only one "directions"
    if(wipdDiffGradAmpl2 > 0.001) {
        //m_lNoDiffDir = m_DiffDirRead.getNoDiffDir();
        m_lNoDiffDir = m_myDiff.getNoDiffDir();
    } else {
        m_lNoDiffDir = 1;
    }


    m_mySeqLoop.setDiffusionLoopLength (m_lNoDiffDir); //SeqLoop
    m_mySeqLoop.setLinesToMeasure(m_lLinesToMeasure); //SeqLoop

    //set the time for preparing scans in us (0 means none)
    m_mySeqLoop.setlPreparingTime(0);                   //SeqLoop
   // m_mySeqLoop.setScaleIRThickness (1.25);

/*
    //from ep2d_diff
    if ( rMrProt.concatenations() > 1 )
  {
      bool bMultiConcatsAllowed = false;

      // Multiple concatenations are allowed if long TR triggering mode is enabled
      // and standard triggering is active

      if ( m_mySeqLoop.isLongTRTrigMode() )
      {
          SEQ::PhysioSignal FirstSignal;
          SEQ::PhysioMethod FirstMethod;
          SEQ::PhysioSignal SecondSignal;
          SEQ::PhysioMethod SecondMethod;

          rMrProt.physiology().getPhysioMode (FirstSignal, FirstMethod, SecondSignal, SecondMethod);

		  if ( FirstMethod == SEQ::METHOD_TRIGGERING )
          {
              bMultiConcatsAllowed = true;
          }
      }

      // Multiple concatenations are allowed if navigator triggering is active

      #ifdef SUPPORT_PACE

          if ( rMrProt.NavigatorParam().getlRespComp() != SEQ::RESP_COMP_OFF )
              bMultiConcatsAllowed = true;

      #endif

      }

      // navigator triggering

    if ((rMrProt.NavigatorParam().getlRespComp() == SEQ::RESP_COMP_TRIGGER)
        || (rMrProt.NavigatorParam().getlRespComp()  == SEQ::RESP_COMP_TRIGGER_AND_FOLLOW))
    {
      m_mySeqLoop.setOptfsPrepflag(true);
    }
    else
    {
      m_mySeqLoop.setOptfsPrepflag(false);
    }
      */
    
    m_mySeqLoop.setpCppSequence(this);
    if (!m_mySeqLoop.prep (rMrProt, rSeqLim, rSeqExpo)) //SeqLoop
        return(m_mySeqLoop.getNLSStatus());             //SeqLoop

    if(bDebug) {
        std::cout<< "*****SeqLoop Stuff: ******" << std::endl;
        std::cout << "lKernelRequestsPerMeasurement: " << m_mySeqLoop.getKernelRequestsPerMeasurement(rSeqLim) << std::endl;
        std::cout << "lines to Measure: " << m_mySeqLoop.getLinesToMeasure() << std::endl;
        std::cout << "partitions to measure: " << m_mySeqLoop.getPartitionsToMeasure() << std::endl;
        std::cout << "phases to measure: " << m_mySeqLoop.getPhasesToMeasure() << std::endl;
        std::cout << "repetitions to measure: " << m_mySeqLoop.getRepetitionsToMeasure () << std::endl;
        std::cout << "preparing scans total: " << m_mySeqLoop.getPreparingScansTotal() << std::endl;
        std::cout << "prep scans per slice: " << m_mySeqLoop.getlPreparingScans() << std::endl;
        std::cout << "time for preparatory pulses: " << m_mySeqLoop.getlScanTimeAllSats() << std::endl;
    }
    //get the limits for crusher, slice, and spoiler gradients
    m_dMinRiseTime = wipdCrushRiseTime;//SysProperties::getGradMinRiseTime(rMrProt.gradSpec().mode());//
    m_dGradMaxAmpl = wipdCrushGradAmpl;//SysProperties::getGradMaxAmpl(rMrProt.gradSpec().mode());//

    //This should be set to the duration of zero gradient in the spiral file
    m_lReadoutBeforeEcho = wipdReadBeforeEcho;

    //I have no experience with solve handlers, so I choose this solution. The reason for no SEQ_ERROR
    //is that it would prevent to set the number of slices below it.
    if(wiplSliToTrig > rMrProt.sliceSeries().getlSize()){
        rMrProt.getsWipMemBlock().getalFree()[WIP_lSliceToTrigger] = rMrProt.sliceSeries().getlSize();
        wiplSliToTrig = rMrProt.sliceSeries().getlSize();
    }
    bool bNoExcGrad = false;
    bool bNoReadGrad = false;
    bool bNoRefocGrad = false;
    std::string sSetGrad0 = "No"; //(m_WIPParamTool.getSelectionValue(rMrProt, WIP_lSetGradToZero_Pos));
    if(sSetGrad0.find("exc") != std::string::npos) {
        bNoExcGrad = true;
    }
    if(sSetGrad0.find("read") != std::string::npos) {
        bNoReadGrad = true;
    }
    if(sSetGrad0.find("refoc") != std::string::npos) {
        bNoRefocGrad = true;
    }
    if (bDebug) {
        std::cout << "bNoExcGrad " << bNoExcGrad << std::endl;
        std::cout << "bNoReadGrad " << bNoReadGrad << std::endl;
        std::cout << "bNoRefocGrad " << bNoRefocGrad << std::endl;
    }


//******************************************************************************************************
// prepare the SBBs
//******************************************************************************************************
    if (bDebug) {
        std::cout << "prepare the SBBs: ..." << std::endl; 
    }
    long lScanTimeSBBs = 0;
    double dEnergyInSBBs = 0.;
    
    double adMaxGradAmpl[] = {m_dGradMaxAmpl, m_dGradMaxAmpl, m_dGradMaxAmpl};
    double adMinRiseTime[] = {m_dMinRiseTime, m_dMinRiseTime, m_dMinRiseTime};
    // start with fatSat
    m_CSatSBB.setGSWDGradientPerformance(rMrProt, rSeqLim);
    m_CSatSBB.setRequestsPerMeasurement(m_mySeqLoop.getKernelRequestsPerMeasurement(rSeqLim));
    m_CSatSBB.setMaxMagnitudes(adMaxGradAmpl);
    m_CSatSBB.setMinRiseTimes(adMinRiseTime);
    m_CSatSBB.setCSatMode(SBBCSatCode_Fat);
    if (!m_CSatSBB.prep(rMrProt, rSeqLim, rSeqExpo)) return m_CSatSBB.getNLSStatus();
    lScanTimeSBBs += m_CSatSBB.getDurationPerRequest();
    dEnergyInSBBs += m_CSatSBB.getEnergyPerRequest();

    if (bDebug) {
        std::cout << "\tCSatSBB done" << std::endl;
    }

    //regional saturation
    for (lI=0; lI < MAXRSATS; lI++) {                      
        m_RSatSBB[lI].setGSWDGradientPerformance(rMrProt, rSeqLim);                                                            
        m_RSatSBB[lI].setRequestsPerMeasurement (m_mySeqLoop.getKernelRequestsPerMeasurement(rSeqLim));                    
        m_RSatSBB[lI].setMaxMagnitudes(adMaxGradAmpl);                                      
        m_RSatSBB[lI].setMinRiseTimes (adMinRiseTime);    

        if ( !m_RSatSBB[lI].prep(rMrProt,rSeqLim,rSeqExpo))  {
            // Preparation of RSatSBB has failed. If this is within the binary search,
            //  that's not really bad, so don't print out any text, just return
            //  with an NLS status.
            // BUT if it happens in the real preparation, then print the information
            //  that it was the RSatSBB that caused the failure
            if ( ! rSeqLim.isContextPrepForBinarySearch() ) {                                                                            
                STRING_TR(rSeqLim, "FAILED to prepare ",m_RSatSBB[lI].getIdent())   
            }                                                               
            return m_RSatSBB[lI].getNLSStatus();                        
        }                                             
        dEnergyInSBBs += m_RSatSBB[lI].getEnergyPerRequest();
        lScanTimeSBBs += m_RSatSBB[lI].getDurationPerRequest();        
     }
    //for  the calculation of TIfill we need the time of all SBBs (including IR)
    //but for the correct TI we need the time for the SBBs without IR in ::run()
    m_lScanTimeSBBs = lScanTimeSBBs;

    if (bDebug) {
        std::cout << "\tRSatSBB done" << std::endl;
    }
    //first try dual inversion recovery with a quick hack. SeqLoop takes care of the first inversion time.
    //if I make sure that the first is the longer one I can use the SBBs for the second one
    //and absorb this in the kernel duration for SeqLoop, or so I hope.
    // slice-selective inversion block
    m_IRselSBB.setGSWDGradientPerformance(rMrProt, rSeqLim);
    m_IRselSBB.setRequestsPerMeasurement(m_mySeqLoop.getKernelRequestsPerMeasurement(rSeqLim));
    m_IRselSBB.setMaxMagnitudes(adMaxGradAmpl);
    m_IRselSBB.setMinRiseTimes(adMinRiseTime);
    if (!m_IRselSBB.prep(rMrProt, rSeqLim, rSeqExpo)) return m_IRselSBB.getNLSStatus();
	
    if (bDebug) {
        std::cout << "\tIRselSBB done" << std::endl;
    }
    // global inversion block
    m_IRnsSBB.setGSWDGradientPerformance(rMrProt, rSeqLim);
    m_IRnsSBB.setRequestsPerMeasurement(m_mySeqLoop.getKernelRequestsPerMeasurement(rSeqLim));
    m_IRnsSBB.setMaxMagnitudes(adMaxGradAmpl);
    m_IRnsSBB.setMinRiseTimes(adMinRiseTime);
    if (!m_IRnsSBB.prep(rMrProt, rSeqLim, rSeqExpo)) return m_IRnsSBB.getNLSStatus();
    //dEnergyInPrepulsesPerRequest += IRnsSBB.getEnergyPerRequest();        //RSAT
	
    if (bDebug) {
        std::cout << "\tIRnsSBB done" << std::endl;
    }

    if(rMrProt.preparationPulses().getucMTC()){
    //check the timing
        dEnergyInSBBs += m_IRnsSBB.getEnergyPerRequest();
        dEnergyInSBBs += m_IRselSBB.getEnergyPerRequest();
        if(m_IRselSBB.getDurationPerRequest()+m_IRnsSBB.getDurationPerRequest() > 0) {
            if (rMrProt.ti()[0] < rMrProt.ti()[1]+(m_IRselSBB.getDurationPerRequest()+m_IRnsSBB.getDurationPerRequest())) {
                return SBB_NEGATIV_TIFILL;
            }
            if(rMrProt.ti()[1] - (m_IRselSBB.getDurationPerRequest()+m_IRnsSBB.getDurationPerRequest()) < lScanTimeSBBs) {
                return SEQU_ERROR;
            }
            lScanTimeSBBs = rMrProt.ti()[1];
        }

    }

    dEnergyInSBBs *= m_mySeqLoop.getKernelRequestsPerMeasurement(rSeqLim) * (rMrProt.getlRepetitions()+1);
    // Noise adjustment SBB.
    //NoiseSBB.setRequestsPerMeasurement(1);
    //if (!NoiseSBB.prep(rMrProt, rSeqLim, rSeqExpo)) return NoiseSBB.getNLSStatus();

    if (bDebug) {
        std::cout << "SBB preparation finished" << std::endl;
    }

//******************************************************************************************************
// read the external spiral and prepare the readout gradients 
//******************************************************************************************************
    //get a string of the file name of the spiral readout file
	std::string sFilename(getenv("CustomerSeq"));
    #ifdef WIN32
	    //sFilename += "\\spiral_multishot\\"; 
        sFilename += "\\spiral_FWF\\";
    #else 
	    sFilename += "/spiral_FWF/";
    #endif
    sFilename += wipsSpiralFile; 
    sFilename += ".txt";
    if (bDebug) {
        std::cout << "try to open file: " << sFilename << "..." << std::flush;
    }
    //read the file and save the input in member variables of the reading class
    m_SpiralRead.setFilename(sFilename);
    m_SpiralRead.setLengthToFileLength();
    if (bDebug) {
        std::cout << "(file length set to " << m_SpiralRead.getLength() << ") ..." << std::flush;
    }
    int i = m_SpiralRead.readFile();
    if(bDebug) {
            std::cout << "File read ended with status: " << i << std::endl;
    }
    if (i < 0) {      
        return SEQU_ERROR;
    }
    if(bDebug) {
        std::cout << "success" << std::endl;
    }
    //for the seq/Special card, the length of the gradient given in the file
    rMrProt.wipMemBlock().getalFree()[WIP_lFileLengthProt] = m_SpiralRead.getFileLength();
	

    m_vfGSpiralP = m_SpiralRead.getPhaseGradient();
    m_vfGSpiralR = m_SpiralRead.getReadGradient();
    double dActualGradAmplP = m_SpiralRead.getGradientAmplitudePhase();
    double dActualGradAmplR = m_SpiralRead.getGradientAmplitudeRead();
    if(bDebug) {
        std::cout << "values for the gradient preparation set properly. " << std::endl;
    }
	
	//if(m_WIPParamTool.getBoolValue(rMrProt,WIP_bUseGradAmpl_Pos)) {
    if( bNoReadGrad ) {
		dActualGradAmplP = 0.;//m_WIPParamTool.getDoubleValue(rMrProt, WIP_dGradAmpls_Pos, 0);
		dActualGradAmplR = 0.;//m_WIPParamTool.getDoubleValue(rMrProt, WIP_dGradAmpls_Pos, 1);
    }

    //limits for the readout gradients perfomance is the system limits
    m_sGSpiralP.setMaxMagnitude(SysProperties::getGradMaxAmplAbsolute());
	m_sGSpiralP.setMinRiseTime(SysProperties::getGradMinRiseTimeAbsolute());

	m_sGSpiralR.setMaxMagnitude(SysProperties::getGradMaxAmplAbsolute());
	m_sGSpiralR.setMinRiseTime(SysProperties::getGradMinRiseTimeAbsolute());
	
    //actually prepare the gradients. The reading class makes sure
    //the gradient forms end on zero.
	m_sGSpiralR.setRampShape(&m_vfGSpiralR[0], m_SpiralRead.getActualLength());//, 0, true);
	m_sGSpiralR.setAmplitude(dActualGradAmplR);
	m_sGSpiralR.setRampUpTime(m_SpiralRead.getGradientDuration());
	m_sGSpiralR.setDuration(m_SpiralRead.getGradientDuration());
	if(!m_sGSpiralR.prep()) {
        if (bDebug) {
            std::cout << "Read spiral prep failed" << std::endl;
        }
		return m_sGSpiralR.getNLSStatus();
    }
    if(!m_sGSpiralR.check()) {
        if (bDebug) {
            std::cout << "Read spiral check failed" << std::endl;
        }
	    return m_sGSpiralR.getNLSStatus();
	}
    m_sGSpiralP.setRampShape(&m_vfGSpiralP[0], m_SpiralRead.getActualLength());//, 0, true);
	m_sGSpiralP.setAmplitude(dActualGradAmplP);
	m_sGSpiralP.setRampUpTime(m_SpiralRead.getGradientDuration());
	m_sGSpiralP.setDuration(m_SpiralRead.getGradientDuration());
	if(!m_sGSpiralP.prep()) {
        if (bDebug) {
            std::cout << "Phase spiral prep failed" << std::endl;
        }
		return m_sGSpiralP.getNLSStatus();
	}
    if(!m_sGSpiralP.check()) {
        if (bDebug) {
            std::cout << "Phase spiral check failed" << std::endl;
        }
		return m_sGSpiralP.getNLSStatus();
	}
    
	if(bDebug) {
        std::cout << "*** file read for spiral ***" << std::endl;
		std::cout << "Its length is: " << m_SpiralRead.getActualLength() << std::endl;
		std::cout << "The amplitudes are: " << dActualGradAmplP << " and " 
			<< dActualGradAmplR << std::endl;
		std::cout << "m_sGSpiralP: " << m_sGSpiralP.getRampUpTime() << " / "
			<< m_sGSpiralP.getDuration() << " / " << m_sGSpiralP.getTotalTime() 
			<< " // " << m_sGSpiralP.getAmplitude() << std::endl;
	}
	long lActualGradientDuration = std::max(m_sGSpiralP.getDuration(),m_sGSpiralR.getDuration());

//******************************************************************************************************
    //prepare RF pulses, slice selection gradients and ADC
//******************************************************************************************************
//ADC might need to know the readout gradient duration (depending on sequence special card)
	//------------------------------------------------
	//prepare external trigger event
	//------------------------------------------------
	m_ExtTrig.prep(0,20);

    //Prepare RF pulses
    bool bInvertExcGrad = false;
    bool bInvertRefocGrad = false;
    std::string sInvertPulseGrad = "No"; //(m_WIPParamTool.getSelectionValue(rMrProt, WIP_lInvertSliceGrads_Pos));
    if(sInvertPulseGrad.find("exc") != std::string::npos) {
        bInvertExcGrad = true;
    }
    if(sInvertPulseGrad.find("refoc") != std::string::npos) {
        bInvertRefocGrad = true;
    }
    if (bDebug) {
        std::cout << "sInvertPulseGrad: " << sInvertPulseGrad << std::endl;
        std::cout << "bInvertExcGrad " << bInvertExcGrad << std::endl;
        std::cout << "bInvertRefocGrad " << bInvertRefocGrad << std::endl;
    }
    //I took the pulses from a_se_b130_15000, but they should be the same family in ep2d_diff
    //although he excitation duration is only 1762 in ep2D_Diff
    m_sSRF01.setTypeExcitation    ();
    //m_sSRF01.setDuration          (2048); // specified min. ratio of slice select gradients (refoc/excit.) is 1.2
    m_sSRF01.setDuration          ( wiplRf90Dur);
    m_sSRF01.setFlipAngle         (rMrProt.flipAngle()); //from MiniFLASH
    //m_sSRF01.setFlipAngle         (rMrProt.getadFlipAngleDegree()[0]); 
    m_sSRF01.setInitialPhase      (90);
    m_sSRF01.setFamilyName        ("SE2560A90.SE90_12A2_2") ;
    //m_sSRF01.setThickness         (rMrProt.getsSliceArray().getasSlice()[0].getdThickness() ) ;
    m_sSRF01.setThickness         (rMrProt.sliceSeries().aFront().thickness()); //from MiniFLASH
    if(bInvertExcGrad) {
        m_sSRF01.setRequiredGSPolarity(-1.0);
    } else {
        m_sSRF01.setRequiredGSPolarity(1.0);
    }
    if(! m_sSRF01.prepExternal(rMrProt,rSeqExpo) )   return (m_sSRF01.getNLSStatus());

    m_sSRF02.setTypeRefocussing   ();
    //m_sSRF02.setDuration          (2560); // specified min. ratio of slice select gradients (refoc/excit.) is 1.
    m_sSRF02.setDuration          (wiplRf180Dur);
    m_sSRF02.setFlipAngle         (180);
    if(rMrProt.flipAngle() == 0) m_sSRF02.setFlipAngle(0);
    m_sSRF02.setInitialPhase      (180);
    m_sSRF02.setFamilyName        ("SE2560A180.SE180_12A2_2") ;
    //m_sSRF02.setThickness         (rMrProt.getsSliceArray().getasSlice()[0].getdThickness());
    m_sSRF02.setThickness         (rMrProt.sliceSeries().aFront().thickness()); //from MiniFLASH
    //decide to invert the refocussing gradient for the 180° pulse for some fat ssturation
    if (
        ( rMrProt.preparationPulses().getucFatSatMode() == SEQ::FAT_SAT_WEAK && 
		rMrProt.preparationPulses().getucFatSat() == SEQ::FAT_SATURATION &&
        !bInvertRefocGrad ) || 
        ( (rMrProt.preparationPulses().getucFatSatMode() == SEQ::FAT_SAT_STRONG || 
		rMrProt.preparationPulses().getucFatSat() == SEQ::FAT_SUPPRESSION_OFF)&&
        bInvertRefocGrad )
        ) {
		m_sSRF02.setRequiredGSPolarity(1.0);
	} else {
		m_sSRF02.setRequiredGSPolarity(-1.0);
	}
    
    if(! m_sSRF02.prepExternal(rMrProt,rSeqExpo) )   return (m_sSRF02.getNLSStatus());

    double dRfEnergyInSRFs  = (m_sSRF01.getPulseEnergyWs()+m_sSRF02.getPulseEnergyWs()) 
                                   * m_mySeqLoop.getKernelRequestsPerMeasurement(rSeqLim) * (rMrProt.getlRepetitions()+1);

	//. Check for Rf pulse clipping
	#ifdef WIN32

		rMrProt.getsWipMemBlock().getadFree()[WIP_dReal90Angle]  = m_sSRF01.getActualFlipAngle();
		rMrProt.getsWipMemBlock().getadFree()[WIP_dReal180Angle] = m_sSRF02.getActualFlipAngle();

		rMrProt.getsWipMemBlock().getalFree()[WIP_l90MinDur]  = static_cast<long>(wiplRf90Dur * 90./m_sSRF01.getActualFlipAngle() + 0.5);
		rMrProt.getsWipMemBlock().getalFree()[WIP_l180MinDur] = static_cast<long>(wiplRf180Dur * 180./m_sSRF02.getActualFlipAngle() + 0.5);
			
    #endif

    //. Prepare  and check slice selection gradient
    m_sGSliSel.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSliSel.setMinRiseTime(m_dMinRiseTime);
    //adjust the ramp time from the MiniFLASH
    long lSliSelRampTime = fSDSRoundUpGRT(std::max<double>(SysProperties::getCoilCtrlLead(),
        m_dMinRiseTime * fabs(m_sSRF01.getGSAmplitude())));
    
    if(!m_sGSliSel.prepAmplitude(lSliSelRampTime, fSDSRoundUpGRT(m_sSRF01.getDuration()/2)*2+lSliSelRampTime,
        lSliSelRampTime, m_sSRF01.getGSAmplitude()))return(m_sGSliSel.getNLSStatus());

    //. Prepare and check slice rephase gradient
    m_sGSliSelReph.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSliSelReph.setMinRiseTime(m_dMinRiseTime);
 /*   if ( !m_sGSliSelReph.prepSymmetricTOTShortestTime(-m_sGSliSel.getMomentum(
            m_sGSliSel.getDuration()-m_sSRF01.getDuration()/2,m_sGSliSel.getTotalTime()))) 
            return (m_sGSliSelReph.getNLSStatus());*/
   if ( !m_sGSliSelReph.prepSymmetricTOTShortestTime(-m_sGSliSel.getMomentum(
          m_sGSliSel.getTotalTime()/2,m_sGSliSel.getTotalTime()))) 
          return (m_sGSliSelReph.getNLSStatus());   

    if(bNoExcGrad) {
        if(!m_sGSliSelReph.prepAmplitude(m_sGSliSelReph.getRampUpTime(), m_sGSliSelReph.getDuration(), 
            m_sGSliSelReph.getRampDownTime(), 0.0)) return m_sGSliSelReph.getNLSStatus();
        if(!m_sGSliSel.prepAmplitude(lSliSelRampTime, fSDSRoundUpGRT(m_sSRF01.getDuration()/2)*2+lSliSelRampTime,
            lSliSelRampTime, 0.0 )) return(m_sGSliSel.getNLSStatus());
    }
    if(!m_sGSliSelReph.check()) return m_sGSliSelReph.getNLSStatus();
    if(!m_sGSliSel.check() ) return (m_sGSliSel.getNLSStatus());

    //Prepare the Crushers around the 180° pulse, done here to potentially merge them with sliceRefocGradient
    //Beware of 2 pi: m_sSRF01.getLarmorConst() gives gamma/2/pi in MHz/T (42.5756)
    double dRes = 2; //in mm
    //this should ensure 6 pi dephasing over dRes
    double dCrushMoment = 3/(dRes*m_sSRF01.getLarmorConst())*1e6;  
    m_sGCrushP.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGCrushP.setMinRiseTime(m_dMinRiseTime);
    m_sGCrushR.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGCrushR.setMinRiseTime(m_dMinRiseTime);
    m_sGCrushS.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGCrushS.setMinRiseTime(m_dMinRiseTime);

    if(!m_sGCrushP.prepSymmetricTOTShortestTime(0.4*dCrushMoment)) return m_sGCrushP.getNLSStatus();
    if(!m_sGCrushR.prepSymmetricTOTShortestTime(0.4*dCrushMoment)) return m_sGCrushR.getNLSStatus();
    if(!m_sGCrushS.prepSymmetricTOTShortestTime(0.4*dCrushMoment)) return m_sGCrushS.getNLSStatus();
    //for when we use different crusher moments
    if(bDebug) {
        std::cout << "m_sGCrushX prepared once" << std::endl;
    }
    //prepare a dummy of the slice refocusing gradient and save the original ramp time in 
    //a member variable
    m_sGSliRefoc.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSliRefoc.setMinRiseTime(m_dMinRiseTime);
    lSliSelRampTime = fSDSRoundUpGRT((std::max<double>(SysProperties::getCoilCtrlLead(),
        m_dMinRiseTime * fabs(m_sSRF02.getGSAmplitude()))));
    m_lSliRampTimeOrig = lSliSelRampTime;

    //prepare the crushers that are used around the 180-pulse
    //m_sGCrushP.setRampTimes(std::max(m_sGCrushP.getRampDownTime(), lSliSelRampTime));
    //m_sGCrushR.setRampTimes(std::max(m_sGCrushP.getRampDownTime(), lSliSelRampTime));
    //m_sGCrushS.setRampTimes(std::max(m_sGCrushP.getRampDownTime(), lSliSelRampTime));

    //for the bridging to work, we need the slice crusher and slice gradient to be in the same direction
 /*   if(m_sGCrushS.getAmplitude()*m_sSRF02.getGSAmplitude() < 0) {
        m_sGCrushS.setAmplitude(-m_sGCrushS.getAmplitude());
    }*/
    if( bNoRefocGrad ) {
        m_sGCrushP.setAmplitude(0);
        m_sGCrushR.setAmplitude(0);
        m_sGCrushS.setAmplitude(0);
    }
 /*   if(!m_sGCrushP.prep()) return m_sGCrushP.getNLSStatus();
    if(!m_sGCrushR.prep()) return m_sGCrushR.getNLSStatus();
    if(!m_sGCrushS.prep()) return m_sGCrushS.getNLSStatus();*/
    if(!m_sGCrushP.check()) return m_sGCrushP.getNLSStatus();
    if(!m_sGCrushR.check()) return m_sGCrushR.getNLSStatus();
    if(!m_sGCrushS.check()) return m_sGCrushS.getNLSStatus();
    m_lCrushTime = std::max(m_sGCrushP.getTotalTime(),std::max(m_sGCrushR.getTotalTime(),m_sGCrushS.getTotalTime()));

    //let's put this on twice the GRT, to make sure all gradient timing is on GRT
    long lSliSelFlatTopTime = fSDSRoundUpGRT(m_sSRF02.getDuration()/2)*2;
    if (bNoRefocGrad) {
       if(!m_sGSliRefoc.prepAmplitude(lSliSelRampTime, lSliSelRampTime+lSliSelFlatTopTime,
            lSliSelRampTime, 0.0 ))return(m_sGSliRefoc.getNLSStatus());
    } else {
        
        if(!m_sGSliRefoc.prepAmplitude(lSliSelRampTime, lSliSelRampTime+lSliSelFlatTopTime,
            lSliSelRampTime, m_sSRF02.getGSAmplitude()))return(m_sGSliRefoc.getNLSStatus());
    }
    if (! m_sGSliRefoc.check() ) return (m_sGSliRefoc.getNLSStatus());

    //. Prepare ADC
    //m_sADC01.setColumns (rMrProt.kSpace().getlBaseResolution());	//LM: This one needs to go higher
	long lNoSamples = 2*m_SpiralRead.getActualLength();
    long lDwellTime = 5000;
	//this gets some more advanced handling:
    // if only one of no samples and dwelltime is chosen the other is adjusted so that the readout covers
    //the whole spiral gradient exactly, if none is chosen we fall back to default dwelltime of 5000, except if
    //this would lead to no samples larger than 8192.
    //both given in UI
    bool bUseSamples = false;//(rMrProt.wipMemBlock().getalFree()[WIP_bUseNoADCSamples] == WIP_CheckBoxOn); //qf
    bool bUseDwellTime = true;//(rMrProt.wipMemBlock().getalFree()[WIP_bUseDwellTime] == WIP_CheckBoxOn);   //qf
    if(bDebug) {
        std::cout << "bUseSamples: " << bUseSamples << ", bUseDwellTime: " << bUseDwellTime << std::endl;
    }

    if(bUseSamples && bUseDwellTime) {
        lNoSamples = wiplNoAdcSamples;
        lDwellTime = wiplDwellTime;
    } else if(!bUseSamples && bUseDwellTime){
        lDwellTime = wiplDwellTime;
        lNoSamples = fSDSRoundToInc(1000*m_SpiralRead.getGradientDuration()/lDwellTime,2);
        if (lNoSamples > 8192) {
            m_lNoSegments = (lNoSamples-lNoSamples%8192)/8192+1;
            lNoSamples = fSDSRoundToInc(lNoSamples/m_lNoSegments,2);}
        rMrProt.getsWipMemBlock().getalFree()[WIP_lNoADCSamples] = lNoSamples;
        wiplNoAdcSamples = lNoSamples;
    } else if(bUseSamples && !bUseDwellTime){
        lNoSamples = wiplNoAdcSamples;
        //I think dwell time should be on a 100 ns raster, or is adjusted to be
        lDwellTime = fSDSRoundToInc(1000*m_SpiralRead.getGradientDuration()/lNoSamples,100);
        rMrProt.getsWipMemBlock().getalFree()[WIP_lDwellTime] = lDwellTime;
        wiplDwellTime = lDwellTime;
    } else if (!bUseSamples && !bUseDwellTime){
        lDwellTime = 4000;
        lNoSamples = fSDSRoundToInc(1000*m_SpiralRead.getGradientDuration()/lDwellTime,2);
        if(lNoSamples > 8192) {
            lNoSamples = 8192;
            lDwellTime = fSDSRoundToInc(1000*m_SpiralRead.getGradientDuration()/lNoSamples,100);
        }
        rMrProt.getsWipMemBlock().getalFree()[WIP_lNoADCSamples] = lNoSamples;
        rMrProt.getsWipMemBlock().getalFree()[WIP_lDwellTime] = lDwellTime;
        wiplNoAdcSamples = lNoSamples;
        wiplDwellTime = lDwellTime;
    }
    if(bDebug) {
        std::cout << "m_lNoSegments: " << m_lNoSegments << ", lNoSamples: " << lNoSamples << std::endl;
    }
    if (lNoSamples > 8192) return SEQU_ERROR;
    if (lDwellTime < 2000) return SEQU_ERROR;

	m_sADC01.setColumns (lNoSamples);
    //m_sADC01.setDwellTime ((long)((rMrProt.rxSpec().effDwellTime(rSeqLim.getReadoutOSFactor())[0]) + 0.5)); 
	m_sADC01.setDwellTime(lDwellTime);
	//m_sADC01.getMDH().setKSpaceCentreColumn((unsigned short)(rMrProt.kSpace().getlBaseResolution()/2));
	m_sADC01.getMDH().setKSpaceCentreColumn(static_cast<unsigned short> fSDSRoundToInc(0.5+(1000.*m_lReadoutBeforeEcho)/lDwellTime,1));

	m_sADC02.setColumns (lNoSamples); 
	m_sADC02.setDwellTime(lDwellTime);
	m_sADC02.getMDH().setKSpaceCentreColumn(static_cast<unsigned short> fSDSRoundToInc(0.5+(1000.*m_lReadoutBeforeEcho)/lDwellTime,1));

	m_sADC03.setColumns (lNoSamples); 
	m_sADC03.setDwellTime(lDwellTime);
	m_sADC03.getMDH().setKSpaceCentreColumn(static_cast<unsigned short> fSDSRoundToInc(0.5+(1000.*m_lReadoutBeforeEcho)/lDwellTime,1));

	m_sADC04.setColumns (lNoSamples); 
	m_sADC04.setDwellTime(lDwellTime);
	m_sADC04.getMDH().setKSpaceCentreColumn(static_cast<unsigned short> fSDSRoundToInc(0.5+(1000.*m_lReadoutBeforeEcho)/lDwellTime,1));

	m_sADC05.setColumns (lNoSamples); 
	m_sADC05.setDwellTime(lDwellTime);
	m_sADC05.getMDH().setKSpaceCentreColumn(static_cast<unsigned short> fSDSRoundToInc(0.5+(1000.*m_lReadoutBeforeEcho)/lDwellTime,1));

    //Fill parameters for sequence special card tooltips     
    rMrProt.wipMemBlock().getalFree()[WIP_lNoAdcSamplesProt] = lNoSamples;
    rMrProt.wipMemBlock().getalFree()[WIP_lDwellTimeProt] = lDwellTime;
    rMrProt.wipMemBlock().getalFree()[WIP_lSpiralDurProt] = std::max(m_sGSpiralP.getDuration(),m_sGSpiralR.getDuration());
    rMrProt.wipMemBlock().getalFree()[WIP_lAdcDurProt] = m_sADC01.getRoundedDuration(10);
    rMrProt.wipMemBlock().getalFree()[WIP_lNoSegments] = m_lNoSegments;

    //since I moved the fill time in front of the sequence to re-enable the multishot (hopefully),
    //one event block ends with an ADC event (potentially). The Problem is that the ADC needs time
    //to shut down which extends into the next 10us block and delays the next event block by 10us.
    //To avoid this I just add 10us to the readout time, which is more than enough time for the 
    //shutdown and avoids annoying TR errors of 10us*noSlices.
    m_lReadoutDuration = std::max(m_sGSpiralP.getTotalTime(),m_sADC01.getRoundedDuration(10))+10;
    
    //spiral refocussing gradients __EK__
	//limits:
	m_sGSpiralRefocP.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSpiralRefocP.setMinRiseTime(m_dMinRiseTime);

    m_sGSpiralRefocR.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSpiralRefocR.setMinRiseTime(m_dMinRiseTime);

	//prepare:
	//getMomentum() gives the gradient moment that is acquired from
    m_sGSpiralRefocP.prepSymmetricTOTShortestTime(-m_sGSpiralP.getMomentum(0, m_sGSpiralP.getTotalTime()));
    m_sGSpiralRefocR.prepSymmetricTOTShortestTime(-m_sGSpiralR.getMomentum(0, m_sGSpiralR.getTotalTime()));
    
	if(!m_sGSpiralRefocP.check()) return m_sGSpiralRefocP.getNLSStatus();
    if(!m_sGSpiralRefocR.check()) return m_sGSpiralRefocR.getNLSStatus();
	
	if(bDebug) {
		std::cout << "NUMBER OF ECHOS: " << wiplNoReadouts << std::endl;
	}
	m_alTEFil[4] = wiplDelEcho1*1000; //__EK/MM__ (this is the delay of the spin-echo TE)
	m_alTEFil[5] = 0; //__EK/MM__
	m_alTEFil[6] = 0; //__EK/MM__
	m_alTEFil[7] = 0; //__EK/MM__
	m_alTEFil[8] = 0; //__EK/MM__
	if(bDebug) {
		std::cout << "VALUE: " << - m_lReadoutDuration
			- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime()) << std::endl;
	}

	//m_lNoReadouts = wiplNoReadouts;
	if(bDebug) {
		std::cout << "ECHO 0: " << m_alTEFil[4] << std::endl;
	}

	if(wiplNoReadouts>1)	{			//__EK/MM__
		m_alTEFil[5] = wiplDelEcho2*1000 - wiplDelEcho1*1000 - m_lReadoutDuration 
			- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
		if(bDebug) {
			std::cout << "ECHO 1: " << m_alTEFil[5] << std::endl;
		}
		if(m_alTEFil[5]<0)  {
			wiplDelEcho2 = wiplDelEcho2 + (abs(m_alTEFil[5])+999)/1000; // rounding up for small values of long
			rMrProt.getsWipMemBlock().getalFree()[WIP_lNumEcho+2] = wiplDelEcho2;
			m_alTEFil[5] = wiplDelEcho2*1000 - wiplDelEcho1*1000 - m_lReadoutDuration 
				- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
			if(bDebug) {
				std::cout << "ECHO 1: " << m_alTEFil[5] << std::endl;
			}
		}
		if(wiplDelEcho2 > m_pUI->getMaxDelayTime()) return SEQU_ERROR;
	}
	if(wiplNoReadouts>2)	{			//__EK/MM__ 
		m_alTEFil[6] = wiplDelEcho3*1000 - wiplDelEcho2*1000 - m_lReadoutDuration	
			- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
		if(bDebug) {
			std::cout << "ECHO 2: " << m_alTEFil[6] << std::endl;
		}
		if(m_alTEFil[6]<0)  {
			wiplDelEcho3 = wiplDelEcho3 + (abs(m_alTEFil[6])+999)/1000; // rounding up for small values of long
			rMrProt.getsWipMemBlock().getalFree()[WIP_lNumEcho+3] = wiplDelEcho3;
			m_alTEFil[6] = wiplDelEcho3*1000 - wiplDelEcho2*1000 - m_lReadoutDuration 
				- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
			if(bDebug) {
				std::cout << "ECHO 2: " << m_alTEFil[6] << std::endl;
			}
		}
		if(wiplDelEcho3 > m_pUI->getMaxDelayTime()) return SEQU_ERROR;
	}
	if(wiplNoReadouts>3)    {			//__EK/MM__
		m_alTEFil[7] = wiplDelEcho4*1000 - wiplDelEcho3*1000 - m_lReadoutDuration
			- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
		if(bDebug) {
			std::cout << "ECHO 3: " << m_alTEFil[7] << std::endl;
		}
		if(m_alTEFil[7]<0)  {
			wiplDelEcho4 = wiplDelEcho4 + (abs(m_alTEFil[7])+999)/1000; // rounding up for small values of long
			rMrProt.getsWipMemBlock().getalFree()[WIP_lNumEcho+4] = wiplDelEcho4;
			m_alTEFil[7] = wiplDelEcho4*1000 - wiplDelEcho3*1000 - m_lReadoutDuration 
				- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
			if(bDebug) {
				std::cout << "ECHO 3: " << m_alTEFil[7] << std::endl;
			}
		}
		if(wiplDelEcho4 > m_pUI->getMaxDelayTime()) return SEQU_ERROR;
	}
	if(wiplNoReadouts>4)   {				//__EK/MM__
		m_alTEFil[8] = wiplDelEcho5*1000 - wiplDelEcho4*1000 - m_lReadoutDuration
			- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
		if(bDebug) {
			std::cout << "ECHO 4: " << m_alTEFil[8] << std::endl;
		}
		if(m_alTEFil[8]<0) {
			wiplDelEcho5 = wiplDelEcho5 + (abs(m_alTEFil[8])+999)/1000; // rounding up for small values of long
			rMrProt.getsWipMemBlock().getalFree()[WIP_lNumEcho+5] = wiplDelEcho5;
			m_alTEFil[8] = wiplDelEcho5*1000 - wiplDelEcho4*1000 - m_lReadoutDuration 
				- std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime());
			if(bDebug) {
				std::cout << "ECHO 4: " << m_alTEFil[8] << std::endl;
			}
		}
		if(wiplDelEcho5 > m_pUI->getMaxDelayTime()) return SEQU_ERROR;
	}
    if((m_alTEFil[5] < 0) || (m_alTEFil[6] < 0) || (m_alTEFil[7] < 0) || (m_alTEFil[8]  < 0)){	//__EK__
        return SEQU_ERROR;
    }

	rMrProt.setlContrasts(wiplNoReadouts);

//******************************************************************************************************
    //prepare the diffusion gradients
//******************************************************************************************************
    //the correct limits for capital Delta need the the duration of different slice selection gradients
    
    //do the diffusion preparation    
    //take care of the timing first
    long lSmallDelta2 = wiplSmallDelta;
    long lBigDelta = wiplCapitalDelta;
    double dDiffMinRiseTime1 = wipdDiffRiseTime1;
    double dDiffGradAmpl1 = wipdDiffGradAmpl1;
    long lDiffRampTime1 = fSDSRoundUpGRT(dDiffGradAmpl1*dDiffMinRiseTime1);
    long lCrushTime = 0;
    double dDiffMinRiseTime2 = wipdDiffRiseTime2;
    double dDiffGradAmpl2 = wipdDiffGradAmpl2;
    long lFieldCameraDelay  = 510; //after the dynamic field camera gets a trigger it takes 505us to measure

    //lDiffRampTime2 not needed, as the only case of lDiffRampTime2!=0 is if lDiffRampTime2==lDiffRampTime1
    if(m_bIsFreeDiffusion) {
        lDiffRampTime1 = 0;
        lSmallDelta2 = wiplSmallDelta2;
        if(m_bCrusherInFwf) {
            lCrushTime = m_lCrushTime;
        }
    }
    //if(dDiffGradAmpl > m_dGradMaxAmpl) return SEQU_ERROR;
    
    long lTimeBeforeRefoc = rMrProt.te()[0]/2 - m_sGSliSel.getTotalTime()/2 - m_sGSliSelReph.getTotalTime()
        /*- m_lCrushTime*/ - m_sGSliRefoc.getTotalTime()/2;
    long lTimeAfterRefoc = rMrProt.te()[0]/2 - m_sGSliRefoc.getTotalTime()/2 /*- m_lCrushTime*/
        - m_lReadoutBeforeEcho;
    long lTimeRefoc = m_sGSliRefoc.getTotalTime() /*+ 2*m_lCrushTime*/;
    if(bDebug) {
        std::cout << "lTimeBeforeRefoc: " << lTimeBeforeRefoc << std::endl;
        std::cout << "rMrProt.te()[0]/2: " << rMrProt.te()[0]/2 << std::endl;
        std::cout << "m_sGSliSel.getTotalTime()/2: " << m_sGSliSel.getTotalTime()/2 <<std::endl;
        std::cout << "m_sGSliSelReph.getTotalTime(): " << m_sGSliSelReph.getTotalTime() << std::endl;
        std::cout << "m_sGSliRefoc.getTotalTime()/2: " << m_sGSliRefoc.getTotalTime()/2 << std::endl;
        std::cout << "lSmallDelta: " << wiplSmallDelta << std::endl;
        std::cout << "lDiffRampTime1: " << lDiffRampTime1 << std::endl;
        std::cout << "lTimeAfterRefoc: " << lTimeAfterRefoc << std::endl;
        std::cout << "lTimeRefoc: " << lTimeRefoc << std::endl;
        std::cout << "m_lCrushTime: " << m_lCrushTime << std::endl;
        std::cout << "dDiffMinRiseTime1: " << dDiffMinRiseTime1 << std::endl;
        std::cout << "dDiffGradAmpl1: " << dDiffGradAmpl1 << std::endl;
        std::cout << "lBigDelta: " << lBigDelta << std::endl;
    }
    if(dDiffGradAmpl1 < 0) {
        return SEQU_ERROR;
    }
    //check the possibility of the timing 
    if(lDiffRampTime1 > wiplSmallDelta) { //ramptime only important for linear diff encoding
        if(bDebug){
        std::cout << "lDiffRampTime1 > wiplSmallDelta ( " << lDiffRampTime1 << " > " << wiplSmallDelta << std::endl;
        }
            return SEQU_ERROR;
    }
    if(lBigDelta < lTimeRefoc+wiplSmallDelta+lDiffRampTime1+2*lCrushTime){
        if(bDebug){
        std::cout << "lBigDelta < lTimeRefoc+wiplSmallDelta+lDiffRampTime1 ( " << lBigDelta << " < " 
            << lTimeRefoc+wiplSmallDelta+lDiffRampTime1 << std::endl;
        }
        return SEQU_ERROR;
    }
    if(wiplSmallDelta+lDiffRampTime1+lCrushTime > lTimeBeforeRefoc){
        if(bDebug){
        std::cout << "wiplSmallDelta+lDiffRampTime1 > lTimeBeforeRefoc ( " << wiplSmallDelta+lDiffRampTime1
            << " > " << lTimeBeforeRefoc << std::endl;
        }
        return SBB_NEGATIV_TEFILL; 
    }
    if(lSmallDelta2+lDiffRampTime1+std::max<long>(lCrushTime, lBigDelta - lTimeBeforeRefoc - lTimeRefoc - lCrushTime)+lFieldCameraDelay > lTimeAfterRefoc){
        if(bDebug){
        std::cout << "wiplSmallDelta+lDiffRampTime1 > lTimeAfterRefoc ( " << wiplSmallDelta+lDiffRampTime1
            << " > " << lTimeAfterRefoc << std::endl;
        }
        return SBB_NEGATIV_TEFILL;
    }
    if(lBigDelta + lSmallDelta2 + lDiffRampTime1 > lTimeBeforeRefoc+lTimeRefoc+lTimeAfterRefoc){
        if(bDebug) {
        std::cout << "lBigDelta + wiplSmallDelta + lDiffRampTime1 > lTimeBeforeRefoc+lTimeRefoc+lTimeAfterRefoc ( " 
            << lBigDelta + wiplSmallDelta + lDiffRampTime1 << " > " << lTimeBeforeRefoc+lTimeRefoc+lTimeAfterRefoc << std::endl;
        }
        return SBB_NEGATIV_TEFILL;
    }
    //if the crushers are used, they are bridged with the slice refocusing gradient. 
    if(m_lCrushTime /*- m_lSliRampTimeOrig*/ > wiplSmallDelta+lDiffRampTime1) return SEQU_ERROR;
    //get the fill times needed, the std::max is used as the possibility has already been checked
    //we want the diffusion gradients to come as early as possible, therefore m_alTEFil[2] only >0
    // if m_alTEFil[0]==0, so that lTimeBeforeRefoc is fully in BigDelta, that is the term 
    // lBigDelta - lTimeBeforeRefoc - lTimeRefoc - 2*lCrushTime is only >= 0 in these cases
    //m_alTEFil[1/2] include m_lCrushTime for FWF with always crushers
    m_alTEFil[2] = std::max<long>(lCrushTime, lBigDelta - lTimeBeforeRefoc - lTimeRefoc - lCrushTime);
    //fixing m_alTEFil[2] defines all other fill times uniquely in all cases
    m_alTEFil[1] = lBigDelta - wiplSmallDelta - lDiffRampTime1 - lTimeRefoc - m_alTEFil[2] - lCrushTime;
    m_alTEFil[0] = lTimeBeforeRefoc- wiplSmallDelta - lDiffRampTime1 - m_alTEFil[1];
    m_alTEFil[3] = lTimeAfterRefoc - lSmallDelta2 - lDiffRampTime1 - m_alTEFil[2]; 

    //vibration gradient:
    m_alTEFil[10] = lBigDelta - wiplSmallDelta - lDiffRampTime1 - m_alTEFil[0] - m_sGSliSel.getTotalTime() 
        - m_sGSliSelReph.getTotalTime() - m_lCrushTime;
    if (m_alTEFil[10] < 0) {
        m_alTEFil[10] = 0;
    }
	if(wipbVibrationGrad) {
		m_lScanTimeSBBs += m_alTEFil[5] + wiplSmallDelta + lDiffRampTime1;
		if( rMrProt.preparationPulses().getucMTC() && 
			m_IRselSBB.getDurationPerRequest()+m_IRnsSBB.getDurationPerRequest() > 0 ) {
			
            if ( rMrProt.ti()[0] < rMrProt.ti()[1]+(m_IRselSBB.getDurationPerRequest()+m_IRnsSBB.getDurationPerRequest()) ) {
                return SBB_NEGATIV_TIFILL;
            }
            if(rMrProt.ti()[1] - (m_IRselSBB.getDurationPerRequest()+m_IRnsSBB.getDurationPerRequest()) < m_lScanTimeSBBs) {
                return SEQU_ERROR;
            }
        }
	} // if(wipbVibrationGrad)

    if (bDebug) {
        std::cout << "m_alTEFil[0]: " << m_alTEFil[0] << std::endl;
        std::cout << "m_alTEFil[1]: " << m_alTEFil[1] << std::endl;
        std::cout << "m_alTEFil[2]: " << m_alTEFil[2] << " could " << lBigDelta - lTimeBeforeRefoc - lTimeRefoc << std::endl;
        std::cout << "m_alTEFil[3]: " << m_alTEFil[3] << std::endl;
        std::cout << "m_alTEFil[4]: " << m_alTEFil[4] << std::endl;
        std::cout << "m_alTEFil[10]: " << m_alTEFil[10] << std::endl;
    }

    //m_alTEFil[0] = std::max<long>(0, lTimeBeforeRefoc + lTimeRefoc - lBigDelta);
    //m_alTEFil[1] = std::max<long>(0, lTimeBeforeRefoc - m_alTEFil[0] - wiplSmallDelta - lDiffRampTime1);
    //m_alTEFil[3] = std::max<long>(0, lTimeAfterRefoc - m_alTEFil[2] - wiplSmallDelta - lDiffRampTime1);

    //not needed anymore
    //if (m_alTEFil[3] < m_ExtTrig.getDuration()) return SBB_NEGATIV_TEFILL;
   
    //long lMinTE = m_sGSliSel.getTotalTime()/2 + m_sGSliSelReph.getTotalTime() + wiplSmallDelta + lBigDelta + lDiffRampTime + m_lReadoutBeforeEcho ;
    //if (rMrProt.te()[0] < lMinTE) return SBB_NEGATIV_TEFILL

    //set the diffusion values for later use in bMatrix calculation
    m_myDiff.setGradients(wiplSmallDelta, lDiffRampTime1, lBigDelta, dDiffGradAmpl1, m_sSRF01.getLarmorConst()*2*3.1415926536);
    double dBValue = 0;
    if(!m_bIsFreeDiffusion) {
    //do dummy prep of the gradients to check them
        m_sGDiffP.setMaxMagnitude(dDiffGradAmpl1);
        m_sGDiffR.setMaxMagnitude(dDiffGradAmpl1);
        m_sGDiffS.setMaxMagnitude(dDiffGradAmpl1);
        
        m_sGDiffP.setMinRiseTime(dDiffMinRiseTime1);
        m_sGDiffR.setMinRiseTime(dDiffMinRiseTime1);
        m_sGDiffS.setMinRiseTime(dDiffMinRiseTime1);

        m_sGDiffP.set(lDiffRampTime1, wiplSmallDelta, lDiffRampTime1);
        m_sGDiffR.set(lDiffRampTime1, wiplSmallDelta, lDiffRampTime1);
        m_sGDiffS.set(lDiffRampTime1, wiplSmallDelta, lDiffRampTime1);

        double dPFactor=0., dRFactor=0., dSFactor=0.;
        for(lI = 0; lI < m_lNoDiffDir; lI++) {
            //checks for every diffusion directions, if they could work
            //if(m_vdFactorP[lI]*m_vdFactorP[lI] + m_vdFactorR[lI]*m_vdFactorR[lI] + m_vdFactorS[lI]*m_vdFactorS[lI] > 1.0001)
            m_myDiff.getDirectionFactors(lI, dPFactor, dRFactor, dSFactor);
            if(bDebug) {
                std::cout << "Dir " << lI << ": " << dPFactor << ", " << 
                    dRFactor << ", " << dSFactor << ", Norm: " << 
                    m_myDiff.getSquareRowNorm(lI) << std::endl;
            }
            //this test limits the total gradient amplitude effectively to 300 mT/m
            //if we want to use maximal amplitude on each axis we don't need it
            if(m_myDiff.getCoordinateSystem() == PRS) {
                if(m_myDiff.getSquareRowNorm(lI) > 1.001) {
                    return SEQU_ERROR;
                }
            }
            if(!m_sGDiffP.prepAmplitude(dDiffGradAmpl1*dPFactor)) return m_sGDiffP.getNLSStatus();
            if(!m_sGDiffP.check()) return m_sGDiffP.getNLSStatus();
            if(!m_sGDiffR.prepAmplitude(dDiffGradAmpl1*dRFactor)) return m_sGDiffR.getNLSStatus();
            if(!m_sGDiffR.check()) return m_sGDiffR.getNLSStatus();
            if(!m_sGDiffS.prepAmplitude(dDiffGradAmpl1*dSFactor)) return m_sGDiffS.getNLSStatus();
            if(!m_sGDiffS.check()) return m_sGDiffS.getNLSStatus();
        }

        dBValue = bValueMono(wiplSmallDelta, lDiffRampTime1, lTimeRefoc+m_alTEFil[1]+m_alTEFil[2], dDiffGradAmpl1,  m_sSRF01.getLarmorConst()*2*3.1415926536);
        
    } else { //if(!m_bIsFreeDiffusion) {

        if (wipbVibrationGrad) {
            return SEQU_ERROR;
        }

        std::string sFreeDiffFile(getenv("CustomerSeq"));
        #ifdef WIN32
                sFreeDiffFile += "\\spiral_FWF\\FreeWaveforms\\"; 
        #else 
                sFreeDiffFile += "/spiral_FWF/FreeWaveforms/";
        #endif
        sFreeDiffFile += wipsFwfFile;
        sFreeDiffFile += ".txt";
        
        m_readFreeDiff.setFilename(sFreeDiffFile);
        m_readFreeDiff.setAsymmetricBasedOnFiles();
        m_readFreeDiff.setReverseSecondGradient(m_readFreeDiff.isSymmetric()); //if we read only one file, we want the second to be time reversed
        //m_readFreeDiff.setSysProperties::getGradMinRiseTime(rMrProt.gradSpec().mode()));
        m_readFreeDiff.setLengthToFileLength();            
        if (bDebug) {
            std::cout << "trying to read file " << sFreeDiffFile << std::flush;
            std::cout << "(file length set to " << m_readFreeDiff.getLength() << ") ..." << std::flush;
        }
        i = m_readFreeDiff.readFile();
        if(bDebug) {
                std::cout << "File read ended with status: " << i << std::endl;
        }
        if (i < 0) {      
            return SEQU_ERROR;
        }
        if(bDebug) {
            std::cout << "success" << std::endl;
            std::cout << "dDiffGradAmpl: " << dDiffGradAmpl1 << ", " << dDiffGradAmpl2 << std::endl; 
        }

        //std::vector<std::vector<float> > vfFreeDiffForm;
        /*if(bDebug) {
            std::cout << "fill times: " << m_alTEFil[0] << ", " << m_alTEFil[1] << ", " 
                << m_alTEFil[2] << ", " << m_alTEFil[3] << std::endl;  
        }*/
        m_readFreeDiff.setGradientAmplitudes(dDiffGradAmpl1, dDiffGradAmpl2);
        if(bDebug) {
            std::cout << "interpolate diffusion gradients to " << wiplSmallDelta << " and " << wiplSmallDelta2 << "..." <<  std::flush;
        }
        m_readFreeDiff.interpGradients(wiplSmallDelta, wiplSmallDelta2);
        m_readFreeDiff.setRefocDuration(wiplCapitalDelta - wiplSmallDelta - lTimeRefoc - 2*lCrushTime);
        
        if(bDebug) {
            std::cout << "done" << std::endl;
        }
        //set the diffusion values for later use in bMatrix calculation
        m_myDiff.setGradients(wiplSmallDelta, lDiffRampTime1, wiplCapitalDelta, dDiffGradAmpl1, m_sSRF01.getLarmorConst()*2*3.1415926536);

        //do dummy prep of the gradients to check them
        m_sGFwfP1.setMaxMagnitude(dDiffGradAmpl1);//(SysProperties::getGradMaxAmplAbsolute());
        m_sGFwfR1.setMaxMagnitude(dDiffGradAmpl1);
        m_sGFwfS1.setMaxMagnitude(dDiffGradAmpl1);
        
        m_sGFwfP1.setMinRiseTime(dDiffMinRiseTime1);//(SysProperties::getGradMinRiseTimeAbsolute());
        m_sGFwfR1.setMinRiseTime(dDiffMinRiseTime1);
        m_sGFwfS1.setMinRiseTime(dDiffMinRiseTime1);

        m_vfGFwfP1 = m_readFreeDiff.getFirstInterpPhaseGradient();
        m_sGFwfP1.setRampShape(&m_vfGFwfP1[0], wiplSmallDelta/10);//, 0, true);
        m_sGFwfP1.setAmplitude(dDiffGradAmpl1);
        m_sGFwfP1.setRampUpTime(wiplSmallDelta);
        m_sGFwfP1.setDuration(wiplSmallDelta);
        //m_sGDiffArb1[lI].setRampUpTime(10*m_vfFreeDiffP.size());
        //m_sGDiffArb1[lI].setDuration(10*m_vfFreeDiffP.size());
        if(!m_sGFwfP1.prep()) {
            if(bDebug) {
                std::cout << "phase prep failed" << std::endl;
            }
            return m_sGFwfP1.getNLSStatus();
        }
        if(!m_sGFwfP1.check()) {
            if(bDebug) {
                std::cout << "phase check failed" << std::endl;
            }
            return m_sGFwfP1.getNLSStatus();
        }

        m_vfGFwfR1 = m_readFreeDiff.getFirstInterpReadGradient();
        m_sGFwfR1.setRampShape(&m_vfGFwfR1[0], wiplSmallDelta/10);//, 0, true);
        m_sGFwfR1.setAmplitude(dDiffGradAmpl1);
        m_sGFwfR1.setRampUpTime(wiplSmallDelta);
        m_sGFwfR1.setDuration(wiplSmallDelta);
        //m_sGDiffArb1[lI].setRampUpTime(10*m_vfFreeDiffP.size());
        //m_sGDiffArb1[lI].setDuration(10*m_vfFreeDiffP.size());
        if(!m_sGFwfR1.prep()) {
            if(bDebug) {
                std::cout << "read prep failed" << std::endl;
            }
            return m_sGFwfR1.getNLSStatus();
        }
        if(!m_sGFwfR1.check()) {
            if(bDebug) {
                std::cout << "read check failed" << std::endl;
            }
            return m_sGFwfR1.getNLSStatus();
        }

        m_vfGFwfS1 = m_readFreeDiff.getFirstInterpSliceGradient();
        m_sGFwfS1.setRampShape(&m_vfGFwfS1[0], wiplSmallDelta/10);//, 0, true);
        m_sGFwfS1.setAmplitude(dDiffGradAmpl1);
        m_sGFwfS1.setRampUpTime(wiplSmallDelta);
        m_sGFwfS1.setDuration(wiplSmallDelta);
        //m_sGDiffArb1[lI].setRampUpTime(10*m_vfFreeDiffP.size());
        //m_sGDiffArb1[lI].setDuration(10*m_vfFreeDiffP.size());
        if(!m_sGFwfS1.prep()) {
            if(bDebug) {
                std::cout << "slice prep failed" << std::endl;
            }
            return m_sGFwfS1.getNLSStatus();
        }
        if(!m_sGFwfS1.check()) {
            if(bDebug) {
                std::cout << "slice check failed" << std::endl;
            }
            return m_sGFwfS1.getNLSStatus();
        }
        if(bDebug) {
            std::cout << "finished prepping the first diffusion gradients" << std::endl;
        }
        
        m_sGFwfP2.setMaxMagnitude(dDiffGradAmpl2);//(SysProperties::getGradMaxAmplAbsolute());
        m_sGFwfR2.setMaxMagnitude(dDiffGradAmpl2);
        m_sGFwfS2.setMaxMagnitude(dDiffGradAmpl2);
        
        m_sGFwfP2.setMinRiseTime(dDiffMinRiseTime2);//(SysProperties::getGradMinRiseTimeAbsolute());
        m_sGFwfR2.setMinRiseTime(dDiffMinRiseTime2);
        m_sGFwfS2.setMinRiseTime(dDiffMinRiseTime2);

        m_vfGFwfP2 = m_readFreeDiff.getSecondInterpPhaseGradient();
        m_sGFwfP2.setRampShape(&m_vfGFwfP2[0], wiplSmallDelta2/10);//, 0, true);
        m_sGFwfP2.setAmplitude(dDiffGradAmpl2);
        m_sGFwfP2.setRampUpTime(wiplSmallDelta2);
        m_sGFwfP2.setDuration(wiplSmallDelta2);
        if(!m_sGFwfP2.prep()) {
            if(bDebug) {
                std::cout << "phase2 prep failed" << std::endl;
            }
            return m_sGFwfP2.getNLSStatus();
        }
        if(!m_sGFwfP2.check()) {
            if(bDebug) {
                std::cout << "phase2 check failed" << std::endl;
            }
            return m_sGFwfP2.getNLSStatus();
        }

        m_vfGFwfR2 = m_readFreeDiff.getSecondInterpReadGradient();
        m_sGFwfR2.setRampShape(&m_vfGFwfR2[0], wiplSmallDelta2/10);//, 0, true);
        m_sGFwfR2.setAmplitude(dDiffGradAmpl2);
        m_sGFwfR2.setRampUpTime(wiplSmallDelta2);
        m_sGFwfR2.setDuration(wiplSmallDelta2);
        if(!m_sGFwfR2.prep()) {
            if(bDebug) {
                std::cout << "read2 prep failed" << std::endl;
            }
            return m_sGFwfR2.getNLSStatus();
        }
        if(!m_sGFwfR2.check()) {
            if(bDebug) {
                std::cout << "read2 check failed" << std::endl;
            }
            return m_sGFwfR2.getNLSStatus();
        }

        m_vfGFwfS2 = m_readFreeDiff.getSecondInterpSliceGradient();
        m_sGFwfS2.setRampShape(&m_vfGFwfS2[0], wiplSmallDelta2/10);//, 0, true);
        m_sGFwfS2.setAmplitude(dDiffGradAmpl2);
        m_sGFwfS2.setRampUpTime(wiplSmallDelta2);
        m_sGFwfS2.setDuration(wiplSmallDelta2);
        if(!m_sGFwfS2.prep()) {
            if(bDebug) {
                std::cout << "slice prep failed" << std::endl;
            }
            return m_sGFwfS2.getNLSStatus();
        }
        if(!m_sGFwfS2.check()) {
            if(bDebug) {
                std::cout << "slice check failed" << std::endl;
            }
            return m_sGFwfS2.getNLSStatus();
        }


        if( !m_readFreeDiff.calculateGradientIntegrals() ){
            if(bDebug) {
                std::cout << "calculating b-matrix failed" << std::endl;
            }
                return SEQU_ERROR;
        }
        double dBMat[3][3] = {0};
        m_readFreeDiff.getBTensor(dBMat);
        dBValue = dBMat[0][0] + dBMat[1][1] + dBMat[2][2];

        m_myDiff.setAmplitude(dDiffGradAmpl1);
        m_myDiff.setRotationType(m_lRotType);
        m_myDiff.prepRotMats();

        //prepare gradient pulses for fixing rephasing errors between the two interpolated gradients

   
        m_sGRepMomP.setMaxMagnitude(m_dGradMaxAmpl);
        m_sGRepMomP.setMinRiseTime(m_dMinRiseTime);
        double dMomentToFix = m_sGFwfP1.getMomentum(0, m_sGFwfP1.getTotalTime()) - m_sGFwfP2.getMomentum(0, m_sGFwfP2.getTotalTime());
        if(!m_sGRepMomP.prepSymmetricTOTShortestTime(dMomentToFix)) return m_sGRepMomP.getNLSStatus();

        m_sGRepMomR.setMaxMagnitude(m_dGradMaxAmpl);
        m_sGRepMomR.setMinRiseTime(m_dMinRiseTime);
        dMomentToFix = m_sGFwfR1.getMomentum(0, m_sGFwfR1.getTotalTime()) - m_sGFwfR2.getMomentum(0, m_sGFwfR2.getTotalTime());
        if(!m_sGRepMomR.prepSymmetricTOTShortestTime(dMomentToFix)) return m_sGRepMomR.getNLSStatus();

        m_sGRepMomS.setMaxMagnitude(m_dGradMaxAmpl);
        m_sGRepMomS.setMinRiseTime(m_dMinRiseTime);
        dMomentToFix = m_sGFwfS1.getMomentum(0, m_sGFwfS1.getTotalTime()) - m_sGFwfS2.getMomentum(0, m_sGFwfS2.getTotalTime());
        if(!m_sGRepMomS.prepSymmetricTOTShortestTime(dMomentToFix)) return m_sGRepMomS.getNLSStatus();

        m_lMomentFixingTime = std::max(m_sGRepMomP.getTotalTime(),std::max(m_sGRepMomS.getTotalTime(),m_sGRepMomS.getTotalTime()));
        if(m_lMomentFixingTime > m_alTEFil[3]-lFieldCameraDelay) return SBB_NEGATIV_TEFILL;
    }//else { //if(!m_bIsFreeDiffusion) {


    rMrProt.wipMemBlock().getalFree()[WIP_lBValueDiff] = (long) floor(dBValue*1000+0.5);
    rMrProt.wipMemBlock().getalFree()[WIP_lCrushMoment] = (long) floor(dCrushMoment*1000+0.5);
    rMrProt.wipMemBlock().getalFree()[WIP_lDiffRampTime] = lDiffRampTime1;
    rMrProt.wipMemBlock().getalFree()[WIP_lNoDiffDir] = m_lNoDiffDir;
    rMrProt.wipMemBlock().getalFree()[WIP_alTEFil0] = m_alTEFil[0];
    rMrProt.wipMemBlock().getalFree()[WIP_alTEFil3] = m_alTEFil[3];
    double dBValueCrusherP = bValueMono(m_sGCrushP.getDuration(), m_sGCrushP.getRampUpTime(),
        m_sGSliRefoc.getTotalTime(), m_sGCrushP.getAmplitude(), m_sSRF01.getLarmorConst()*2*3.1415926536);
    double dBValueCrusherR = bValueMono(m_sGCrushR.getDuration(), m_sGCrushR.getRampUpTime(),
        m_sGSliRefoc.getTotalTime(), m_sGCrushR.getAmplitude(), m_sSRF01.getLarmorConst()*2*3.1415926536);
    double dBValueCrusherS = bValueSMono(m_sGCrushS.getDuration(), m_sGCrushS.getRampUpTime(),m_sGSliRefoc.getRampUpTime(),
        m_sGSliRefoc.getTotalTime(), m_sGCrushS.getAmplitude(), m_sGSliRefoc.getAmplitude(), 
        m_sSRF01.getLarmorConst()*2*3.1415926536);
    rMrProt.wipMemBlock().getalFree()[WIP_lBValueCrusher] = (long) floor((dBValueCrusherP+dBValueCrusherR+dBValueCrusherS)*1000+0.5);
    if (bDebug) {
        std::cout << "***Crusher and Diffusion***" << std::endl;
        std::cout << "min amplitude for crushing with diffusion: " << dCrushMoment/wiplSmallDelta << std::endl;
        std::cout << "b-value of diffusion weighting: " << dBValue << std::endl;
        std::cout << "diffusion gradient: " << lDiffRampTime1 << " / " << wiplSmallDelta << std::endl; 
        std::cout << "Crusher b-values: P: " << dBValueCrusherP << ", R: " << dBValueCrusherR << 
            ", S" << dBValueCrusherS << std::endl;
        std::cout << "Crusher P: " << m_sGCrushP.getRampUpTime() << " / " << m_sGCrushP.getDuration() <<
            " / " << m_sGCrushP.getAmplitude() << " --> " << dCrushMoment << std::endl;
        std::cout << "Crusher R: " << m_sGCrushR.getRampUpTime() << " / " << m_sGCrushR.getDuration() <<
            " / " << m_sGCrushR.getAmplitude() << " --> " << dCrushMoment << std::endl;
        std::cout << "Crusher S: " << m_sGCrushS.getRampUpTime() << " / " << m_sGCrushS.getDuration() <<
            " / " << m_sGCrushS.getAmplitude() << " --> " << dCrushMoment << std::endl;
    }

//******************************************************************************************************
    //Final calculations and preparations
//******************************************************************************************************

    //. Prepare and check spoiler gradient
    //I might like to change the spoil moment, so let's not give this to seqLoop yet
    m_sGSpoil.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSpoil.setMinRiseTime (m_dMinRiseTime);
	if(!m_sGSpoil.prepSymmetricTOTShortestTime(dCrushMoment))
		return (m_sGSpoil.getNLSStatus());
    if(bNoExcGrad || bNoRefocGrad || bNoReadGrad) {
        if(!m_sGSpoil.prepAmplitude(m_sGSpoil.getRampUpTime(), m_sGSpoil.getDuration(), m_sGSpoil.getRampDownTime(), 0.0))
            return m_sGSpoil.getNLSStatus();
    }
    if(!m_sGSpoil.check()) return m_sGSpoil.getNLSStatus();
    //SeqLoop start
    //long lSpoilDelay = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadoutBeforeEcho_Pos, 1);
    //SBB uses TRmin here, with the additional SBBs before the kernel

	long lScanTime = lScanTimeSBBs + fSDSRoundUpGRT(m_sGSliSel.getTotalTime()/2) + rMrProt.te()[0] + m_alTEFil[4]
        + m_lReadoutDuration * wiplNoReadouts // __EK__ 
		+ std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime()) * (wiplNoReadouts-1)// __EK__  spiral refocussing
		+ m_alTEFil[5] + m_alTEFil[6] + m_alTEFil[7] + m_alTEFil[8]  // __EK__ fill times between readouts, perhaps more needed in case of more than 5 readouts
		- m_lReadoutBeforeEcho + m_sGSpoil.getTotalTime();
	if (bDebug) { std::cout << "SCAN TIME: " << lScanTime << std::endl; }

    if (wipbVibrationGrad) {
        lScanTime += m_alTEFil[10] + wiplSmallDelta + lDiffRampTime1;
    }
        //+ lSpoilDelay + m_sGSpoil.getTotalTime(); ///< Duration of the sequence kernel plus the duration of all saturation pulses
    ///< or any other SBB applied between an IR or SR preparation pulse and the end
    ///< of TR interval. lScanTime does not include the duration of the IR or SR
    ///< pulses.
    long lTIMinAdd1 = lScanTimeSBBs; //getlSBBScanTime() ///< Duration of all saturation pulses and any other SBB applied between an IR
    ///< or SR preparation pulse and the kernel for non-interleaved IR

    if (wipbVibrationGrad) {
        lTIMinAdd1 += m_alTEFil[10] + wiplSmallDelta + lDiffRampTime1;
    }
    long lTIMinAdd2 =lTIMinAdd1;     ///< Duration of all saturation pulses and any other SBB applied between an IR
    ///< or SR preparation pulse and the kernel for interleaved IR

    //here seems to be only kernel plus SBBs after: lScanTime = lDummyScanTime + SBB times before kernel(except IR)?
    long lDummyScanTime = lScanTime;     ///< Duration of the kernel and all SBBs applied after ther kernel
    long lDummySBBTime = lTIMinAdd1;     ///< Duration of all saturation pulses or any other SBB between an IR or SR
    ///< preparation pulse and the kernel
    long lNegativeFillTime = 0;
    bool bSuccess = m_mySeqLoop.TrTiFillTimes (rMrProt, rSeqLim, rSeqExpo,  
        lScanTime, 1 ,   ///< Number of TR fills per TR period (single slice?)
        lTIMinAdd1, lTIMinAdd2, lDummyScanTime, lTIMinAdd1,   
        &lNegativeFillTime);

    if(bDebug) {
        std::cout << "lNegativeFillTime: " << lNegativeFillTime << std::endl;
	}
    long lNeededTI = 0;
    long lNeededTR = 0;
    #ifdef WIN32
        if(!fUICEvaluateSeqLoopTrTiFillTimes(rMrProt, rSeqLim, bSuccess, m_mySeqLoop.getlTIneeded(), m_mySeqLoop.getlTRneeded(), &lNeededTI, &lNeededTR)){
            return m_mySeqLoop.getNLSStatus();
        }
        if(bDebug) {
            std::cout << "lNeededTr: " << lNeededTR << std::endl;
            std::cout << "lNeededTI: " << lNeededTI << std::endl;
        }
        if (lNeededTR!=rMrProt.getalTR()[0] || lNeededTI!=rMrProt.getalTI()[0])
        {
            return SEQU_ERROR;
        }
    #endif
    
    if(bDebug) {
        std::cout << "*****Timing Parameters*****" << std::endl;
        std::cout << "m_sGSliSel.getTotalTime()" << m_sGSliSel.getTotalTime() << std::endl;
        std::cout << "m_sGSliSelReph.getTotalTime()" << m_sGSliSelReph.getTotalTime() << std::endl;
        std::cout << "m_sGSliRefoc.getTotalTime()" << m_sGSliRefoc.getTotalTime() << std::endl;
        std::cout << "m_lReadoutBeforeEcho: " << m_lReadoutBeforeEcho << std::endl;
        std::cout << "m_sGSpiral.getTotalTime(): " << m_sGSpiralR.getTotalTime() << std::endl;
        std::cout << "m_sADC01.getDuration()" << m_sADC01.getDuration() << std::endl;
        std::cout << "rMrProt.te()[0]: " << rMrProt.te()[0] << std::endl;
        //std::cout << "lTRMin: " << lTRmin << std::endl;
		std::cout << "rMrProt.tr()[0]: " << rMrProt.tr()[0] << std::endl;
        std::cout << "dMeasureTimeUsec: " <<  m_mySeqLoop.getMeasurementTimeUsec(rMrProt, rSeqLim) << std::endl;
        std::cout << "m_sSRF01.getLarmorConst():" << std::setprecision(12) << m_sSRF01.getLarmorConst() << "\n" << std::endl;
    }

    // Set receiver gain
    OnErrorReturn(fSSLSetRxGain(K_RX_GAIN_CODE_HIGH,rMrProt,rSeqLim));
    // Prepare the slice position array
    OnErrorReturn(fSUPrepSlicePosArray (rMrProt, rSeqLim, m_asSLC));

//******************************************************************************************************
    //prepare for sync scans
//******************************************************************************************************
	m_lSyncTr = 300000;
	m_lAfterSyncTr = 3000000; //3 seconds needed for FC to switch
    long lSyncScanDuration = fSDSRoundUpGRT(m_sGSliSel.getTotalTime() / 2) + rMrProt.te()[0] + m_alTEFil[4]
        + m_lReadoutDuration - m_lReadoutBeforeEcho + m_sGSpoil.getTotalTime() + m_lSyncTr;
	double dMeasureTimeUsec = m_mySeqLoop.getMeasurementTimeUsec(rMrProt, rSeqLim) + lSyncScanDuration*wiplSyncScans;
	if(wiplSyncScans > 0) {
		dMeasureTimeUsec += m_lAfterSyncTr;
	}

//******************************************************************************************************
    //Set SeqExpo values
//******************************************************************************************************
    
    // Set sequence string for display in images
    fSUSetSequenceString              ("LMSpi", rMrProt, rSeqExpo); //LM
    
    rSeqExpo.setMeasureTimeUsec (dMeasureTimeUsec); //SeqLoop
    rSeqExpo.setTotalMeasureTimeUsec (dMeasureTimeUsec); //SeqLoop
    rSeqExpo.setRelevantReadoutsForMeasTime (m_mySeqLoop.getNumberOfRelevantADCs());    //SeqLoop
    //. Fill export section
    rSeqExpo.setNSet(m_lNoDiffDir);
    rSeqExpo.setNIda(wiplNoReadouts);
    rSeqExpo.setRFEnergyInSequence_Ws(dRfEnergyInSRFs + dEnergyInSBBs + m_mySeqLoop.getEnergy(rMrProt));
    rSeqExpo.setMeasuredPELines      (m_lLinesToMeasure);
    //rSeqExpo.setMeasureTimeUsec      (dMeasureTimeUsec);
    //rSeqExpo.setTotalMeasureTimeUsec (dMeasureTimeUsec*m_lRepetitions);
    //m_mySeqLoop.setbSwitchToOffline(true) //don't know if this helps
    rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_NONE);
    rSeqExpo.setOnlineFFT            (SEQ::ONLINE_FFT_NONE); //LM#
    #ifdef WIN32
        SeqUT.setSizeOfDimSet(m_lNoDiffDir);
    #endif
	rSeqExpo.setICEProgramFilename ("%SiemensIceProgs%\\IceProgramOnline2D");

    //disable some tests in the ut, that should not be passed anyway
    #ifdef WIN32
        SeqUT.DisableTestCase(lGpFirstMomentNotRephasedErr, RTEB_ORIGIN_fSEQRunKernel,
            "Not really trying to be flow compensated.");
       /* SeqUT.DisableTestCase(lTIClockErr, RTEB_ClockCheck,
            "der unit test ist zu bloed, TI richtig zu berechnen...\n"
            "man muss noch die pulsdauer und rampuptime beruecksichtigen\n");*/
        SeqUT.DisableTestCase(lGrNotInRoErr, RTEB_ORIGIN_fSEQRunKernel,
            "This is a Spiral sequence and must have gradients on two axes.");
    #endif


    return (lStatus);
}

//. -------------------------------------------------------------------------
//.. fSEQCheck: check lines at the border of k-space (gradient overflow, GSWD)
//. -------------------------------------------------------------------------
NLSStatus lm_Spiral::check(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo, SEQCheckMode * /*pSEQCheckMode*/)
{   //currently checks all diffusion directions 
    
    static const char *ptModule = {"lm_Spiral::check"};
    NLS_STATUS   lStatus = SEQU__NORMAL;

    m_mySeqLoop.setlinesToCheck(1);
    m_mySeqLoop.setlineNoToCheck(0, 0);
    m_mySeqLoop.setpartitionsToCheck   (1);
    m_mySeqLoop.setparNoToCheck        (0, 0);
    //apparently SeqLoop does not loop over the free loop counter in check, but I really wanna do that at the moment,
    // so let's try to do it this way. To communicate the current run of this loop, we need a member variable.
    m_lDiffLoopCounterForCheck = 0;
    while (m_lDiffLoopCounterForCheck < 1/*m_lNoDiffDir*/)
    {
        if(bDebug) {
            std::cout << "fSEQcheck: " << m_lDiffLoopCounterForCheck << std::endl;
        }
        if(!m_mySeqLoop.check(NULL, rMrProt, rSeqLim,
            rSeqExpo, m_asSLC, &m_sADC01) ) {
                lStatus = m_mySeqLoop.getNLSStatus();
                break;
        }
        m_lDiffLoopCounterForCheck++;
    }
    

    return(lStatus);
}

//. -------------------------------------------------------------------------
//.. fSEQRun: run the sequence
//. -------------------------------------------------------------------------
NLSStatus lm_Spiral::run(MrProt  &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo)
{
    static const char *ptModule = {"lm_Spiral::run"};
    NLS_STATUS lStatus          = SEQU__NORMAL;

    //. Send the delay between measurements
    //OnErrorReturn(fSBBMeasRepetDelaysRun ( rMrProt, rSeqLim, rSeqExpo, 0 ));
    // Fill some entries of the measurement data header
    //m_sADC01.getMDH().setEvalInfoMask (m_sADC01.getMDH().getEvalInfoMask() | MDH_OFFLINE) ; //LM
    m_sADC01.getMDH().setKSpaceCentreLineNo    ((unsigned short)rMrProt.kSpace().echoLine());
    m_sADC01.getMDH().setKSpaceCentrePartitionNo (0);

	m_sADC02.getMDH().setKSpaceCentreLineNo    ((unsigned short)rMrProt.kSpace().echoLine());
    m_sADC02.getMDH().setKSpaceCentrePartitionNo (0);
	m_sADC03.getMDH().setKSpaceCentreLineNo    ((unsigned short)rMrProt.kSpace().echoLine());
    m_sADC03.getMDH().setKSpaceCentrePartitionNo (0);
	m_sADC04.getMDH().setKSpaceCentreLineNo    ((unsigned short)rMrProt.kSpace().echoLine());
    m_sADC04.getMDH().setKSpaceCentrePartitionNo (0);
	m_sADC05.getMDH().setKSpaceCentreLineNo    ((unsigned short)rMrProt.kSpace().echoLine());
    m_sADC05.getMDH().setKSpaceCentrePartitionNo (0);
   // if(IS_UNIT_TEST_ACTIVE(rSeqLim))
   // {
    mSEQTest(rMrProt,rSeqLim,rSeqExpo,RTEB_ORIGIN_fSEQRunStart,0,0,0,0,0);
       // mSEQTest (rMrProt, rSeqLim, rSeqExpo, RTEB_ClockInitTR, 0, 0, m_asSLC[0].getSliceIndex(), 0, 0);
   // } 
	m_sADC01.getMDH().addToEvalInfoMask(MDH_PHASCOR); 
    for (long lI = 0; lI < m_WIPParamTool.getLongValue(rMrProt, WIP_lSliceToTrigger_Pos, 1); ++lI) {
        m_sADC01.getMDH().setClin(lI);
        runKernel(rMrProt, rSeqLim, rSeqExpo, KERNEL_PHASECOR, -1, 0, 0);
    }
    m_sADC01.getMDH().deleteFromEvalInfoMask(MDH_PHASCOR);

    if(! m_mySeqLoop.run( NULL, //(NLS_STATUS (*)(TYPESFor_fSEQRunKernel))fSEQRunKernel,
        rMrProt,
        rSeqLim,
        rSeqExpo,
        m_asSLC,
        &m_sADC01)
        ) lStatus=m_mySeqLoop.getNLSStatus();
   
    //if(IS_UNIT_TEST_ACTIVE(rSeqLim))
    //{
        mSEQTest(rMrProt,rSeqLim,rSeqExpo,RTEB_ORIGIN_fSEQRunFinish,0,0,0,0,0);
    //}
    return(lStatus);
}


//. -------------------------------------------------------------------------
//.. runKernel
//. -------------------------------------------------------------------------
NLS_STATUS lm_Spiral::runKernel(MrProt &rMrProt, SeqLim &rSeqLim,
                             MrProtocolData::SeqExpo &rSeqExpo, long lKernelMode, long lSlice, long /*lPartition*/, long lLine)
{
    long lI = 0, lJ=0, lK=0;
    long lDir = m_mySeqLoop.getDiffusionLoopCounter();
    if (lKernelMode == KERNEL_CHECK){
        lDir = m_lDiffLoopCounterForCheck;
    }

	long wiplNoReadouts = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 0);

    long lFieldCameraDelay  = 510; //after the dynamic field camera gets a trigger it takes 505us to measure
    
    fRTSetGPEnable(!m_bNoGradients);
    fRTSetGREnable(!m_bNoGradients);
    fRTSetGSEnable(!m_bNoGradients);
    fRTSetRFShapeEnable(!m_bNoRFpulse);

    bool isSyncRun = false;
    if (lSlice < 0) {
        isSyncRun = true;
        fRTSetGPEnable(false);
        fRTSetGREnable(false);
        fRTSetGSEnable(false);
        fRTSetRFShapeEnable(false);
        lSlice = 0;
    }

    //to tell the unit test where we are
    unsigned long ulTestIdent = RTEB_ORIGIN_fSEQRunKernel;
    if (lKernelMode == KERNEL_CHECK) ulTestIdent = RTEB_ORIGIN_fSEQCheck;

    if(bDebug) {
        if (lKernelMode == KERNEL_CHECK) {
            std::cout << "Kernel check: ";
        } else {
            std::cout << "Kernel run: "; 
        }
        std::cout << "lSlice=" << lSlice << ", DiffDir=" << lDir << std::endl;
    }
    //long lLine = 0; //I used the line loop for reps so its always the first line
    static const char *ptModule = {"lm_Spiral::runKernel"};
    NLS_STATUS         lStatus  = SEQU__NORMAL;
    
    //. Prepare FrequencyPhase objects of the RF pulse
    m_sSRF01zSet.prepSet (m_asSLC[lSlice], m_sSRF01) ;
    m_sSRF01zNeg.prepNeg (m_asSLC[lSlice], m_sSRF01) ;
    m_sSRF02zSet.prepSet (m_asSLC[lSlice], m_sSRF02) ;
    m_sSRF02zNeg.prepNeg (m_asSLC[lSlice], m_sSRF02) ;

    //. Prepare FrequencyPhase objects of the ADC
    /*m_sADC01zSet.prepSet (m_asSLC[lSlice], m_sADC01, m_sGSpiralP) ; //LM
    m_sADC01zNeg.prepNeg (m_asSLC[lSlice], m_sADC01, m_sGSpiralP) ; //LM
    
	m_sADC02zSet.prepSet (m_asSLC[lSlice], m_sADC02, m_sGSpiralP) ; //LM
    m_sADC02zNeg.prepNeg (m_asSLC[lSlice], m_sADC02, m_sGSpiralP) ; //LM
	m_sADC03zSet.prepSet (m_asSLC[lSlice], m_sADC03, m_sGSpiralP) ; //LM
    m_sADC03zNeg.prepNeg (m_asSLC[lSlice], m_sADC03, m_sGSpiralP) ; //LM
	m_sADC04zSet.prepSet (m_asSLC[lSlice], m_sADC04, m_sGSpiralP) ; //LM
    m_sADC04zNeg.prepNeg (m_asSLC[lSlice], m_sADC04, m_sGSpiralP) ; //LM
	m_sADC05zSet.prepSet (m_asSLC[lSlice], m_sADC05, m_sGSpiralP) ; //LM
    m_sADC05zNeg.prepNeg (m_asSLC[lSlice], m_sADC05, m_sGSpiralP) ; //LM */
//rotation matrix for free diffusion waveform   
    sROT_MATRIX sDiffRotMatrix;
    sDiffRotMatrix = m_asSLC[lSlice].getROT_MATRIX();
    if (m_myDiff.getCoordinateSystem() == XYZ) {
        for(lI=0; lI<3; ++lI) {
            for(lJ=0; lJ<3; ++lJ) {
                sDiffRotMatrix.dMat[lI][lJ] = static_cast<double>(lI==lJ);
            }
        }
    }
    double dP=0., dR=0., dS=0.;
    double wipdDiffGradAmpl1 = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffGradAmpl_Pos, 0);
    double wipdDiffGradAmpl2 = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffGradAmpl_Pos, 1);  
    double dDiffNorm = m_myDiff.getSquareRowNorm(lDir);
    if(bDebug) {
        std::cout << "diffAmpl: " << wipdDiffGradAmpl1 << ", " << wipdDiffGradAmpl2 << ", dDiffNorm: " << dDiffNorm << std::endl;
    }
    if(m_bIsFreeDiffusion){   
        double aadRotNew[3][3] = {0};
        double aadRotOld[3][3] = {0};
        for(lI=0; lI<3; ++lI) {
            for(lJ=0; lJ<3; ++lJ) {
                aadRotOld[lI][lJ] = sDiffRotMatrix.dMat[lI][lJ];
                aadRotNew[lI][lJ] = sDiffRotMatrix.dMat[lI][lJ];
            }
        }
        double aadBuf[3][3] = {0};
        double aadBMat[3][3] = {0};
        m_myDiff.getRotMat(lDir, aadBuf);
        //rotate the b matrix for saving purposes
        m_readFreeDiff.getBTensor(aadBMat);
        MatMult(aadBuf, aadBMat, aadRotNew);
        MatMultTrans(aadRotNew, aadBuf, aadBMat);
        double dScaling = 1;
        if(m_myDiff.isScaledGradients()) {
            for(lI=0; lI<3; ++lI) {
                for(lJ=0; lJ<3; ++lJ) {
                    aadBMat[lI][lJ] *= (dDiffNorm*dDiffNorm);
                }
            }
            dScaling = dDiffNorm;     
        }
        m_sGFwfP1.setAmplitude(wipdDiffGradAmpl1*dScaling);
        if(!m_sGFwfP1.prep() || !m_sGFwfP1.check()) {
            return m_sGFwfP1.getNLSStatus();
        }
        m_sGFwfR1.setAmplitude(wipdDiffGradAmpl1*dScaling);
        if(!m_sGFwfR1.prep() || !m_sGFwfR1.check()) {
            return m_sGFwfR1.getNLSStatus();
        }
        m_sGFwfS1.setAmplitude(wipdDiffGradAmpl1*dScaling);
        if(!m_sGFwfS1.prep() || !m_sGFwfS1.check()) {
            return m_sGFwfS1.getNLSStatus();
        }
        m_sGFwfP2.setAmplitude(wipdDiffGradAmpl2*dScaling);
        if(!m_sGFwfP2.prep() || !m_sGFwfP2.check()) {
            return m_sGFwfP2.getNLSStatus();
        }
        m_sGFwfR2.setAmplitude(wipdDiffGradAmpl2*dScaling);
        if(!m_sGFwfR2.prep() || !m_sGFwfR2.check()) {
            return m_sGFwfR2.getNLSStatus();
        }
        m_sGFwfS2.setAmplitude(wipdDiffGradAmpl2*dScaling);
        if(!m_sGFwfS2.prep() || !m_sGFwfS2.check()) {
            return m_sGFwfS2.getNLSStatus();
        }
        double dMomentToFix = m_sGFwfP1.getMomentum(0, m_sGFwfP1.getTotalTime()) - m_sGFwfP2.getMomentum(0, m_sGFwfP2.getTotalTime());
        if(!m_sGRepMomP.prepSymmetricTOTShortestTime(dMomentToFix)) return m_sGRepMomP.getNLSStatus();
        dMomentToFix = m_sGFwfR1.getMomentum(0, m_sGFwfR1.getTotalTime()) - m_sGFwfR2.getMomentum(0, m_sGFwfR2.getTotalTime());
        if(!m_sGRepMomR.prepSymmetricTOTShortestTime(dMomentToFix)) return m_sGRepMomR.getNLSStatus();
        dMomentToFix = m_sGFwfS1.getMomentum(0, m_sGFwfS1.getTotalTime()) - m_sGFwfS2.getMomentum(0, m_sGFwfS2.getTotalTime());
        if(!m_sGRepMomS.prepSymmetricTOTShortestTime(dMomentToFix)) return m_sGRepMomS.getNLSStatus();

        m_myDiff.setExternalBMatrix(aadBMat);
        //get the rotation matrix 
        //aadRotOld is the original slice rotation
        //aadBuf is the rotation for the diffusion encoding in PRS
        //By multiplying both matrices, we first rotate in PRS and then rotate to scanner system
        MatMult(aadRotOld, aadBuf, aadRotNew); 
        for(lI=0; lI<3; ++lI) {
            for(lJ=0; lJ<3; ++lJ) {
                sDiffRotMatrix.dMat[lI][lJ]= aadRotNew[lI][lJ];
            }
        }
    } else {//if(m_bIsFreeWaveform) 
        m_myDiff.getDirectionFactors(lDir, dP, dR, dS);
        if(!m_sGDiffP.prepAmplitude(wipdDiffGradAmpl1*dP)) return m_sGDiffP.getNLSStatus();
        if(!m_sGDiffR.prepAmplitude(wipdDiffGradAmpl1*dR)) return m_sGDiffR.getNLSStatus();
        if(!m_sGDiffS.prepAmplitude(wipdDiffGradAmpl1*dS)) return m_sGDiffS.getNLSStatus();
    }

    bool bRunCrushers = false;
    long lSliSelRampTime = m_lSliRampTimeOrig;
    //double dDiffGradAmpl2 = m_WIPParamTool.getDoubleValue(rMrProt, WIP_dDiffGradAmpl_Pos,1);
    if( m_myDiff.getSquareRowNorm(lDir) < 0.0001 ||
        (!m_bIsFreeDiffusion && wipdDiffGradAmpl1 < 0.005) ) {
        bRunCrushers = true;
        //lSliSelRampTime = std::min(m_lSliRampTimeOrig, m_sGCrushS.getRampUpTime()); 
    }
    //prepare the actual slice refocusing gradient
    /*if(bNoRefocGrad) {
        if(!m_sGSliRefoc.prepAmplitude(lSliSelRampTime, lSliSelRampTime+m_sGSliRefoc.getFlatTopTime(),
            lSliSelRampTime, 0.0 ))return(m_sGSliRefoc.getNLSStatus());
    }else {
        if(!m_sGSliRefoc.prepAmplitude(lSliSelRampTime, lSliSelRampTime+m_sGSliRefoc.getFlatTopTime(),
            lSliSelRampTime, m_sSRF02.getGSAmplitude()))return(m_sGSliRefoc.getNLSStatus());
    }*/
    if (! m_sGSliRefoc.check() ) return (m_sGSliRefoc.getNLSStatus());

    m_sADC01.getMDH().setCset((unsigned short) lDir);
    // m_sADC01.getMDH().setClin            ((unsigned short)lLine);
	m_sADC01.getMDH().setLastScanInConcat(lDir == m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 0)-1);
	m_sADC01.getMDH().setLastScanInMeas(lDir == m_WIPParamTool.getLongValue(rMrProt, WIP_lReadMultiEcho_Pos, 0)-1);
    m_sADC01.getMDH().setPhaseFT         (false);

	//write the bMatrix in the MDH
    m_myDiff.writeBMatrix(lDir, m_sADC01, rMrProt.sliceSeries()[static_cast<int32_t>( m_asSLC->getSliceIndex() )],
        m_asSLC[lSlice]);//, bDebug);

	m_sADC02.getMDH() = m_sADC01.getMDH();
	m_sADC02.getMDH().setCeco(1);
	m_sADC03.getMDH() = m_sADC01.getMDH();
	m_sADC03.getMDH().setCeco(2);
	m_sADC04.getMDH() = m_sADC01.getMDH();
	m_sADC04.getMDH().setCeco(3);
	m_sADC05.getMDH() = m_sADC01.getMDH();
	m_sADC05.getMDH().setCeco(4);
//m_sADC01.getMDH().setFirstScanInSlice(first_scan_sl && lEcho == 0) ;
			//m_sADC01.getMDH().setLastScanInSlice(last_scan_sl && (lEcho == (wiplNoReadouts-1))) ;
//			m_sADC01.getMDH().setLastScanInConcat(last_scan_concat && (lEcho == (wiplNoReadouts-1))) ;
//			m_sADC01.getMDH().setLastScanInMeas(last_scan_meas && (lEcho == (wiplNoReadouts-1))) ;
	switch(wiplNoReadouts) {
	case 2:
		//m_sADC02.getMDH().setLastScanInSlice(m_sADC01.getMDH().isLastScanInSlice());
		m_sADC02.getMDH().setLastScanInConcat(m_sADC01.getMDH().isLastScanInConcat());
		m_sADC02.getMDH().setLastScanInMeas(m_sADC01.getMDH().isLastScanInMeas());
		//m_sADC01.getMDH().setLastScanInSlice(false);
		m_sADC01.getMDH().setLastScanInConcat(false);
		m_sADC01.getMDH().setLastScanInMeas(false);
	case 3:
		//m_sADC03.getMDH().setLastScanInSlice(m_sADC02.getMDH().isLastScanInSlice());
		m_sADC03.getMDH().setLastScanInConcat(m_sADC02.getMDH().isLastScanInConcat());
		m_sADC03.getMDH().setLastScanInMeas(m_sADC02.getMDH().isLastScanInMeas());
		//m_sADC02.getMDH().setLastScanInSlice(false);
		m_sADC02.getMDH().setLastScanInConcat(false);
		m_sADC02.getMDH().setLastScanInMeas(false);
	case 4:
		//m_sADC04.getMDH().setLastScanInSlice(m_sADC03.getMDH().isLastScanInSlice());
		m_sADC04.getMDH().setLastScanInConcat(m_sADC03.getMDH().isLastScanInConcat());
		m_sADC04.getMDH().setLastScanInMeas(m_sADC03.getMDH().isLastScanInMeas());
		//m_sADC03.getMDH().setLastScanInSlice(false);
		m_sADC03.getMDH().setLastScanInConcat(false);
		m_sADC03.getMDH().setLastScanInMeas(false);
	case 5:
		//m_sADC05.getMDH().setLastScanInSlice(m_sADC04.getMDH().isLastScanInSlice());
		m_sADC05.getMDH().setLastScanInConcat(m_sADC04.getMDH().isLastScanInConcat());
		m_sADC05.getMDH().setLastScanInMeas(m_sADC04.getMDH().isLastScanInMeas());
		//m_sADC04.getMDH().setLastScanInSlice(false);
		m_sADC04.getMDH().setLastScanInConcat(false);
		m_sADC04.getMDH().setLastScanInMeas(false);
	}
	//m_sADC02.getMDH().setFirstScanInSlice(false);
	//m_sADC03.getMDH().setFirstScanInSlice(false);
	//m_sADC04.getMDH().setFirstScanInSlice(false);
	//m_sADC05.getMDH().setFirstScanInSlice(false);

    

    //Now we need to rotate the spiral for the interleaves, but what we will actually do:
    //is rotate the rotation matrix
    sROT_MATRIX sNewRotationMatrix;
    double aadRot[3][3];
        
   //copy the original rotation matrix
    sNewRotationMatrix = m_asSLC[lSlice].getROT_MATRIX();
   
    //determine the rotation needed for the current line
     double dRotAngle = 2.0*3.141592653559* lLine/m_lLinesToMeasure;
    aadRot[0][0] = cos(dRotAngle); aadRot[0][1] =-sin(dRotAngle); aadRot[0][2] = 0.;
    aadRot[1][0] = sin(dRotAngle); aadRot[1][1] = cos(dRotAngle); aadRot[1][2] = 0.;
    aadRot[2][0] = 0.;             aadRot[2][1] = 0.;             aadRot[2][2] = 1.;

    //now lets rotate:
    for(lI=0; lI<3; ++lI) {
        for(lJ=0; lJ<3; ++lJ) {
            sNewRotationMatrix.dMat[lI][lJ] = 0.;
            for(lK=0; lK<3; ++lK){
                sNewRotationMatrix.dMat[lI][lJ] += m_asSLC[lSlice].getROT_MATRIX().dMat[lI][lK]*aadRot[lK][lJ];
            }
        }
    }

    if(bDebug) {
        std::cout << "DiffFactors: " << dP << ", " << dR << ", " << dS << std::endl;
        printMatrix(sDiffRotMatrix.dMat, "diffRotMat");
        printMatrix(sNewRotationMatrix.dMat, "SpiralRotMat");
    }

    long lTRFill = fSDSRoundDownGRT(m_mySeqLoop.getlTRFill()); 
	long lRefocDelay = std::max(m_sGSpiralRefocP.getTotalTime(),m_sGSpiralRefocR.getTotalTime()) ; //__EK__

    //long lSpoilDelay = m_WIPParamTool.getLongValue(rMrProt, WIP_lReadoutBeforeEcho_Pos, 1);
    
    //I need to use at least two event blocks, as only the readout should use the rotated rotation matrix
    //- ***************************************************** S E Q U E N C E   T I M I N G **********************************************************************
    //- *           Start Time             |      NCO     |    SRF   |    ADC   |              Gradient Events               |  Sync
    //- *             (usec)               |     Event    |   Event  |   Event  |    phase    |    read     |      slice     |  Event
    //- *fRTEI(                            ,              ,          ,          ,             ,             ,                ,          );
    //- **********************************************************************************************************************************************************
    long lT = 0; 
    long lTotalTime = 0;
    
    if(m_IRselSBB.getDurationPerRequest() + m_IRnsSBB.getDurationPerRequest() <= 0 && !isSyncRun) {
        fRTEBInit(m_asSLC[lSlice].getROT_MATRIX());
        fRTEI (  lTRFill,             0,         0,         0,   &m_sGSpoil,        &m_sGSpoil,      &m_sGSpoil,          0); //LM
        fRTEI (lTRFill + m_sGSpoil.getTotalTime()  , 0,0,0, 0,0,0, 0);
        mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     10, lLine, m_asSLC[lSlice].getSliceIndex(), 0, 0);
        OnErrorReturn((NLS_STATUS)fRTEBFinish());
    }

    if(rMrProt.preparationPulses().getucMTC() && !isSyncRun){
        if (!m_IRselSBB.run(rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice])) return m_IRselSBB.getNLSStatus();
        if (!m_IRnsSBB.run(rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice])) return m_IRnsSBB.getNLSStatus();
        if (m_IRselSBB.getDurationPerRequest() + m_IRnsSBB.getDurationPerRequest() > 0) {
            fRTEBInit(m_asSLC[lSlice].getROT_MATRIX());
            fRTEI(rMrProt.ti()[1] - m_IRselSBB.getDurationPerRequest()
                - m_IRnsSBB.getDurationPerRequest() - m_lScanTimeSBBs, 0, 0, 0, 0, 0, 0, 0);
            mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent, 10, 0, m_asSLC[lSlice].getSliceIndex(),
                0, 0);
            OnErrorReturn(lStatus = fRTEBFinish());
            lTotalTime += (rMrProt.ti()[1] - m_lScanTimeSBBs);
        }
    }

    if (!isSyncRun) {
        for (lI = MAXRSATS - 1; lI >= 0; lI--) // send RSats in inverse order                            //RSAT
        {                                                                                        //RSAT
            if (!m_RSatSBB[lI].run(rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice]))                    //RSAT
                return (m_RSatSBB[lI].getNLSStatus());                                            //RSAT
        }
        if (!m_CSatSBB.run(rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice])) return m_CSatSBB.getNLSStatus();
        lTotalTime += m_lScanTimeSBBs;
    }
    if (m_WIPParamTool.getBoolValue(rMrProt, WIP_bVibrationGrad_Pos)) {
        fRTEBInit(sDiffRotMatrix, true);
        fRTEI(0, 0, 0, 0, &m_sGDiffP, &m_sGDiffR, &m_sGDiffS, 0);
        fRTEI(m_sGDiffP.getTotalTime()+m_alTEFil[5], 0, 0, 0, 0, 0, 0, 0);
		mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     500, 0, m_asSLC[lSlice].getSliceIndex(), 0, 0);
		OnErrorReturn((NLS_STATUS)fRTEBFinish());    
    }

    fRTEBInit( m_asSLC[lSlice].getROT_MATRIX());
    if(bDebug) {
        std::cout << "excitation event block" << std::endl;
    }
    //fRTEBInit(sNewRotationMatrix, true);
    lT = 0;
//excitation
    if(m_IRselSBB.getDurationPerRequest() + m_IRnsSBB.getDurationPerRequest() > 0 ) {
        fRTEI (  lT,             0,         0,         0,   &m_sGSpoil,        &m_sGSpoil,      &m_sGSpoil,          0);
        lT += m_sGSpoil.getTotalTime();
    }
	fRTEI ( lT                              ,             0,         0,         0,            0,            0,     &m_sGSliSel,         0);
    fRTEI ( lT+m_sGSliSel.getTotalTime()/2 - m_sSRF01.getDuration()/2, &m_sSRF01zSet, &m_sSRF01,         0,            0,            0,               0,         0);
    fRTEI ( lT+m_sGSliSel.getTotalTime()/2 + m_sSRF01.getDuration()/2, &m_sSRF01zNeg,         0,         0,            0,            0,               0,         0);
    fRTEI ( lT+=m_sGSliSel.getTotalTime()   ,             0,         0,         0,            0,            0, &m_sGSliSelReph,         0);

    lT += m_alTEFil[0] + m_sGSliSelReph.getTotalTime();

    fRTEI(lT,0,0,0, 0,0,0, 0);
    mSEQTest(rMrProt, rSeqLim, rSeqExpo, RTEB_ClockCheck, 20, 0, m_asSLC[lSlice].getSliceIndex(), 0, 0);
    mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     301, 0, m_asSLC[lSlice].getSliceIndex(), 0, 0);
    OnErrorReturn((NLS_STATUS)fRTEBFinish());
//first diffusion gradient
    
    fRTEBInit(sDiffRotMatrix, true);
    lT = 0;
    if(bDebug) {
        std::cout << "first diffusion event block" << std::endl;
        std::cout << "m_sGDiffP.getTotalTime()" << m_sGDiffP.getTotalTime() << std::endl;
        std::cout << "m_sGFwfP1.getTotalTime()" << m_sGFwfP1.getTotalTime() << std::endl;
        std::cout << "m_bIsFreeDiffusion: " << m_bIsFreeDiffusion << ", bRunCrushers: " << bRunCrushers << std::endl;
    }
    //if(!bRunCrushers || ){
    lT += m_sGDiffP.getTotalTime();
    if(bDebug) std::cout << " lT = " << lT << std::flush; 
    if(!m_bIsFreeDiffusion && !bRunCrushers) {
        fRTEI (lT-m_sGDiffP.getTotalTime(),             0,         0,         0,   &m_sGDiffP,   &m_sGDiffR,     &m_sGDiffS,          0);  
    } else if(m_bIsFreeDiffusion) {
        lT -= m_sGDiffP.getTotalTime();
        fRTEI (lT     ,             0,         0,         0,   &m_sGFwfP1,   &m_sGFwfR1,     &m_sGFwfS1,          0);
        lT += m_sGFwfP1.getTotalTime();
    }
    if(bDebug) std::cout << " lT = " << lT << std::flush; 
    //}
    lT +=  m_alTEFil[1];
    if(bDebug) std::cout << " lT = " << lT << std::endl; 
    if(bRunCrushers || (m_bIsFreeDiffusion && m_bCrusherInFwf)) {
        if(bDebug) std::cout << " lT- m_lCrushTime = " << lT- m_lCrushTime << std::flush; 
        fRTEI (lT - m_lCrushTime /*+ m_lSliRampTimeOrig*/,             0,         0,         0,  &m_sGCrushP,  &m_sGCrushR,     &m_sGCrushS,         0);
    }

    fRTEI(lT,0,0,0, 0,0,0, 0);
    if(bDebug) {
        std::cout << "first diffusion event block finished" << std::endl;
    }
    lT = 0;
    mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     302, 0, m_asSLC[lSlice].getSliceIndex(), 0, 0);
    OnErrorReturn((NLS_STATUS)fRTEBFinish());    

//refocusing 
    fRTEBInit(m_asSLC[lSlice].getROT_MATRIX());
    if(bDebug) {
        std::cout << "refocucsing event block" << std::endl;
    }
    lT = 0;
    fRTEI (lT                              ,             0,         0,         0,            0,            0,   &m_sGSliRefoc,         0);
    lT += m_sGSliRefoc.getTotalTime()/2;
    fRTEI (lT-m_sSRF02.getDuration()/2     , &m_sSRF02zSet, &m_sSRF02,         0,            0,            0,               0,         0);
    fRTEI (lT+m_sSRF02.getDuration()/2     , &m_sSRF02zNeg,         0,         0,            0,            0,               0,         0);

    lT += m_sGSliRefoc.getTotalTime()/2;
    fRTEI(lT,0,0,0, 0,0,0, 0);
    mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     303, 0, m_asSLC[lSlice].getSliceIndex(), 0, 0);
    OnErrorReturn((NLS_STATUS)fRTEBFinish()); 

//second diffusion gradient
    fRTEBInit(sDiffRotMatrix, true);
    if(bDebug) {
        std::cout << "second diffusion event block" << std::endl;
    }
    lT = 0;
    if(bRunCrushers  || (m_bIsFreeDiffusion && m_bCrusherInFwf)) {
    fRTEI (lT /*- m_lSliRampTimeOrig*/    ,             0,         0,         0,  &m_sGCrushP,  &m_sGCrushR,     &m_sGCrushS,         0);
    }
    lT += /*m_lCrushTime +*/ m_alTEFil[2];
    if(!m_bIsFreeDiffusion && !bRunCrushers) {
        fRTEI (lT     ,             0,         0,         0,   &m_sGDiffP,   &m_sGDiffR,     &m_sGDiffS,          0);   
    } else if (m_bIsFreeDiffusion){
        fRTEI (lT     ,             0,         0,         0,   &m_sGFwfP2,   &m_sGFwfR2,     &m_sGFwfS2,          0);
        lT += m_sGFwfP2.getTotalTime();
        fRTEI (lT     ,             0,         0,         0, &m_sGRepMomP, &m_sGRepMomR,   &m_sGRepMomS,          0);
        lT -= m_sGDiffP.getTotalTime();
    }
    lT += m_sGDiffP.getTotalTime();
    lT += m_alTEFil[3] + m_alTEFil[4];
    

    //readout
    //logically this would be better in the readout event block, but that would require more reworking of the code
    if( (lSlice == m_WIPParamTool.getLongValue(rMrProt,WIP_lSliceToTrigger_Pos,0)-1 
          || m_WIPParamTool.getLongValue(rMrProt,WIP_lSliceToTrigger_Pos,0) == 0) || isSyncRun){

    fRTEI ( lT - lFieldCameraDelay,             0,         0, 0, 0, 0,               0, &m_ExtTrig);
    }
    fRTEI(lT,0,0,0, 0,0,0, 0);
    lTotalTime += lT;
    mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     304, lLine, m_asSLC[lSlice].getSliceIndex(), 0, 0);
    OnErrorReturn((NLS_STATUS)fRTEBFinish());

    fRTEBInit(sNewRotationMatrix, true);
    if(bDebug) {
        std::cout << "readout event block" << std::endl;
    }

	lT = 0;
		//bool first_scan_sl = m_sADC01.getMDH().isFirstScanInSlice ; //__EK__ 
		//bool last_scan_sl  = m_sADC01.getMDH().isLastScanInSlice ;  //__EK__ 
		//bool last_scan_concat = m_sADC01.getMDH().isLastScanInConcat() ;  //__EK__ 
		//bool last_scan_meas = m_sADC01.getMDH().isLastScanInMeas() ;  //__EK__ 
	long lEcho ;
	long lSegment ;

	for (lEcho=0; lEcho < wiplNoReadouts; lEcho++)//__EK__
	{ //__EK__
		//if(bDebug) {
		//	std::cout << "Readout number:" << lEcho << std::endl;
		//	std::cout << "Slice number:" << lSlice << std::endl;
		//	std::cout << "Diff dir number:" << lDir << std::endl;
		//	std::cout << "Last scan in concat 1:" << last_scan_concat << std::endl;
		//	std::cout << "Last scan in meas 1:" << last_scan_meas << std::endl;
		//}
		//	m_sADC01.getMDH().setCida((unsigned short) lEcho); //__EK__ 
			//m_sADC01.getMDH().setFirstScanInSlice(first_scan_sl && lEcho == 0) ;
			//m_sADC01.getMDH().setLastScanInSlice(last_scan_sl && (lEcho == (wiplNoReadouts-1))) ;
		//	m_sADC01.getMDH().setLastScanInConcat(last_scan_concat && (lEcho == (wiplNoReadouts-1))) ;
		//	m_sADC01.getMDH().setLastScanInMeas(last_scan_meas && (lEcho == (wiplNoReadouts-1))) ;
    
		//if(bDebug) {
		//	std::cout << "Last scan in concat 2: " << m_sADC01.getMDH().isLastScanInConcat() << ", " << m_mySeqLoop.getbIsLastScanInConcat()<< std::endl;
		//	std::cout << "Last scan in meas 2: " << m_sADC01.getMDH().isLastScanInMeas() <<  ", " << m_mySeqLoop.getbIsLastScanInMeas() << std::endl;
		//}
		switch(lEcho) {
		case 0:
			fRTEI ( lT                             , &m_sADC01zSet,         0, &m_sADC01, &m_sGSpiralP, &m_sGSpiralR,               0, 0); //LM
			fRTEI (lT+m_sADC01.getRoundedDuration(), &m_sADC01zNeg,         0,         0,            0,            0,               0,          0); //LM
            if (m_lNoSegments>1) {
                for (lSegment=1; lSegment < m_lNoSegments; lSegment++){
                    fRTEI (lT+lSegment*m_sADC01.getRoundedDuration(), &m_sADC01zSet,         0, &m_sADC01,            0,            0,               0,          0); //LM
	                fRTEI (lT+(lSegment+1)*m_sADC01.getRoundedDuration(), &m_sADC01zNeg,         0,         0,            0,            0,               0,          0); //LM
                }
            }
			break;
		case 1:
			fRTEI ( lT                             , &m_sADC02zSet,         0, &m_sADC02, &m_sGSpiralP, &m_sGSpiralR,               0, 0); //LM
			fRTEI (lT+m_sADC02.getRoundedDuration(), &m_sADC02zNeg,         0,         0,            0,            0,               0,          0); //LM
			break;
		case 2:
			fRTEI ( lT                             , &m_sADC03zSet,         0, &m_sADC03, &m_sGSpiralP, &m_sGSpiralR,               0, 0); //LM
			fRTEI (lT+m_sADC03.getRoundedDuration(), &m_sADC03zNeg,         0,         0,            0,            0,               0,          0); //LM
			break;
		case 3:
			fRTEI ( lT                             , &m_sADC04zSet,         0, &m_sADC04, &m_sGSpiralP, &m_sGSpiralR,               0, 0); //LM
			fRTEI (lT+m_sADC04.getRoundedDuration(), &m_sADC04zNeg,         0,         0,            0,            0,               0,          0); //LM
			break;
		case 4:
			fRTEI ( lT                             , &m_sADC05zSet,         0, &m_sADC05, &m_sGSpiralP, &m_sGSpiralR,               0, 0); //LM
			fRTEI (lT+m_sADC05.getRoundedDuration(), &m_sADC05zNeg,         0,         0,            0,            0,               0,          0); //LM
			break;
		
		}
		lT += m_lReadoutDuration;//+lSpoilDelay;
		if (wiplNoReadouts > 1 && lEcho < (wiplNoReadouts - 1))//__EK__
		{
			fRTEI ( lT                             ,		 0,         0,		   0, &m_sGSpiralRefocP, &m_sGSpiralRefocR,               0, 0); //__EK__
			lT += lRefocDelay+m_alTEFil[lEcho+5]; 
		}
	}//__EK__

	if(isSyncRun) {
		lT =+ m_lSyncTr;
		if(m_sADC01.getMDH().getClin() == m_WIPParamTool.getLongValue(rMrProt, WIP_lSliceToTrigger_Pos, 1)-1 ) {
			lT += m_lAfterSyncTr;
		}
	}

	/*
    fRTEI ( lT                             , &m_sADC01zSet,         0, &m_sADC01, &m_sGSpiralP, &m_sGSpiralR,               0, 0); //LM
    fRTEI (lT+m_sADC01.getRoundedDuration(), &m_sADC01zNeg,         0,         0,            0,            0,               0,          0); //LM
	lT += m_lReadoutDuration;//+lSpoilDelay;
	*/
    if(m_IRselSBB.getDurationPerRequest() + m_IRnsSBB.getDurationPerRequest() > 0 ){
        lT += lTRFill;
    } //shifting by lTRFill to not into problems with timings i.e. no enough time for the system to
	  //calculate the last event block when no diffusion is played before or events are short e.g. only 5ms
	  //and the last event with readout requires more time to be calcualted > no system crush
	  //if there is IR pulse this is not shifted 
	  //there should be to equal lines in the code at the beggining of the *.cpp
    fRTEI (lT,                                           0,         0,         0,            0,            0,               0,          0); //LM

	//mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     20, lLine, m_asSLC[lSlice].getSliceIndex(), 0, 0);
    //OnErrorReturn((NLS_STATUS)fRTEBFinish());

    
    /*lT = fSDSRoundDownGRT(rMrProt.tr()[0] / m_lSlicesToMeasure); 
    if (lSlice == m_lSlicesToMeasure-1) {
        lT = rMrProt.tr()[0] - fSDSRoundDownGRT(rMrProt.tr()[0] / m_lSlicesToMeasure)*lSlice;
    }
    lT -= lTotalTime*/
    //fRTEBInit( m_asSLC[lSlice].getROT_MATRIX());
    //fRTEI (m_mySeqLoop.getlTRFill()                         ,             0,         0,         0,            0,            0,               0,          0);
    //mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     10, lLine, m_asSLC[lSlice].getSliceIndex(), 0, 0);
    //OnErrorReturn((NLS_STATUS)fRTEBFinish());

    mSEQTest(rMrProt, rSeqLim, rSeqExpo, ulTestIdent,     305, 0, m_asSLC[lSlice].getSliceIndex(), 0, 0);

    OnErrorReturn((NLS_STATUS)fRTEBFinish());

    fRTSetGPEnable(true);
    fRTSetGREnable(true);
    fRTSetGSEnable(true);
    fRTSetRFShapeEnable(true);

    return(lStatus);
}


NLS_STATUS lm_Spiral::createUI (SeqLim&)
{
    #ifdef WIN32

        static const char *ptModule = {"lm_Spiral::createUI"};

        //  ----------------------------------------------------------------------
        //  Delete existing instance if necessary
        //  ----------------------------------------------------------------------
        if(m_pUI)  {
            delete m_pUI;
            m_pUI = NULL;
        }

        //  ----------------------------------------------------------------------
        //  Instantiation of the UI class
        //  ----------------------------------------------------------------------
        try  {
            m_pUI = new lm_SpiralUI();
        }

        catch (...)  {
            delete m_pUI;
            m_pUI = NULL;

            TRACE_PUT1(TC_ALWAYS, TF_SEQ,"%s: Cannot instantiate UI class !", ptModule);
            return ( SEQU_ERROR );
        }

    #endif

    return ( SEQU_NORMAL );

}   // end: MiniFlash::createUI



const lm_SpiralUI* lm_Spiral::getUI (void) const
{
    return ( m_pUI );
}
