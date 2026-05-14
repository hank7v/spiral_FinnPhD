#include "MrServers/MrImaging/seq/common/myHelperClasses/vxMacros.h"
#include "MrServers/MrImaging/seq/common/myHelperClasses/myDiffusionNumbers.h"
#include "MrServers/MrImaging/seq/common/myHelperClasses/helpfulFunctions.h"
#include "MrServers/MrMeasSrv/MeasPatient/MeasPatient.h"
#if VX_VERSION >= VX_VERSION_VD
#include "MrCommon/MrMath/TransformCoordinates.h"  //LROTMAT
#include "MrServers/MrMeasSrv/SeqIF/libRT/include/sROT_MATRIX.h"
#else
#include "MrServers/MrMeasSrv/SeqIF/libRT/sROT_MATRIX.h"
#endif
#include "MrServers/MrMeasSrv/SeqFW/libGSL/libGSL.h"               // fGSL... prototypes

#include  "MrServers/MrMeasSrv/SeqIF/MDH/MdhProxy.h"
#include <cmath>
#include <algorithm>
#include <iostream>

myDiffusionNumbers::myDiffusionNumbers() 
: m_ReadExtDir() 
, m_dBpp(0)
, m_dBpr(0)
, m_dBps(0)
, m_dBrr(0)
, m_dBrs(0)
, m_dBss(0)
, m_vlRefocTimes(1,0)
, m_bUseExternalMatrix(false)
{
    m_vgGrads.resize(2); //since at the moment we use Stejskal Tanner
}

myDiffusionNumbers::myDiffusionNumbers(std::string sName)
: m_ReadExtDir(sName) 
, m_dBpp(0)
, m_dBpr(0)
, m_dBps(0)
, m_dBrr(0)
, m_dBrs(0)
, m_dBss(0)
, m_vlRefocTimes(1,0)
, m_bUseExternalMatrix(false)
{
    m_vgGrads.resize(2); //since at the moment we use Stejskal Tanner
}

myDiffusionNumbers::myDiffusionNumbers(long lDuration, long lRampTime, long lCapitalDelta, double dAmplitude, double dGamma)
: m_ReadExtDir() 
, m_dBpp(0)
, m_dBpr(0)
, m_dBps(0)
, m_dBrr(0)
, m_dBrs(0)
, m_dBss(0)
, m_vlRefocTimes(1,0)
, m_bUseExternalMatrix(false)
{
    setGradients(lDuration, lRampTime, lCapitalDelta, dAmplitude, dGamma);
}

myDiffusionNumbers::myDiffusionNumbers(std::string sName, long lDuration, long lRampTime, long lCapitalDelta, double dAmplitude, double dGamma)
: m_ReadExtDir(sName) 
, m_dBpp(0)
, m_dBpr(0)
, m_dBps(0)
, m_dBrr(0)
, m_dBrs(0)
, m_dBss(0)
, m_vlRefocTimes(1,0)
, m_bUseExternalMatrix(false)
{
    setGradients(lDuration, lRampTime, lCapitalDelta, dAmplitude, dGamma);
}

void myDiffusionNumbers::setFilename(std::string sNewFile) {
    m_ReadExtDir.setFilename(sNewFile); 
    m_ReadExtDir.setLengthToFileLength();
}

bool myDiffusionNumbers::initDirections() {
    m_ReadExtDir.setLengthToFileLength();
    return (m_ReadExtDir.readFile()>=0);
}

long myDiffusionNumbers::initDirections(long anything) {
    m_ReadExtDir.setLengthToFileLength();
    return m_ReadExtDir.readFile();
}

