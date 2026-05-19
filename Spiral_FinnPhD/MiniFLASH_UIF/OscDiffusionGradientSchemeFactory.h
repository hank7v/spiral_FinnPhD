/*! 
***************************************************************************


***************************************************************************
*/



#ifndef OscDIffusionGradientSchemeFactory_h
/// multiple include protection
#define OscDIffusionGradientSchemeFactory_h 1
#include "AbstractDiffusionGradientSchemeFactory.h"
#include "OscDiffusionGradientScheme.h"	

class DiffusionOscGradientFactory: public AbstractDiffusionGradientSchemeFactory
	{
  	public:
		DiffusionOscGradientFactory(void);  
		 ~DiffusionOscGradientFactory(void);
		AbstractDiffusionGradientScheme* getGradientScheme ();
		AbstractUIConfigurator* getUIConfigurator();
	private:
		DiffusionOscGradientScheme m_sDiffusionGradientScheme;
		OscGradUIConfigurator m_sUIConfig;
	};

#endif     
