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



#ifndef a_MiniFLASH_UI_h
#define a_MiniFLASH_UI_h 1



//  -------------------------------------------------------------------------- *
//  Application includes                                                       *
//  -------------------------------------------------------------------------- *
#include "MrServers/MrImaging/seq/MiniFLASH_UIF/a_MiniFLASH.h"
#include "MrServers/MrImaging/libUICtrl/UICtrl.h"


#ifdef WIN32
    #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkLimited.h"
    #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkSelection.h"
#endif


//  -------------------------------------------------------------------------- *
//  Forward declarations                                                       *
//  -------------------------------------------------------------------------- *
class MrProt;
class SeqLim;
class SeqExpo;
class Sequence;









namespace SEQ_NAMESPACE
{
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

    class MiniFlashUI {

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
        MiniFlashUI();


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
        virtual ~MiniFlashUI();



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
        NLS_STATUS registerUI (SeqLim &rSeqLim);

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

    };

};

#endif

