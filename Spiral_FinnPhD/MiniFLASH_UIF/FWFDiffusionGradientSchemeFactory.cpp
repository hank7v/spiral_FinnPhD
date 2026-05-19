#include "FWFDIffusionGradientSchemeFactory.h"	


// ***************************************************************************
// class 
// ***************************************************************************


FWFDiffusionGradientSchemeFactory::FWFDiffusionGradientSchemeFactory (void)
// ===========================================================================
{
  static const char *ptModule = {"AbstractDiffusionGradientSchemeFactory::AbstractDiffusionGradientSchemeFactory"};
}


// ===========================================================================
/// The destructor releases the allocated memory of the vector array.
FWFDiffusionGradientSchemeFactory::~FWFDiffusionGradientSchemeFactory(void)
// ===========================================================================

{
}

AbstractUIConfigurator* FWFDiffusionGradientSchemeFactory::getUIConfigurator()
{
	return new FWFGradUIConfigurator;
}

AbstractDiffusionGradientScheme* FWFDiffusionGradientSchemeFactory::getGradientScheme ()
{
	return new FWFDiffusionGradientScheme;
}



