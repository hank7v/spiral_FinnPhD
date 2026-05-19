#include "TrapDIffusionGradientSchemeFactory.h"	


// ***************************************************************************
// class 
// ***************************************************************************


TrapDIffusionGradientSchemeFactory::TrapDIffusionGradientSchemeFactory (void)
// ===========================================================================
{
  static const char *ptModule = {"AbstractDiffusionGradientSchemeFactory::AbstractDiffusionGradientSchemeFactory"};
}


// ===========================================================================
/// The destructor releases the allocated memory of the vector array.
TrapDIffusionGradientSchemeFactory::~TrapDIffusionGradientSchemeFactory(void)
// ===========================================================================

{
}

AbstractUIConfigurator* TrapDIffusionGradientSchemeFactory::getUIConfigurator()
{
	return new TrapGradUIConfigurator;
}

AbstractDiffusionGradientScheme* TrapDIffusionGradientSchemeFactory::getGradientScheme ()
{
	return new TrapDIffusionGradientScheme;
}