double myDiffusionNumbers::calcBMatrix(long lDir, bool bDumpInfo) {
    double dP = 0, dR = 0, dS = 0;
    //check for valid direction number and get this direction
    if(!getDirectionFactors(lDir, dP, dR, dS)) {
        return -1;
    }
    std::sort(m_vgGrads.begin(),m_vgGrads.end());
    /*if(bDumpInfo) {
        std::cout << "dir: (" << dP << ", " << dR << ", " << dS << ")" << std::endl; 
        std::cout << "1.Gradient: " << std::endl;
        m_vgGrads[0].dumpInfo(0.);
        std::cout << "2.Gradient: " << std::endl;
        m_vgGrads[1].dumpInfo(-m_vgGrads[0].getM0Addition()*m_vgGrads[0].getGamma()*1.e3);
    }*/
    m_dBpp = 0.; m_dBpr = 0.; m_dBps = 0.;
    m_dBrr = 0.; m_dBrs = 0.; m_dBss = 0.;
    double dKp = 0., dKr = 0., dKs = 0.;
    double lCurrTime = 0.;
    double lT = 0.;
    double dGam = m_vgGrads[0].getGamma()*1.e3; //in 1/(mT*s)
    unsigned int iRefPulse = 0; //number of the ref pulse we are in
    for(unsigned i = 0; i<m_vgGrads.size(); ++i) {
        //add potential times without gradient
        lT = double(m_vgGrads[i].getStartTime())*1e-6;
        if(lT < lCurrTime) return -1.; //some kind of gradient overlap, I can't handle right now
        m_dBpp += (lT-lCurrTime)*dKp*dKp; 
        m_dBpr += (lT-lCurrTime)*dKp*dKr; 
        m_dBps += (lT-lCurrTime)*dKp*dKs; 
        m_dBrr += (lT-lCurrTime)*dKr*dKr; 
        m_dBrs += (lT-lCurrTime)*dKr*dKs; 
        m_dBss += (lT-lCurrTime)*dKs*dKs; 
        /*if(bDumpInfo) {
        std::cout << "b( "<<lCurrTime*1e6<<", "<<lT*1e6<<"): " << m_dBpp + m_dBrr + m_dBss << 
            ", k_p: " << dKp << ", k_r: " << dKr << ", k_s: " << dKs << std::endl;
        }*/
        lCurrTime = lT+double(m_vgGrads[i].getTotalTime())*1.e-6;
        //take care of 180° pulses
        if(!m_vlRefocTimes.empty()) {
            if( (iRefPulse < m_vlRefocTimes.size()) && (m_vlRefocTimes[iRefPulse]<lT*1e6) ) {
                /*if(bDumpInfo) {
                    std::cout << "refoc pulse at " << m_vlRefocTimes[iRefPulse] << " flipping k" << std::endl;
                }*/
                dKp *= -1.; dKr *= -1.; dKs *= -1.; 
                ++iRefPulse;
            }
        }                   
        m_dBpp += (m_vgGrads[i].getBMatrixAddition(dP,dP,dKp,dKp));
        m_dBpr += (m_vgGrads[i].getBMatrixAddition(dP,dR,dKp,dKr));
        m_dBps += (m_vgGrads[i].getBMatrixAddition(dP,dS,dKp,dKs));
        m_dBrr += (m_vgGrads[i].getBMatrixAddition(dR,dR,dKr,dKr));
        m_dBrs += (m_vgGrads[i].getBMatrixAddition(dR,dS,dKr,dKs));
        m_dBss += (m_vgGrads[i].getBMatrixAddition(dS,dS,dKs,dKs));
        dKp += m_vgGrads[i].getM0Addition()*dGam*dP;
        dKr += m_vgGrads[i].getM0Addition()*dGam*dR;
        dKs += m_vgGrads[i].getM0Addition()*dGam*dS;
        /*if(bDumpInfo) {
        std::cout << "b( "<<i<<"): " << m_dBpp + m_dBrr + m_dBss << 
            ", k_p: " << dKp << ", k_r: " << dKr << ", k_s: " << dKs << std::endl;
        }*/
    }  
    
    return m_dBpp + m_dBrr + m_dBss;
}

