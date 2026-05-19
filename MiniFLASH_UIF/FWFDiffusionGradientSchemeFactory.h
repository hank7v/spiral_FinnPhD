/*! 
***************************************************************************


***************************************************************************
*/



#ifndef FWFDIffusionGradientSchemeFactory_h
/// multiple include protection
#define FWFDIffusionGradientSchemeFactory_h 1
#include "AbstractDiffusionGradientSchemeFactory.h"
#include "FWFDiffusionGradientScheme.h"		


// Lars Helper Classes 
#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadFWFLund.h"

class FWFDiffusionGradientSchemeFactory: public AbstractDiffusionGradientSchemeFactory
	{
  	public:
		FWFDiffusionGradientSchemeFactory(void);  
		 ~FWFDiffusionGradientSchemeFactory(void);
		AbstractDiffusionGradientScheme* getGradientScheme ();
		AbstractUIConfigurator* getUIConfigurator();
	private:
		FWFDiffusionGradientScheme m_sDiffusionGradientScheme;
		FWFGradUIConfigurator m_sUIConfig;
	};

#endif     
