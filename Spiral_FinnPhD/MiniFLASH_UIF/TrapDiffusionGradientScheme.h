/*! 
***************************************************************************


***************************************************************************
*/



#ifndef TrapDIffusionGradientScheme_h
/// multiple include protection
#define TrapDIffusionGradientScheme_h 1


#include "AbstractDiffusionGradientScheme.h"	
#include <math.h>                                       // sqrt
#include  "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"
//	x INTERPOLATION												___FF___ 
#include <iostream>
#include <iomanip>
#include <vector>

	class TrapDIffusionGradientScheme: public AbstractDiffusionGradientScheme
	{
  	public:
		TrapDIffusionGradientScheme(void);  
		 ~TrapDIffusionGradientScheme(void);
		IGRAD_PULSE_BASE* getGradientPulse (MrProt& rMrProt/*long lgradDuration*/);
		float interpolate( std::vector<float> &xData, std::vector<float> &yData, float x, bool extrapolate );
	private:
		sGRAD_PULSE_ARB m_sDiffusionGradient;

	};

#endif     
