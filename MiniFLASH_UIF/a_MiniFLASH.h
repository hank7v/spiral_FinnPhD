//    -----------------------------------------------------------------------------
//      Copyright (C) Siemens AG 1998  All Rights Reserved.
//    -----------------------------------------------------------------------------
//
//     Project: NUMARIS/4
//        File: \n4\pkg\MrServers\MrImaging\seq\a_MiniFLASH\a_MiniFLASH.h
//     Version:
//      Author: Clinical
//        Date: n.a.
//
//        Lang: C++
//
//     Descrip: Declarations for a_MiniFLASH.cpp
//
//     Classes:
//
//    -----------------------------------------------------------------------------


// Make sure that this header is read only once:
#ifndef a_MiniFlash_h
#define a_MiniFlash_h 1


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
#include "MrServers/MrMeasSrv/SeqIF/SeqBuffer/SeqLim.h"            // SeqLim
#include "MrServers/MrProtSrv/MrProt/SeqIF/SeqExpo.h"              // SeqExpo
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"              // sREADOUT
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"           // sGRAD_PULSE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sRF_PULSE.h"             // sRF_PULSE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sFREQ_PHASE.h"           // sFREQ_PHASE

#include "MrServers/MrMeasSrv/SeqIF/Sequence/sequmsg.h"
#include "MrServers/MrProtSrv/MrProt/SeqDefines.h"

#ifdef WIN32
	#include "TCHAR.h"
#endif

#ifdef BUILD_SEQU
    #define __OWNER
#endif

// The following include is necessary for the DLL generation
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h"


#include "AbstractDiffusionGradientSchemeFactory.h"//								___FF___ factory stuff
#include "ProtBasic/Interfaces/MrWipMemBlock.h"//							___FF___
#include "UIF_UILink.h"//													___FF___
#include "AbstractDiffusionGradientScheme.h"//								___FF___						