//the way to call Slice object
//    rMrProt.sliceSeries()[static_cast<int32_t>( pSLC->getSliceIndex() )]
void myDiffusionNumbers::writeBMatrix(long lDir, sREADOUT& sADC, Slice const& slice, sSLICE_POS const& sSLC, bool bDumpInfo) {
    
    double adXYZtoSagCorTra[3][3]; 
    double adPRStoXYZ[3][3];
    int iPatDirection=0;
    int iPatPosition = 0;
    MeasPatient theMeasPatient;
    theMeasPatient.getDirection (&iPatDirection); 
    theMeasPatient.getPosition (&iPatPosition);
    LROTMAT sPLAMatrix;
    //this should give x-y-z to Sag-Cor-Tra as I am assuming from comments from SBBDiffusion
    fGSLCalcPLAMatrix (iPatDirection, iPatPosition, &(sPLAMatrix));
    //sPLAMatrix.lMat[r][0])
    for(unsigned int ui=0; ui<3; ++ui) {
        for(unsigned int uj=0; uj<3; ++uj) {
            adXYZtoSagCorTra[ui][uj] = sPLAMatrix.lMat[ui][uj];
            if(m_ReadExtDir.getCoordinateSystem() == PRS) {
                adPRStoXYZ[ui][uj] = VX_ROTMAT(sSLC).dMat[ui][uj];
            } else {
                adPRStoXYZ[ui][uj] = static_cast<double>(ui == uj); //this should give the unit matrix
            }
        }
    }

    if(bDumpInfo) {
        printMatrix(adXYZtoSagCorTra, "XYZtoSagCorTra");  
        printMatrix(adPRStoXYZ, "PRStoXYZ");       
    }

     //calculate the b-matrix in Ph-Re-Sl:
    if(!m_bUseExternalMatrix) {
        calcBMatrix(lDir, bDumpInfo);
    }
    // and rotate to Sag-Cor-Tra
    double covB[3][3];
    double matTmp[3][3];
    double covBRot[3][3];
    covB[0][0] = m_dBpp;
    covB[0][1] = m_dBpr;
    covB[0][2] = m_dBps;

    covB[1][0] = m_dBpr;
    covB[1][1] = m_dBrr;
    covB[1][2] = m_dBrs;

    covB[2][0] = m_dBps;
    covB[2][1] = m_dBrs;
    covB[2][2] = m_dBss;

    if(bDumpInfo) {
        printMatrix(covB, "B in PRS");
    }

    //first rotate PRS to XYZ
    MatMultTrans (covB,adPRStoXYZ,matTmp);       // matTmp = covB * adPRStoXYZ'
    MatMult      (adPRStoXYZ,matTmp,covBRot);    //covBRot = adPRStoXYZ * matTmp
    //then rotate XYZ to Sag-Cor-Tra
    MatMultTrans (covBRot,adXYZtoSagCorTra,matTmp);       // matTmp = covBRot * adXYZtoSagCorTra'
    MatMult      (adXYZtoSagCorTra,matTmp,covB);    //covB = adXYZtoSagCorTra * matTmp

    if(bDumpInfo) {
        printMatrix(covBRot,"B in XYZ"); 
        printMatrix(covB, "B in SagCorTra"); 
    }
    //this is actually in Sag-Cor-Tra, but this way I could just copy the 
    //necessary code from SBBDiffusion
    double dBxx = covB[0][0];
    double dBxy = covB[0][1];
    double dBxz = covB[0][2];
    double dByy = covB[1][1];
    double dByz = covB[1][2];
    double dBzz = covB[2][2];

    //get the diffusion direction in Ph-Re-Sl
    double dDidiP = 0.;
    double dDidiR = 0.;
    double dDidiS = 0.;
    getDirectionFactors(lDir, dDidiP, dDidiR, dDidiS);
    double dDidiSag = 0.;
    double dDidiCor = 0.;
    double dDidiTra = 0.;
    //Diffusion directions are saved in the patient coordinate system (PCS),
    // that is sagittal, coronal, transverse,
    //but I am normally handling them in gradient coordinate system (GCS),
    //that is phase, read slice.
    //this function transforms between the two
    transformGCSToPCS     // Library: libGSL
        (
        dDidiP,                 // Import: Phase encoding component
        dDidiR,                 // Import: Readout component
        dDidiS,                 // Import: Slice selection component
        dDidiSag,               // Export: Sagittal component
        dDidiCor,               // Export: Coronal component
        dDidiTra,               // Export: Transverse component
        slice.normal().sag(),   // Import: Sagittal component of slice normal vector
        slice.normal().cor(),   // Import: Coronal component of slice normal vector
        slice.normal().tra(),   // Import: Transverse component of slice normal vector
        slice.rotationAngle()   // Import: Slice rotation angle ("swap Fre/Pha")
        );

    if(bDumpInfo) {
        std::cout << "slice(SCT-rot ): " << slice.normal().sag() << ", " << slice.normal().cor() << ", "
            <<  slice.normal().tra() << " - " << slice.rotationAngle() << std::endl;
        std::cout << "dir (Ph-Re-Sl ): " << dDidiP << ", " << dDidiR << ", " << dDidiS << std::endl;
        std::cout << "dir (SagCorTra): " << dDidiSag << ", " << dDidiCor << ", " << dDidiTra << std::endl;
    }

    // the b-matrix is postive on the main axis, therefore the sign is used
    // for encoding the diffusion direction (further details in SBBDiffusion.cpp)
    // 3rd step: encode direction
       if (dDidiSag < 0.) {
            dBxx *= -1.;
        }
        if (dDidiCor < 0.) {
            dByy *= -1.;
        }
        if (dDidiTra < 0.) {
            dBzz *= -1.;
        }
    /*if(bDumpInfo) {
        std::cout << "set b matrix in ICE params" << std::endl;
    }*/
        //write bMatrix in MDH
        // Store b-matrix in MDH
    VX_MDH(sADC).setIceProgramPara (0, (unsigned short)(dBxx+16384.5) );
    VX_MDH(sADC).setIceProgramPara (1, (unsigned short)(dByy+16384.5) );
    VX_MDH(sADC).setIceProgramPara (2, (unsigned short)(dBzz+16384.5) );
    VX_MDH(sADC).setIceProgramPara (3, (unsigned short)(dBxy+16384.5) );
    VX_MDH(sADC).setIceProgramPara (4, (unsigned short)(dBxz+16384.5) );
    VX_MDH(sADC).setIceProgramPara (5, (unsigned short)(dByz+16384.5) );
    /*if(bDumpInfo) {
        std::cout << "set b value in ICE params" << std::endl;
    }*/
    VX_MDH(sADC).setIceProgramPara (6, static_cast<unsigned short>( m_dBpp+m_dBrr+m_dBss + 16384.5 ) );   // nominal b value for image text
    VX_MDH(sADC).setIceProgramPara (7, static_cast<unsigned short>( lDir                                          ) );   // direction index for image text

    //multiply pSLC.getRotMatrix with Diffusion direction in Ph-Re-Sl to get x-y-z
    double dDidiX = dDidiP * VX_ROTMAT(sSLC).dMat[0][0] + dDidiR * VX_ROTMAT(sSLC).dMat[0][1] + dDidiS * VX_ROTMAT(sSLC).dMat[0][2];
    double dDidiY = dDidiP * VX_ROTMAT(sSLC).dMat[1][0] + dDidiR * VX_ROTMAT(sSLC).dMat[1][1] + dDidiS * VX_ROTMAT(sSLC).dMat[1][2];
    double dDidiZ = dDidiP * VX_ROTMAT(sSLC).dMat[2][0] + dDidiR * VX_ROTMAT(sSLC).dMat[2][1] + dDidiS * VX_ROTMAT(sSLC).dMat[2][2];
    /*if(bDumpInfo) {
        std::cout << "set grad vec in ICE params: " << m_dAmpl << std::endl;
    }*/
    VX_MDH(sADC).setIceProgramPara (8,  (unsigned short) ( m_dAmpl * dDidiX * 10.  + 16384.5 ) );   // 10 * x-gradient [mT/m]
    VX_MDH(sADC).setIceProgramPara (9,  (unsigned short) ( m_dAmpl * dDidiY * 10.  + 16384.5 ) );   // 10 * x-gradient [mT/m]
    VX_MDH(sADC).setIceProgramPara (10, (unsigned short) ( m_dAmpl * dDidiZ * 10.  + 16384.5 ) );   // 10 * x-gradient [mT/m]
    /*if(bDumpInfo) {
        std::cout << "finished" << std::endl;
    }*/
}
/*
//brief multiplies the matrices given: matC = matA * matB
//taken from SBBDiffusion.cpp
void myDiffusionNumbers::MatMult(double matA[3][3],double matB[3][3],double matC[3][3])
{
	for (int r=0;r<3;r++)
	{
		for (int c=0;c<3;c++)
		{
			matC[r][c] =
				matA[r][0] * matB[0][c] +
				matA[r][1] * matB[1][c] +
				matA[r][2] * matB[2][c];
		}
	}
}

//multiplies the matrices given: matC = matA * matB'
//taken from SBBDiffusion.cpp
void myDiffusionNumbers::MatMultTrans(double matA[3][3],double matB[3][3],double matC[3][3])
{
	for (int r=0;r<3;r++)
	{
		for (int c=0;c<3;c++)
		{
			matC[r][c] =
				matA[r][0] * matB[c][0] + //transposed matB
				matA[r][1] * matB[c][1] +
				matA[r][2] * matB[c][2];
		}
	}
}*/

