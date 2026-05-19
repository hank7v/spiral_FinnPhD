/*! 
***************************************************************************


***************************************************************************
*/



#ifndef OscDIffusionGradientScheme_h
/// multiple include protection
#define OscDIffusionGradientScheme_h 1


#include "AbstractDiffusionGradientScheme.h"	
#include <math.h>                                       // sqrt
#include  "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"
//	x INTERPOLATION												___FF___ 
#include <iostream>
#include <iomanip>
#include <vector>

class DiffusionOscGradientScheme: public AbstractDiffusionGradientScheme
	{
  	public:
		DiffusionOscGradientScheme(void);  
		 ~DiffusionOscGradientScheme(void);
		IGRAD_PULSE_BASE* getGradientPulse (MrProt& rMrProt/*long lgradDuration*/);
		float interpolate( std::vector<float> &xData, std::vector<float> &yData, float x, bool exOscolate );
	private:
		sGRAD_PULSE_ARB m_sDiffusionGradient;

	};

#endif     
