/** 
***************************************************************************


***************************************************************************



***************************************************************************
*/


//#include <fstream.h>                                  // stream.open
#include <stdio.h>
#include <ctype.h>                                      // tolower, isspace
#include "AbstractDiffusionGradientSchemeFactory.h"	
#include "FWFDIffusionGradientSchemeFactory.h"
#include "OscDIffusionGradientSchemeFactory.h"
#include "TrapDIffusionGradientSchemeFactory.h"	
#include "MrServers/MrImaging/seq/SeqDebug.h"           // mPrintTrace
#include "MrServers/MrMeasSrv/SeqIF/libRT/libRTDefines.h"


// ***************************************************************************
// class 
// ***************************************************************************


AbstractDiffusionGradientSchemeFactory::AbstractDiffusionGradientSchemeFactory (void)
{
	//m_sDiffusionGradient.setIdent("FactoryGrad");
}


AbstractDiffusionGradientSchemeFactory::~AbstractDiffusionGradientSchemeFactory(void)
{
}


AbstractDiffusionGradientSchemeFactory* AbstractDiffusionGradientSchemeFactory::getFactory(long lIndex)
{
	switch (lIndex)
    {
	case 1 :
          return new TrapDIffusionGradientSchemeFactory;
          break;
	case 2 :
          return new FWFDiffusionGradientSchemeFactory;
          break;
    case 3 :
          return new DiffusionOscGradientFactory;;
          break;
    }
return new FWFDiffusionGradientSchemeFactory;
}