bool myDiffusionNumbers::getDirectionFactors(long lDir, double& dPFactor, double& dRFactor, double& dSFactor) {
    bool bRet = true;

    if(lDir > m_ReadExtDir.getNoDiffDir()) {
        lDir = m_ReadExtDir.getNoDiffDir();
        bRet = false;
    }
    float fDir[3] = {0.,0.,0.};
    if (!m_ReadExtDir.getRowAsArray(lDir, fDir, 3)) {
        return false;
    }
    dPFactor = fDir[0];
    dRFactor = fDir[1];
    dSFactor = fDir[2];
    return bRet;
}

bool myDiffusionNumbers::getDirectionFactors(long lDir, double adFactors[3]) {
    //double dP, dR, dS;
    //if(getDirectionFactors(lDir, dP, dR, dS)) {
    //    adFactors[0] = dP;
    //    adFactors[1] = dR;
    //    adFactors[2] = dS;
    //    return true;
    //} 
     return getDirectionFactors(lDir, adFactors[0], adFactors[1], adFactors[2]) ;      
}

void myDiffusionNumbers::setGradients(long lDuration, long lRampTime, long lCapitalDelta, double dAmplitude, double dGamma) {
    if(m_vgGrads.size() != 2) {
        m_vgGrads.clear();
        m_vgGrads.resize(2);
    }
    myGradient gBuf(0, lRampTime, lDuration, dAmplitude, dGamma);
    m_vgGrads[0] = gBuf;
    gBuf.setStartTime(lCapitalDelta);
    m_vgGrads[1] = gBuf;
    if(m_vlRefocTimes.size() < 1) {
        m_vlRefocTimes.clear();
        m_vlRefocTimes.resize(1);
    }
    m_vlRefocTimes[0] = lDuration+lRampTime;
    m_dAmpl = dAmplitude;
//    m_lDur = lDuration;
//    m_lRamp = lRampTime;
//    m_lDiffTime = lCapitalDelta;
}//this is specific for Stejskal Tanner