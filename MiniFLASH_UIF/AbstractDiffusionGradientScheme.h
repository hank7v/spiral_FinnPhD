/*! 
***************************************************************************


***************************************************************************
*/



#ifndef AbstractDiffusionGradientScheme_h
/// multiple include protection
#define AbstractDiffusionGradientScheme_h 1

#include <math.h>                                       // sqrt
//	x INTERPOLATION												___FF___ 
#include <iostream>
#include <iomanip>
#include <vector>

#include  "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"
#include "UIF_UILink.h"//
#include "MrServers/MrProtSrv/MrProt/MrProt.h"                     // MrProt

class AbstractDiffusionGradientScheme
	{
  	public:
		AbstractDiffusionGradientScheme(void);  
		 ~AbstractDiffusionGradientScheme(void);
		virtual IGRAD_PULSE_BASE* getGradientPulse (MrProt& rMrProt/*rMrProtlong lgradDuration*/)=0;
	};

#endif     