//#include "UIConfig.h";

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
    class MiniFlashUI;

	/**
	 * @brief The MiniFlash sequence class is intended as a demo implementation for teaching purposes. Therefore, all
	 * functions are implemented in a "minimalistic" fashion.
	 */
    //class __IMP_EXP MiniFlash : public StdSeqIF
	class MiniFlash : public StdSeqIF	//					___FF___
    {
    public:
		/**
		 * @brief Standard constructor.
		 * 
		 * Creates a MiniFlash object with all class member objects being initialized.
		 */
        MiniFlash();


		/**
		 * @brief Destructor.
		 * 
		 * The MiniFlash sequence is destructed. A potentially associated MiniFlashUI object will
		 * be deleted.
		 */
        virtual ~MiniFlash();


		/**
		 * @brief Initialization of the sequence.
		 *
		 * This method is equivalent to the former fSeqInit(...)-function.
		 *
		 * Basically, the "hard limits" for the protocol parameters are set. The hard limits define the maximum
		 * allowed range for each parameter.
		 *
		 * In addition, the associated MiniFlashUI object is created on the Windows-32 (host) machine.
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
        // * Name        :  MiniFlash::run                                          *
        // *                                                                        *
        // * Description :  Execution of the sequence                               *
        // *                                                                        *
        // * Return      :  NLS status                                              *
        // *                                                                        *
        // * ---------------------------------------------------------------------- *
        virtual NLSStatus run(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo);


        // * ---------------------------------------------------------------------- *
        // *                                                                        *
        // * Name        :  Fl3d_ce::runKernel                                      *
        // *                                                                        *
        // * Description :  Executes the basic timing of the real-time sequence.    *
        // *                                                                        *
        // * Return      :  NLS status                                              *
        // *                                                                        *
        // * ---------------------------------------------------------------------- *
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
        //  Return      :  MiniFlashUI*
        //
        //  --------------------------------------------------------------
        const MiniFlashUI* getUI (void) const;

		// here we initialise all the sets we need to manage dynamically the UI								___FF___
		virtual NLSStatus initializeFactoryStuff(void);//	___FF___
		virtual NLSStatus checkAndRefreshInCaseOfSelBoxesVariation(MrProt &rMrProt,SeqLim &rSeqLim);//				___FF___
#ifdef WIN32
		virtual NLSStatus refreshGLOBAL_UI_ELEMENTSET(void);//												___FF___
		virtual NLSStatus initializeUIelementSets(void);//													___FF___
#endif

protected:
    // * ---------------------------------------------------------------------- *
    // * RF spoiling phase                                                      *
    // * ---------------------------------------------------------------------- *
    double m_dRFSpoilPhase;


    // * ---------------------------------------------------------------------- *
    // * Increment of the Rf spoiling phase                                     *
    // * ---------------------------------------------------------------------- *
    double m_dRFSpoilIncrement;


    // * ---------------------------------------------------------------------- *
    // * Index of the k-space center line                                       *
    // * ---------------------------------------------------------------------- *
    long m_lCenterLine;


    // * ---------------------------------------------------------------------- *
    // * Mini Rise Time                                                         *
    // * ---------------------------------------------------------------------- *
    double m_dMinRiseTime;


    // * ---------------------------------------------------------------------- *
    // * Mini Rise Time                                                         *
    // * ---------------------------------------------------------------------- *
    double m_dGradMaxAmpl;


    // * ---------------------------------------------------------------------- *
    // * No of phase encoding lines measured per second                         *
    // * ---------------------------------------------------------------------- *
    long m_lLinesPerSec;


    // * ---------------------------------------------------------------------- *
    // * No of phase encoding lines with iPAT                                   *
    // * m_lLinesToMeasure and m_lLinesToMeasureMax are identical if iPAT is    *
    // * disabled.                                                              *
    // * ---------------------------------------------------------------------- *
    int32_t m_lLinesToMeasure;


    // * ---------------------------------------------------------------------- *
    // * Slice position information (rotation matrices and shifts)              *
    // * ---------------------------------------------------------------------- *
    sSLICE_POS m_asSLC[1];


    // * ---------------------------------------------------------------------- *
    // * RF Pulses and NCO                                                      *
    // * ---------------------------------------------------------------------- *
    sRF_PULSE_SINC   m_sSRF01;
    sFREQ_PHASE      m_sSRF01zSet;
    sFREQ_PHASE      m_sSRF01zNeg;


    // * ---------------------------------------------------------------------- *
    // * Read Out                                                               *
    // * ---------------------------------------------------------------------- *
    sREADOUT         m_sADC01;
    sFREQ_PHASE      m_sADC01zSet;
    sFREQ_PHASE      m_sADC01zNeg;


    // * ---------------------------------------------------------------------- *
    // * Gradient                                                               *
    // * ---------------------------------------------------------------------- *
    sGRAD_PULSE      m_sGSliSel;
    sGRAD_PULSE      m_sGSliSelReph;
    sGRAD_PULSE_RO   m_sGradRO     ;
    sGRAD_PULSE      m_sGReadDeph  ;
    sGRAD_PULSE_PE   m_sGPhasTab   ;
    sGRAD_PULSE_PE   m_sGPhasTabRew;
    sGRAD_PULSE      m_sGSpoil     ;


    // * ---------------------------------------------------------------------- *
    // * RF Pulses                                                              *
    // * ---------------------------------------------------------------------- *
    //  --------------------------------------------------------------
    /// \brief <b> UI class for Templ
    ///
    ///         This class is basically empty on the measurement system
    //  --------------------------------------------------------------
    MiniFlashUI* m_pUI;

	////////////////////////////////////////////////////////////////
	// abstract factory stuff
	///////////////////////////////////////////////////////////////
	// interface for factory 													___FF___
	AbstractDiffusionGradientSchemeFactory* m_pDiffusionGradientSchemeFactory;
	// interface for Diff Grad pulse
	IGRAD_PULSE_BASE *m_pDiffusionGradient;
	// interface for Diff Grad Scheme UI Configurator
	AbstractUIConfigurator *m_pDiffusionGradSchemeUIConfigurator;
	// interface for DIff Grad Scheme
	AbstractDiffusionGradientScheme* m_pDiffusionGradientScheme;
#ifdef WIN32	
	///////////////////////////////////////////////////////////////////
	// UI configuration stuff											___FF___
	///////////////////////////////////////////////////////////////////
	// Main UIelementSets, Std Card and Extra card, will feed the GLOBAL UI ELEMENT SET used in the function handlers
	UIelementSet m_StdUIelmSet;
	UIelementSet m_ExtraCardUIelmSet;
	// Std Card elements
	UIelement m_StdCheckBoxUIelm; // will split into 3 differen ones, one for Grad Scheme class
	UIelementLong m_StdDiffGradDurUIelmLong;
	UIelementLong m_StdNdirUIelmLong;
	UIelementLong m_StdNgvaUIelmLong;
	// Diff Grad Extra Card, composed by Diff Grad Scheme element set and Diff Grad std element set
    UIelementSet m_DiffGradExtraCardUIelmSet;
    UIelementSet m_DiffGradSchemeUIelmSet;
	UIelementSet m_DiffGradStdUIelmSet;
	// the elements in Grad Scheme Std element set
	UIelement m_DiffGradB0CheckBoxUIelm;
	UIelementLong m_DiffGradDirUIelmLongArray;
	UIelementLong m_GvalueUIelmLongArray;
	// Vapor Extra Card, and its elements
	UIelementSet m_VaporExtraCardUIelmSet;
	UIelementLong m_VaporFaUIelmLong;//VAPOR
	UIelement m_VaporCheckBoxUIelm;
	UIelementLong m_VaporWSdeoptThrUIelmLongArray;
	UIelementLong m_VaporDelaysUIelmLongArray;
	UIelementDouble m_VaporWSdeoptFractUIelmDoubleArray;
	UIelementLong m_VaporInvisibleUIelmLong2;// the one i position 2, VAPOR does not use it.
	// Optim Extra Card and its elements
	UIelementSet m_OptimExtraCardUIelmSet;
	UIelementLong m_OptimGradMaxUIelmLong;//Optim
	UIelementLong m_OptimRiseTimeUIelmLong;//Optim
	UIelement m_OptimInvisibleCheckBoxUIelm;
	UIelementLong m_OptimInvisibleUIelmLongArray1;
	UIelementLong m_OptimInvisibleUIelmLongArray2;
	UIelementDouble m_OptimUIelmDoubleArray;
	UIelementDouble m_DiffGradSchemeUIelmDoubleArray;
	UIelementSelectionBox m_UIelmSelBox;
	UIelementSelectionBox m_InvisibleUIelmSelBox;
#endif
	long m_lCurrentGradSchemeChoice;
	long m_lCurrentExtraCardChoice;



    //  ------------------------------------------------------------------
    //
    //  Name        :  MiniFlash::createUI
    //
    //  Description :
    /// \brief <b>     Instantiation of UI classes   </b>
    //
    //  Return      :  NLS status
    //
    //  ------------------------------------------------------------------
    virtual NLS_STATUS createUI (SeqLim &rSeqLim);


    //  ------------------------------------------------------------------
    //
    //  Name        :  MiniFlash::UnusedArg
    //
    //  Description :
    /// \brief         Used to avoid compiler warnings
    //
    //  Return      :  void
    //
    //  ------------------------------------------------------------------
    template< class TYPE > void UnusedArg (TYPE Argument) const { if( false ) { TYPE Dummy; Dummy = Argument; } };

private:

    // * ------------------------------------------------------------------ *
    // * Copy constructor not implemented                                   *
    // * ------------------------------------------------------------------ *
    MiniFlash (const MiniFlash &right);



    // * ------------------------------------------------------------------ *
    // * Assignment operator not implemented                                *
    // * ------------------------------------------------------------------ *
    MiniFlash & operator=(const MiniFlash &right);
};
};

#endif