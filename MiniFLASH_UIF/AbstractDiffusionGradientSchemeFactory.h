/*! 
***************************************************************************


***************************************************************************
*/



#ifndef AbstractDiffusionGradientSchemeFactory_h
/// multiple include protection
#define AbstractDiffusionGradientSchemeFactory_h 1
#include "UIConfigurator.h"
#include "AbstractDiffusionGradientScheme.h"

#include <vector>


class AbstractDiffusionGradientSchemeFactory
	{
  	public:
		AbstractDiffusionGradientSchemeFactory(void);  
		 ~AbstractDiffusionGradientSchemeFactory(void);
		virtual AbstractDiffusionGradientScheme* getGradientScheme ()=0;
		virtual AbstractUIConfigurator* getUIConfigurator ()=0;
		static AbstractDiffusionGradientSchemeFactory* getFactory(long lIndex);
		
	};

#endif     
