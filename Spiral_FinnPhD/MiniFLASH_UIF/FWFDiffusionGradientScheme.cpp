






#include <fstream>                                  // stream.open
#include <stdio.h>
#include <ctype.h>                                      // tolower, isspace
#include "FWFDiffusionGradientScheme.h"	
#include "MrServers/MrImaging/seq/SeqDebug.h"           // mPrintTrace
#include "MrServers/MrMeasSrv/SeqIF/libRT/libRTDefines.h"




// ***************************************************************************
// class 
// ***************************************************************************


FWFDiffusionGradientScheme::FWFDiffusionGradientScheme (void)
// ===========================================================================
{
  static const char *ptModule = {"AbstractDiffusionGradientScheme::AbstractDiffusionGradientScheme"};
  m_sDiffusionGradient.setIdent("FWFdiffGrad");
  //m_sDiffusionGradient.setIdent("FatcoryGrad");
}


// ===========================================================================
/// The destructor releases the allocated memory of the vector array.
FWFDiffusionGradientScheme::~FWFDiffusionGradientScheme(void)
// ===========================================================================

{
}



IGRAD_PULSE_BASE* FWFDiffusionGradientScheme::getGradientPulse (MrProt& rMrProt/*long lgradDuration*/)
{

	/*								//FROM Cord Meyer post MR-IDEA
	IGRAD_PULSE_BASE* pG = NULL;
	sGRAD_PULSE_TRAP  G_trap ("trapezoidal");
	sGRAD_PULSE_ARB   G_arb  ("arbitrary");
	pG = &G_trap;
	*/
	


	//////////////////////////////////////////////////////////////////
	//				read the shapes									//
	//////////////////////////////////////////////////////////////////
	/*
	std::string sFreeDiffFile(getenv("CustomerSeq"));
	#ifdef WIN32
	sFreeDiffFile += "\\Spiral_FWF\\FreeWaveforms\\";
	#else
	sFreeDiffFile += "/Spiral_FWF/FreeWaveforms/";
	#endif
	sFreeDiffFile += wipsFwfFile;
	*/
	std::string sFreeDiffFile = "C:/Temp/FWF_CUSTOM001.txt";
	m_readFreeDiff.setFilename(sFreeDiffFile);
	m_readFreeDiff.setAsymmetricBasedOnFiles();
	//m_readFreeDiff.setReverseSecondGradient(m_readFreeDiff.isSymmetric()); //if we read only one file, we want the second to be time reversed
	m_readFreeDiff.setLengthToFileLength();//make sure to read the whole file (e.g. if it changed)
	if (m_readFreeDiff.readFile() < 0) return 0;

	
	
	//////////////////////////////////////////////////////////////////
	//				auxiliary quantities (TEMP)						//
	//////////////////////////////////////////////////////////////////
	long dDiffGradAmpl1=2;
	long dDiffGradAmpl2=3;
	//long wiplSmallDelta1=2200;
	//long wiplSmallDelta1=lgradDuration;
	long wiplSmallDelta1=rMrProt.wipMemBlock().getalFree()[UIF_UI::WMB_grad_dur];
	long wiplSmallDelta2=2200;
	long wiplCapitalDelta=20000;
	long lTimeRefoc=5000;
	long lCrushTime=1000;
	long dDiffMinRiseTime1=10;


	//Test
	long ciccio1=UIF_UI::REG_grad_dur;
	long ciccio2=UIF_UI::WMB_grad_dur;
	


	//////////////////////////////////////////////////////////////////
	//				for calculating integrals						//
	//////////////////////////////////////////////////////////////////
	m_readFreeDiff.setGradientAmplitudes(dDiffGradAmpl1, dDiffGradAmpl2); //for calculating the integrals, e.g. b-matrix
    m_readFreeDiff.interpGradients(wiplSmallDelta1,wiplSmallDelta2); //Duration of first and second gradient
    m_readFreeDiff.setRefocDuration(wiplCapitalDelta - wiplSmallDelta1 - lTimeRefoc - 2*lCrushTime); //time from end of first to start of second gradient
       
	//////////////////////////////////////////////////////////////////
	//				PREP phase GRADIENT									//
	//////////////////////////////////////////////////////////////////
	//do dummy prep of the gradients to check them
	m_sGFwfP1.setMaxMagnitude(dDiffGradAmpl1);//(SysProperties::getGradMaxAmplAbsolute());
	m_sGFwfP1.setMinRiseTime(dDiffMinRiseTime1);//(SysProperties::getGradMinRiseTimeAbsolute());
	m_vfGFwfP1 = m_readFreeDiff.getFirstInterpPhaseGradient(); //I think m_vfGFwfP1 needs to exist in prep and run and so i made it a member of my sequence
	m_sGFwfP1.setRampShape(&m_vfGFwfP1[0], wiplSmallDelta1/10);//, 0, true);
	m_sGFwfP1.setAmplitude(dDiffGradAmpl1);
	m_sGFwfP1.setRampUpTime(wiplSmallDelta1);
	m_sGFwfP1.setDuration(wiplSmallDelta1);
	if(!m_sGFwfP1.prep()) return 0;
	if(!m_sGFwfP1.check()) return 0;
	return(&m_sGFwfP1);
     

	//////////////////////////////////////////////////////////////////
	//				my old code										//
	//////////////////////////////////////////////////////////////////
	//m_sDiffusionGradient.setIdent("dw_gx_osc8");
	#define FWFSHAPESAMPLES 32
	long current_point;
	float l_faFWFRampShape[FWFSHAPESAMPLES]; 
	double t,current_g;
	m_sDiffusionGradient.setAmplitude (3);
	m_sDiffusionGradient.setRampUpTime (FWFSHAPESAMPLES* GRAD_RASTER_TIME);
	m_sDiffusionGradient.setDuration (FWFSHAPESAMPLES* GRAD_RASTER_TIME);
	m_sDiffusionGradient.setRampDownTime (0 );
	std::ifstream infile("C:/Temp/GradShapeFWF.txt");
	float a;
	int b=0;
	while (infile >> a )
	{
		l_faFWFRampShape[b]= static_cast<float>(a);
		b++;
	}	
	m_sDiffusionGradient.setRampShape(l_faFWFRampShape, FWFSHAPESAMPLES, 0, true);
	m_sDiffusionGradient.prep();	
	return(&m_sDiffusionGradient);
	
}

float FWFDiffusionGradientScheme::interpolate( std::vector<float> &xData, std::vector<float> &yData, float x, bool extrapolate )
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
   if ( !extrapolate )                                                         // if beyond ends of array and not extrapolating
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



