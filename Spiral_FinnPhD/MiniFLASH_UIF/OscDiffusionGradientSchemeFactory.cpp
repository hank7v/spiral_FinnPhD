#include "OscDIffusionGradientSchemeFactory.h"	


// ***************************************************************************
// class 
// ***************************************************************************


DiffusionOscGradientFactory::DiffusionOscGradientFactory (void)
// ===========================================================================
{
  static const char *ptModule = {"AbstractDiffusionGradientSchemeFactory::AbstractDiffusionGradientSchemeFactory"};
}


// ===========================================================================
/// The destructor releases the allocated memory of the vector array.
DiffusionOscGradientFactory::~DiffusionOscGradientFactory(void)
// ===========================================================================

{
}

AbstractUIConfigurator* DiffusionOscGradientFactory::getUIConfigurator()
{
	return new OscGradUIConfigurator;
}

AbstractDiffusionGradientScheme* DiffusionOscGradientFactory::getGradientScheme ()
{
	return new DiffusionOscGradientScheme;
}