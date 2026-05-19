

#include <fstream>                                  // stream.open
#include <stdio.h>
#include <ctype.h>                                      // tolower, isspace
#include "OscDIffusionGradientScheme.h"	
#include "MrServers/MrImaging/seq/SeqDebug.h"           // mPrintTrace
#include "MrServers/MrMeasSrv/SeqIF/libRT/libRTDefines.h"


// ***************************************************************************
// class 
// ***************************************************************************


DiffusionOscGradientScheme::DiffusionOscGradientScheme (void)
// ===========================================================================
{
  static const char *ptModule = {"AbstractDiffusionGradientScheme::AbstractDiffusionGradientScheme"};
  m_sDiffusionGradient.setIdent("OscdiffGrad");
  //m_sDiffusionGradient.setIdent("FatcoryGrad");
  
}


// ===========================================================================
/// The destructor releases the allocated memory of the vector array.
DiffusionOscGradientScheme::~DiffusionOscGradientScheme(void)
// ===========================================================================

{
	delete m_sDiffusionGradient;
}



IGRAD_PULSE_BASE* DiffusionOscGradientScheme::getGradientPulse (MrProt& rMrProt/*long lgradDuration*/)
{

	/*								//FROM Cord Meyer post MR-IDEA
	IGRAD_PULSE_BASE* pG = NULL;
	sGRAD_PULSE_Osc  G_Osc ("Oscezoidal");
	sGRAD_PULSE_ARB   G_arb  ("arbitrary");
	pG = &G_Osc;
	*/
	
	//m_sDiffusionGradient.setIdent("dw_gx_osc8");
	#define OscSHAPESAMPLES 32
	long current_point;
	float l_faOscRampShape[OscSHAPESAMPLES]; 
	double t,current_g;
	m_sDiffusionGradient.setAmplitude (2);
	m_sDiffusionGradient.setRampUpTime (OscSHAPESAMPLES* GRAD_RASTER_TIME);
	m_sDiffusionGradient.setDuration (OscSHAPESAMPLES* GRAD_RASTER_TIME);
	m_sDiffusionGradient.setRampDownTime (0 );
	std::ifstream infile("C:/Temp/GradShapeOsc.txt");
	float a;
	int b=0;
	while (infile >> a )
	{
		l_faOscRampShape[b]= static_cast<float>(a);
		b++;
	}	
	m_sDiffusionGradient.setRampShape(l_faOscRampShape, OscSHAPESAMPLES, 0, true);
	int c=1;
	if (!m_sDiffusionGradient.prep()) c=2;//

	return(&m_sDiffusionGradient);

	
}

float DiffusionOscGradientScheme::interpolate( std::vector<float> &xData, std::vector<float> &yData, float x, bool exOscolate )
{
   int size = xData.size();

   if (x>10100)
   {
	int check=x;
	std::cout << "debugging" << std::endl;
   }

   int i = 0;                                                                  // find left end of interval for interpolation
   if ( x >= xData[size - 2] )                                                 // special case: beyond right end
   {
      i = size - 2;
   }
   else
   {
      while ( x > xData[i+1] ) i++;
   }
   float xL = xData[i], yL = yData[i], xR = xData[i+1], yR = yData[i+1];      // points on either side (unless beyond ends)
   if ( !exOscolate )                                                         // if beyond ends of array and not exOscolating
   {
      if ( x < xL ) yR = yL;
      if ( x > xR ) yL = yR;
   }

   float dydx = ( yR - yL ) / ( xR - xL );                                    // gradient

   if (x>10100)
   {
	int check=x;
	std::cout << "debugging" << std::endl;
   }
   if (x==xData[size-1]) return(0);
   return yL + dydx * ( x - xL );                                              // linear interpolation
}


