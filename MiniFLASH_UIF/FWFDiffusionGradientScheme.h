/*! 
***************************************************************************


***************************************************************************
*/



#ifndef FWFDiffusionGradientScheme_h
/// multiple include protection
#define FWFDiffusionGradientScheme_h 1

	
#include <math.h>                                       // sqrt
#include  "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"
//	x INTERPOLATION												___FF___ 
#include <iostream>
#include <iomanip>
#include <vector>

#include "AbstractDiffusionGradientScheme.h"



// Lars Helper Classes 
#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadFWFLund.h"



	class FWFDiffusionGradientScheme: public AbstractDiffusionGradientScheme
	{
  	public:
		FWFDiffusionGradientScheme(void);  
		 ~FWFDiffusionGradientScheme(void);
		IGRAD_PULSE_BASE* getGradientPulse (MrProt& rMrProt/*long lgradDuration*/);
		float interpolate( std::vector<float> &xData, std::vector<float> &yData, float x, bool extrapolate );
	private:
		sGRAD_PULSE_ARB m_sDiffusionGradient;
		// Lars
		ReadFWFLund m_readFreeDiff;
		std::vector<float> m_vfGFwfP1;
		sGRAD_PULSE_ARB m_sGFwfP1;

	};

#endif     
