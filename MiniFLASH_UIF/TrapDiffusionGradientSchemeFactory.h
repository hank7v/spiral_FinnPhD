/*! 
***************************************************************************


***************************************************************************
*/



#ifndef TrapDIffusionGradientSchemeFactory_h
/// multiple include protection
#define TrapDIffusionGradientSchemeFactory_h 1
#include "AbstractDiffusionGradientSchemeFactory.h"	
#include "TrapDIffusionGradientScheme.h"	

class TrapDIffusionGradientSchemeFactory: public AbstractDiffusionGradientSchemeFactory
	{
  	public:
		TrapDIffusionGradientSchemeFactory(void);  
		 ~TrapDIffusionGradientSchemeFactory(void);
		AbstractDiffusionGradientScheme* getGradientScheme ();
		AbstractUIConfigurator* getUIConfigurator();
	};

#endif     
