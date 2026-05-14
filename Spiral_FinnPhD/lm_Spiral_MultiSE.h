//    -----------------------------------------------------------------------------
//      Copyright (C) Siemens AG 1998  All Rights Reserved.
//    -----------------------------------------------------------------------------
//
//     Project: NUMARIS/4
//        File: \n4\pkg\MrServers\MrImaging\seq\lm_Spiral_?_??\lm_Spiral_?_??.h
//     Version:
//      Author: Clinical
//        Date: n.a.
//
//        Lang: C++
//
//     Descrip: Declarations for lm_Spiral
//
//     Classes:
//
//    -----------------------------------------------------------------------------


// Make sure that this header is read only once:
#ifndef lm_Spiral_h
#define lm_Spiral_h 1


#define MAXRSATS 8

//------------------------------
// Includes
//------------------------------

#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"
#include "MrServers/MrImaging/libSBB/StdSeqIF.h"
#include "MrServers/MrImaging/libSeqUtil/libSeqUtil.h"

#include "MrServers/MrMeasSrv/SeqFW/libGSL/libGSL.h"               // fGSL... prototypes
#include "MrServers/MrMeasSrv/SeqFW/libSSL/libSSL.h"               // fSSL... prototypes
#include "MrServers/MrImaging/libSBB/libSBBmsg.h"                  // SBB_... error codes

#include "MrServers/MrProtSrv/MrProt/KSpace/MrKSpace.h"            // For KSpace
#include "MrServers/MrProtSrv/MrProt/MeasParameter/MrSysSpec.h"    // For GradSpec
#include "MrServers/MrProtSrv/MrProt/MeasParameter/MrRXSpec.h"     // For MrRXSpec
#include "MrServers/MrProtSrv/MrProt/MrProt.h"                     // MrProt
//#include "MrServers/MrMeasSrv/SeqIF/SeqBuffer/SeqLim.h"            // SeqLim
#include "ProtBasic/Interfaces/SeqLim.h"							//LM new header path
#include "MrServers/MrProtSrv/MrProt/SeqIF/SeqExpo.h"              // SeqExpo
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"              // sREADOUT
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"           // sGRAD_PULSE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sRF_PULSE.h"             // sRF_PULSE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sFREQ_PHASE.h"           // sFREQ_PHASE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSYNC.h"					//sSYNC_EXTTRIGGER
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSLICE_POS.h"             //sSLICE_POS

#include "MrServers/MrMeasSrv/SeqIF/Sequence/sequmsg.h"
#include "MrServers/MrProtSrv/MrProt/SeqDefines.h"			

#include "MrServers/MrImaging/libSBB/SBBCSat.h"
#include "MrServers/MrImaging/libSBB/SBBRSat.h"
//#include "MrServers/MrImaging/libSBB/SBBOptfs.h"
//#include "MrServers/MrImaging/libSBB/SBBOptfsPrep.h"
#include "MrServers/MrImaging/libSBB/SBBIRsel.h"
#include "MrServers/MrImaging/libSBB/SBBIRns.h"
//#include "MrServers/MrImaging/libSBB/SBBNoiseMeas.h" //maybe later (for better recon)


#include "MrServers/MrImaging/libSBB/SBBList.h"


#include "MrServers/MrProtSrv/MrProtocol/libUILink/WIPParameterTool.h"
#include <vector>

//for reading external files
//#include "MrServers/MrImaging/seq/common/Utils/ReadExternalSpiral.h"
#include "./../common/Utils/ReadExternalSpiral.h"
#include "./../common/Utils/myDiffusionNumbers.h"
#include "./../common/Utils/ReadFWFLund.h"
//#include "MrServers/MrImaging/seq/common/Utils/ReadExternalDiffDir.h"	
//#include "MrServers/MrImaging/seq/common/Utils/myDiffusionNumbers.h"
//#include "MrServers/MrImaging/seq/common/Utils/ReadFWFLund.h"

//#include "MrServers/MrImaging/seq/common/SeqLoopLongTRTrig/SeqLoopLongTRTrig.h" //SeqLoop
#include "MrServers/MrImaging/libSBB/SEQLoop.h"

#ifdef WIN32
	#include "TCHAR.h"
#endif



#ifdef BUILD_SEQU
    #define __OWNER
#endif

// The following include is necessary for the DLL generation
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h"





//------------------------------
// Forward declarations
//------------------------------

class MrProt;
class SeqLim;
class SeqExpo;
class Sequence;






namespace SEQ_NAMESPACE
{
    // forward declaration
    class lm_SpiralUI;		
	
    class __IMP_EXP lm_Spiral : public StdSeqIF		//LM
    {
    public:
		/**
		 * @brief Standard constructor.
		 * 
		 * Creates a lm_Spiral object with all class member objects being initialized.
		 */
        lm_Spiral();


		/**
		 * @brief Destructor.
		 * 
		 * The lm_Spiral sequence is destructed. A potentially associated MiniFlashUI object will
		 * be deleted.
		 */
        virtual ~lm_Spiral();


