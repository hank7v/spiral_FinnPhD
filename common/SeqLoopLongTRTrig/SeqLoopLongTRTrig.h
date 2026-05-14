//    -----------------------------------------------------------------------------
//      Copyright (C) Siemens AG 2008  All Rights Reserved.
//    -----------------------------------------------------------------------------
//
//     Project: NUMARIS/4
//        File: \n4\pkg\MrServers\MrImaging\seq\common\SeqLoopLongTRTrig\SeqLoopLongTRTrig.h
//
//        Lang: C++
//
//     Descrip: Modified SeqLoop with modified triggering mode for long TR.
//
//            Slices are grouped so that each group is executed during a
//              different RR-interval, allowing all slices to be acquired during
//              diastole.
//
//            This is realised by repositioning the concatenations loop.
//
//     Classes: SeqLoopLongTRTrig
//
//    -----------------------------------------------------------------------------

#ifndef SeqLoopLongTRTrig_h
#define SeqLoopLongTRTrig_h

//------------------------------------------------------------
// Specify SeqLoop version to be used as base class
//------------------------------------------------------------

// standard SeqLoop
#include "MrServers/MrImaging/libSBB/SEQLoop.h"
typedef SeqLoop  SeqLoop_BASE_TYPE;
#ifdef WIN32
#pragma message ("\n********** Base class for SeqLoopLongTRTrig: SeqLoop\n")
#endif // ifdef WIN32



#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif
namespace SEQ_NAMESPACE
{
//------------------------------------------------------------
// SeqLoopLongTRTrig class
//------------------------------------------------------------
class SeqLoopLongTRTrig : public SeqLoop_BASE_TYPE
{
    public:

        // constructor
        SeqLoopLongTRTrig();

        // destructor
        virtual ~SeqLoopLongTRTrig();

        // switch diffusion triggering mode on/off
        void setLongTRTrigMode( bool bSwitch );

        // get status of diffusion triggering mode
        bool isLongTRTrigMode();

    protected:

        // overloaded SeqLoop function: runConcatenationLoop
        virtual bool runConcatenationLoop (pSEQRunKernel pf, MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, sSLICE_POS* pSlcPos, sREADOUT* psADC);

        // overloaded SeqLoop function: runOuterSliceLoop
        virtual bool runOuterSliceLoop (pSEQRunKernel pf, MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, sSLICE_POS* pSlcPos, sREADOUT* psADC);

        // overloaded SeqLoop function: runPreparingScans
        virtual bool runPreparingScans (pSEQRunKernel pf, MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, sSLICE_POS* pSlcPos, sREADOUT* psADC);

        // flag to specify whether long TR triggering mode is active
        // default value set to true by constructor
        bool m_bLongTRTrigMode;

    private:
};

}//end of namespace SEQ_NAMESPACE

using namespace SEQ_NAMESPACE;

//------------------------------------------------------------
// inline functions
//------------------------------------------------------------
inline void SeqLoopLongTRTrig::setLongTRTrigMode( bool bSwitch )
{
    m_bLongTRTrigMode = bSwitch;
}

inline bool SeqLoopLongTRTrig::isLongTRTrigMode()
{
    return m_bLongTRTrigMode;
}


//------------------------------------------------------------
// end of SeqLoopLongTRTrig_h wrapper
//------------------------------------------------------------
#endif