		/**
		 * @brief Initialization of the sequence.
		 *
		 * This method is equivalent to the former fSeqInit(...)-function.
		 *
		 * Basically, the "hard limits" for the protocol parameters are set. The hard limits define the maximum
		 * allowed range for each parameter.
		 *
		 * In addition, the associated lm_SpiralUI object is created on the Windows-32 (host) machine.
		 *
		 * @return If running on a Windows-32-machine, the status will tell whether the creation of MiniFlashUI
		 *         was successful. If not running on a Windows-32-machine, the return value will always
		 *         "SEQU__NORMAL".
		 */
        virtual NLSStatus initialize(SeqLim &rSeqLim);


		/**
		 * @brief Preparation of the sequence.
		 *
		 * This method is equivalent to the former fSeqPrep(...)-function. It checks whether or not the
		 * current protocol is valid (i.e. a sequence run with the given parameters is possible).
		 *
		 * The method is called during the binary search (graphical user interface of EXAM or POET) and prior
		 * to executing a sequence.
		 *
		 * @return Success is indicated by "SEQU__NORMAL". Otherwise, the appropriate error code is returned.
		 */
        virtual NLSStatus prepare(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo);


		/**
		 * @brief Check of the sequence.
		 *
		 * This method is equivalent to the former fSeqCheck(...)-function. It checks whether or not the
		 * sequence could do any harm to the patient. This could be:
		 *
		 * - nerve stimulation in the patient
		 * - exceeding of the specific absorbtion rate (SAR) limit
		 *
		 * @return Success is indicated by "SEQU__NORMAL". Otherwise, the appropriate error code is returned.
		 */
        virtual NLSStatus check(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo, SEQCheckMode *  pSEQCheckMode);


        // * ---------------------------------------------------------------------- *
        // *                                                                        *
        // * Name        :  lm_Spiral::run                                          *
        // *                                                                        *
        // * Description :  Execution of the sequence                               *
        // *                                                                        *
        // * Return      :  NLS status                                              *
        // *                                                                        *
        // * ---------------------------------------------------------------------- *
        virtual NLSStatus run(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo);


        // * ---------------------------------------------------------------------- *
        // *                                                                        *
        // * Name        :  lm_Spiral::runKernel                                      *
        // *                                                                        *
        // * Description :  Executes the basic timing of the real-time sequence.    *
        // *                                                                        *
        // * Return      :  NLS status                                              *
        // *                                                                        *
        // * ---------------------------------------------------------------------- *                                                         //lDir
        virtual NLS_STATUS runKernel(MrProt &rMrProt,SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo, long lKernelMode, long lSlice, long lPartition, long lLine);


        //  --------------------------------------------------------------
        //
        //  Name        :  getUI
        //
        //  Description :
        /// \brief <b>     Returns the pointer to the Templ UI class  </b>
        ///
        ///                This method is only sensible on the host.
        ///                On the measurement system, it will return an nearly empty object.
        ///
        //  Return      :  lm_SpiralUI*
        //
        //  --------------------------------------------------------------
        const lm_SpiralUI* getUI (void) const;
		

protected:
	//switch debug on and off
	bool bDebug; //LM
    
//gradient parameters for non diffusion gradients, only used in prepare
    double m_dMinRiseTime;
    double m_dGradMaxAmpl;

//empty times in TE, fill times 
	long m_alTEFil[K_NO_TIME_ELEMENTS];
//not needed at the moment, but might be used for multishot
    int32_t m_lLinesToMeasure;

    //Paramter used for the diffusion encodings
    //vectors to hold the diffusion directions
    //std::vector<double> m_vdFactorP;
    //std::vector<double> m_vdFactorR;
    //std::vector<double> m_vdFactorS;
    long m_lNoDiffDir;
//no of post-180 readouts
    long m_lNoReadouts; //__EK__ 

//parameters to secure proper timing in fSEQRunKernel
    long m_lSliRampTimeOrig;
    long m_lReadoutBeforeEcho;
    long m_lCrushTime;
    long m_lScanTimeSBBs;
    long m_lReadoutDuration;
    long m_lMomentFixingTime;
//used to check all diffusion directions
    long m_lDiffLoopCounterForCheck;
//add some extra time in the sync scans
	long m_lSyncTr;
	long m_lAfterSyncTr;
//Multiple ADC segments for long readouts
    long m_lNoSegments;

//set gradients to zero
    //bool m_bNoExcGrad;
    //bool m_bNoRefocGrad;
    //bool m_bNoReadGrad;
    bool m_bNoGradients;
    bool m_bNoRFpulse;
    bool m_bIsFreeDiffusion;
    bool m_bCrusherInFwf;

    eRotType m_lRotType; //this is an enum defined in ReadExternalDiffDir.h

//hold free gradient forms for spiral and diffusion
    std::vector<float> m_vfGSpiralP;
    std::vector<float> m_vfGSpiralR;
    std::vector<float> m_vfGFwfP1;
    std::vector<float> m_vfGFwfR1;
    std::vector<float> m_vfGFwfS1;
    std::vector<float> m_vfGFwfP2;
    std::vector<float> m_vfGFwfR2;
    std::vector<float> m_vfGFwfS2;
    // * ---------------------------------------------------------------------- *
    // * Slice position information (rotation matrices and shifts)              *
    // * ---------------------------------------------------------------------- *
    sSLICE_POS m_asSLC[K_NO_SLI_MAX];

    // * ---------------------------------------------------------------------- *
    // * RF Pulses and NCO                                                      *
    // * ---------------------------------------------------------------------- *
    sRF_PULSE_EXT          m_sSRF01     ; //excitation
    sFREQ_PHASE            m_sSRF01zSet ;
    sFREQ_PHASE            m_sSRF01zNeg ;
        
    sRF_PULSE_EXT          m_sSRF02     ; //refocusing
    sFREQ_PHASE            m_sSRF02zSet ;
    sFREQ_PHASE            m_sSRF02zNeg ;

    // * ---------------------------------------------------------------------- *
    // * Read Out                                                               *
    // * ---------------------------------------------------------------------- *
    sREADOUT         m_sADC01;
    sFREQ_PHASE      m_sADC01zSet;
    sFREQ_PHASE      m_sADC01zNeg;

	sREADOUT         m_sADC02;
    sFREQ_PHASE      m_sADC02zSet;
    sFREQ_PHASE      m_sADC02zNeg;

	sREADOUT         m_sADC03;
    sFREQ_PHASE      m_sADC03zSet;
    sFREQ_PHASE      m_sADC03zNeg;

	sREADOUT         m_sADC04;
    sFREQ_PHASE      m_sADC04zSet;
    sFREQ_PHASE      m_sADC04zNeg;

	sREADOUT         m_sADC05;
    sFREQ_PHASE      m_sADC05zSet;
    sFREQ_PHASE      m_sADC05zNeg;

// * Gradients
    //used for actual imaging
    sGRAD_PULSE      m_sGSliSel;
    sGRAD_PULSE      m_sGSliSelReph;
    sGRAD_PULSE      m_sGSliRefoc;
	sGRAD_PULSE_ARB  m_sGSpiralP;
	sGRAD_PULSE_ARB	 m_sGSpiralR;
	sGRAD_PULSE      m_sGSpiralRefocP; //__EK__
	sGRAD_PULSE      m_sGSpiralRefocR; //__EK__


    //used to get rid of unwanted signal
    sGRAD_PULSE      m_sGCrushP;
    sGRAD_PULSE      m_sGCrushR;
    sGRAD_PULSE      m_sGCrushS;
    sGRAD_PULSE      m_sGSpoil;
    
    //used for linear diffusion encoding
    sGRAD_PULSE      m_sGDiffP;
    sGRAD_PULSE      m_sGDiffR;
    sGRAD_PULSE      m_sGDiffS;
    //used for Free WaveForm diffusion encoding
    sGRAD_PULSE_ARB  m_sGFwfP1;
    sGRAD_PULSE_ARB  m_sGFwfR1;
    sGRAD_PULSE_ARB  m_sGFwfS1;
    sGRAD_PULSE_ARB  m_sGFwfP2;
    sGRAD_PULSE_ARB  m_sGFwfR2;
    sGRAD_PULSE_ARB  m_sGFwfS2;
    sGRAD_PULSE      m_sGRepMomP;
    sGRAD_PULSE      m_sGRepMomR;
    sGRAD_PULSE      m_sGRepMomS;

	//external trigger event used for field camera
	sSYNC_EXTTRIGGER m_ExtTrig;

    //Self Building Blocks (SBB)
    SBBList					m_mySBBList;
    SeqBuildBlockCSat		m_CSatSBB;
	//SeqBuildBlockOptfs		OptfsSBB;
	//SeqBuildBlockOptfsPrep	OptfsPrepSBB;
	SeqBuildBlockRSat		m_RSatSBB[MAXRSATS];
	SeqBuildBlockIRsel		m_IRselSBB;
	SeqBuildBlockIRns		m_IRnsSBB;
	//SeqBuildBlockNoiseMeas	NoiseSBB;

    /// \brief <b> UI class for Templ
    ///         This class is basically empty on the measurement system
    lm_SpiralUI* m_pUI;
	WIPParameterTool m_WIPParamTool;

    //objects to read the external files
    ReadExternalSpiral m_SpiralRead;
    //ReadExternalDiffDir m_DiffDirRead;
    myDiffusionNumbers m_myDiff;
    ReadFWFLund m_readFreeDiff;

    //try using SeqLoop
    SeqLoop  m_mySeqLoop; //SeqLoop

    /// \brief <b>     Instantiation of UI classes   </b>--
    virtual NLS_STATUS createUI (SeqLim &rSeqLim);

    /// \brief         Used to avoid compiler warnings
    template< class TYPE > void UnusedArg (TYPE Argument) const { if( false ) { TYPE Dummy; Dummy = Argument; } };

private:
// * Copy constructor not implemented                                   *
    lm_Spiral (const lm_Spiral &right);

// * Assignment operator not implemented                                *
    lm_Spiral & operator=(const lm_Spiral &right);
};
};

#endif