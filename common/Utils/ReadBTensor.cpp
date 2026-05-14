#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadBTensor.h"
#include <math.h> //fabs
#include <cmath> //std::ceil
#include <sstream>
#include <iostream>
//in 10^6 rad/s/T
#define LARMOR_OF_WATER 267.513

ReadBTensor::ReadBTensor() :
    Read3Trajs()                    ,
    m_lNoInterpSteps(1)             ,
    m_dCrushMoment  (0)             ,
    m_dPhaseAmplInFile(2)           ,    
    m_dReadAmplInFile(2)            ,
    m_dSliceAmplInFile(2)           ,
    m_dPhaseRiseTimeInFile(2)       ,
    m_dReadRiseTimeInFile(2)        ,
    m_dSliceRiseTimeInFile(2)       ,
    m_index(0)                      ,
    m_bUseIdenticalGradients(false) ,
    m_dGamma(LARMOR_OF_WATER)
{}

ReadBTensor::ReadBTensor(std::string sFile):
    Read3Trajs(sFile)               ,
    m_lNoInterpSteps(1)             ,
    m_dCrushMoment  (0)             ,
    m_dPhaseAmplInFile(2)           ,    
    m_dReadAmplInFile(2)            ,
    m_dSliceAmplInFile(2)           ,
    m_dPhaseRiseTimeInFile(2)       ,
    m_dReadRiseTimeInFile(2)        ,
    m_dSliceRiseTimeInFile(2)       ,
    m_index(0)                      ,
    m_bUseIdenticalGradients(false) ,
    m_dGamma(LARMOR_OF_WATER)       ,
    m_sFileBaseName(sFile)
{}

ReadBTensor::ReadBTensor(std::string sFile, double dRiseTime):
    Read3Trajs(sFile, dRiseTime)    ,
    m_lNoInterpSteps(1)             ,
    m_dCrushMoment  (0)             ,
    m_dPhaseAmplInFile(2)           ,    
    m_dReadAmplInFile(2)            ,
    m_dSliceAmplInFile(2)           ,
    m_dPhaseRiseTimeInFile(2)       ,
    m_dReadRiseTimeInFile(2)        ,
    m_dSliceRiseTimeInFile(2)       ,
    m_index(0)                      ,
    m_bUseIdenticalGradients(false) ,
    m_dGamma(LARMOR_OF_WATER)       ,
    m_sFileBaseName(sFile)
{}

ReadBTensor::ReadBTensor(std::string sFile, double dRiseTime, double dGamma):
    Read3Trajs(sFile, dRiseTime)    ,
    m_lNoInterpSteps(1)             ,
    m_dCrushMoment  (0)             ,
    m_dPhaseAmplInFile(2)           ,    
    m_dReadAmplInFile(2)            ,
    m_dSliceAmplInFile(2)           ,
    m_dPhaseRiseTimeInFile(2)       ,
    m_dReadRiseTimeInFile(2)        ,
    m_dSliceRiseTimeInFile(2)       ,
    m_index(0)                      ,
    m_bUseIdenticalGradients(false) ,
    m_dGamma(dGamma)                ,
    m_sFileBaseName(sFile)
{}
    
ReadBTensor:: ~ReadBTensor()
{}

int ReadBTensor::readFile() {
   
    //TODO check if all parameters are set (RiseTime and Amplitude)    
    //fills in InFile variables and pulse length
    setFilename(m_sFileBaseName+"_A.txt");
    if(!bCheckExistence()) { 
        m_bUseIdenticalGradients = true;
        setFilename(m_sFileBaseName+".txt");
        if(!bCheckExistence()) return -10;
    } 
    m_lLength = FileReadBase::getFileLength();
    long lReadingLength = m_lLength;
    bool isMinRiseTimeSet = !interpretHeader(FileReadBase::getFileHeader());
    int iRet = FileReadBase::readFile(lReadingLength);
    if(iRet < 0) return iRet;
    m_index++;
    //interpolateAllGradients(FileReadBase::getColumnAsVector(0), FileReadBase::getColumnAsVector(1), FileReadBase::getColumnAsVector(2));
    vfGradP = FileReadBase::getColumnAsVector(0);
    vfGradR = FileReadBase::getColumnAsVector(1);
    vfGradS = FileReadBase::getColumnAsVector(2);
    if(m_bUseIdenticalGradients){
        vfGradP2 = vfGradP;
        vfGradR2 = vfGradR;
        vfGradS2 = vfGradS;
        m_dPhaseAmplInFile[m_index] = m_dAmplP;
        m_dReadAmplInFile[m_index] = m_dAmplR;
        m_dSliceAmplInFile[m_index] = m_dAmplS;
    } else {
        setFilename(m_sFileBaseName+"_B.txt");
        if(!bCheckExistence()) return -10;
        lReadingLength = FileReadBase::getFileLength();
        isMinRiseTimeSet = (!interpretHeader(FileReadBase::getFileHeader())|| isMinRiseTimeSet);
        iRet = FileReadBase::readFile(lReadingLength);
        if(iRet < 0) return iRet;

        vfGradP2 = FileReadBase::getColumnAsVector(0);
        vfGradR2 = FileReadBase::getColumnAsVector(1);
        vfGradS2 = FileReadBase::getColumnAsVector(2);
    }
    m_index=0;
    completeGradientForm();
    //calculateGradientIntegrals();
    return iRet;
}

bool ReadBTensor::interpretHeader(std::string sHeader){
    bool bIsSlewRateGiven = false;
    int i;
    Read3Trajs::interpretHeader(sHeader);
    m_dPhaseAmplInFile[m_index] = m_dAmplP;
    m_dReadAmplInFile[m_index] = m_dAmplR;
    m_dSliceAmplInFile[m_index] = m_dAmplS;

    std::size_t pos1 = sHeader.find("pulseDuration");
    std::size_t pos2;
    if(pos1 != std::string::npos) {
        pos2 = sHeader.find_first_of("#\r\n", pos1);
        m_lPulseDurationInFile = 1000*extractDouble(sHeader.substr(pos1,pos2-pos1));
        m_lPulseDuration = m_lPulseDurationInFile;
    }
    //read the max slew rate if there is any 
    double dMaxSlewRates[3] = {-1., -1., -1.};
    int iNumberSlewRatesGiven = 0;
    pos1 = sHeader.find("MaxSlewRate");
    if(pos1 != std::string::npos){
        pos2 = sHeader.find_first_of("#\n", pos1);
        std::size_t pos3 = sHeader.find_first_of("0123456789", pos1,pos2-pos1);
        if (pos3 != std::string::npos) {
            std::stringstream iss;
            iss << sHeader.substr(pos3,pos2-pos3);
            if(!(iss >> dMaxSlewRates[0])) { 
                 bIsSlewRateGiven = false;
            } else {
                bIsSlewRateGiven = true;
                ++iNumberSlewRatesGiven;
                if((iss >> dMaxSlewRates[1] >> dMaxSlewRates[2])) {
                    iNumberSlewRatesGiven = 3;
                }
            } 
        }
    }
    if(!bIsSlewRateGiven) return false;
    //if necessary fill the array with the first value
    for(i=iNumberSlewRatesGiven; i<3; ++i) {
        dMaxSlewRates[i] = dMaxSlewRates[0];
    }
    m_dPhaseRiseTimeInFile[m_index] = 1000./dMaxSlewRates[0];
    m_dReadRiseTimeInFile[m_index]  = 1000./dMaxSlewRates[1];
    m_dSliceRiseTimeInFile[m_index] = 1000./dMaxSlewRates[2];

    return true;
}

void ReadBTensor::interpolateAllGradients(std::vector<float> const & gradPhase, std::vector<float> const & gradRead, std::vector<float> const & gradSlice) {

    double dLargestStep = getLargestStep(gradPhase, gradRead, gradSlice);
    long lNoTimeSteps = ceil(dLargestStep*m_dRiseTime/10);
    
    vfGradP = interpolateGradient(gradPhase, lNoTimeSteps);
    vfGradR = interpolateGradient(gradRead, lNoTimeSteps);
    vfGradS = interpolateGradient(gradSlice, lNoTimeSteps);
}


float ReadBTensor::getLargestStep(std::vector<float> const & gradPhase, std::vector<float> const & gradRead, std::vector<float> const & gradSlice) {
    double dMaxStepSizeP = 0.;
    double dMaxStepSizeR = 0.;
    double dMaxStepSizeS = 0.;
    double dMaxStepSizePRS = 0.;
    for(unsigned int i = 1; i< gradPhase.size(); ++i) {
        dMaxStepSizeP = std::max<double>(fabs(m_dAmplP*(gradPhase[i]-gradPhase[i-1])), dMaxStepSizeP);
        dMaxStepSizeR = std::max<double>(fabs(m_dAmplR*(gradRead[i] -gradRead[i-1] )), dMaxStepSizeR);
        dMaxStepSizeS = std::max<double>(fabs(m_dAmplS*(gradSlice[i]-gradSlice[i-1])), dMaxStepSizeS);
        dMaxStepSizePRS = std::max<double>( dMaxStepSizePRS,
           ((gradPhase[i]-gradPhase[i-1])*(gradPhase[i]-gradPhase[i-1])*m_dAmplP*m_dAmplP + 
            (gradRead[i]-gradRead[i-1])  *(gradRead[i]-gradRead[i-1])  *m_dAmplR*m_dAmplR + 
            (gradSlice[i]-gradSlice[i-1])*(gradSlice[i]-gradSlice[i-1])*m_dAmplS*m_dAmplS  )  );
    }
    return std::max(std::max(dMaxStepSizeP,dMaxStepSizeR),std::max(dMaxStepSizeS,dMaxStepSizePRS));
}

std::vector<float> ReadBTensor::interpolateGradient( std::vector<float> const &  gradient, long noInterp){
    std::vector<float> output;
    output.reserve(gradient.size()*noInterp);
    output.push_back(gradient.at(0));
    for(unsigned int i = 1; i < gradient.size(); ++i) {
        for(long j=1; j<noInterp; ++j) {
            output.push_back( j*(gradient.at(i)-gradient[i-1])/noInterp );
        }
    }
    return output;
}


bool ReadBTensor::calculateGradientIntegrals() {
    if(vfGradP.empty() || vfGradR.empty() || vfGradS.empty()){
        //maybe set some values if necessary
        return false;
    }
    if(vfGradP.size()!=vfGradR.size() || vfGradP.size()!=vfGradS.size()){
        return false;
    }
    unsigned int ui = 0;
    double kx=0., ky=0., kz=0.;
    m_bxx=0.,m_byy=0., m_bzz=0., m_bxy=0., m_bxz=0., m_byz=0.;
    
//we assume that there is a 180 pulse after the first gradient, effectively inverting all phase (k-space)
    //effects, hence the -= for k for the first gradient.
    for(ui=0; ui<vfGradP.size(); ++ui){
        //numerical integration is done by using the rectangle approximation
        kx -= vfGradP[ui]*m_dAmplP;
        ky -= vfGradR[ui]*m_dAmplR;
        kz -= vfGradS[ui]*m_dAmplS;
        m_bxx += kx*kx;
        m_byy += ky*ky;
        m_bzz += kz*kz;
        m_bxy += kx*ky;
        m_bxz += kx*kz;
        m_byz += ky*kz;
    }
//some unit considerations: m_GRT is in us
    //                      vfGrad is normalized between [-1,1]
    //                      gradient amplitude is in mT/m
    //                      gam is in 10^6 rad/s/T (gam/(2pi) is idea getLarmorConstant())
    m_dCrushMoment = sqrt(kx*kx+ky*ky+kz*kz)*m_GRT;
    
    //pulse duration without gradients (ignore slice refocusing gradient)
/*    cout << "ReadBTensor::Zwischenstand:\n\tkx = " << kx << "\n\tky = " << ky << "\n\tkz = " << kz << std::endl;
    
    cout << "\tm_dCrushMoment = " << m_dCrushMoment << ", " << "m_dGamma" << m_dGamma <<std::endl;
    cout << "\tamplitudes = " << m_dAmplP << " / " << m_dAmplR << " / " << m_dAmplS << std::endl;
    cout << "\tamplitudes2= " << m_dAmplP2 << " / " << m_dAmplR2 << " / " << m_dAmplS2 << std::endl;
    cout << "\tm_GRT = " << m_GRT << ", m_lPulseDuration = " << m_lPulseDuration << std::endl;
    cout << "\tvfGradP.size() = " << vfGradP.size() << ", vfGradP2.size() = " << vfGradP2.size() << std::endl; */
    
    m_bxx += kx*kx*(m_lPulseDuration/m_GRT);
    m_byy += ky*ky*(m_lPulseDuration/m_GRT);
    m_bzz += kz*kz*(m_lPulseDuration/m_GRT);
    m_bxy += kx*ky*(m_lPulseDuration/m_GRT);
    m_bxz += kx*kz*(m_lPulseDuration/m_GRT);
    m_byz += ky*kz*(m_lPulseDuration/m_GRT);

  /*  cout << "\tb:\t " << m_bxx*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bxy*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bxz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << 
            "\n\t\t"  << m_bxy*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byy*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << 
            "\n\t\t"  << m_bxz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bzz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << std::endl;
*/
 //in case we have identical gradients
    for(ui=0; ui<vfGradP2.size(); ++ui){
        //numerical integration is done by using the rectangle approximation
        kx += vfGradP2[ui]*m_dAmplP2;
        ky += vfGradR2[ui]*m_dAmplR2;
        kz += vfGradS2[ui]*m_dAmplS2;
        m_bxx += kx*kx;
        m_byy += ky*ky;
        m_bzz += kz*kz;
        m_bxy += kx*ky;
        m_bxz += kx*kz;
        m_byz += ky*kz;
    }
    //should be 1/m
    kx *= m_GRT*m_dGamma/1000;
    ky *= m_GRT*m_dGamma/1000; 
    kz *= m_GRT*m_dGamma/1000; 

    m_bxx *= m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18;
    m_byy *= m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18;
    m_bzz *= m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18;
    m_bxy *= m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18;
    m_bxz *= m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18;
    m_byz *= m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18;
/*
    cout << "ReadBTensor::Endstand:\n\tkx = " << kx << "\n\tky = " << ky << "\n\tkz = " << kz << std::endl;
    cout << "\tb:\t " << m_bxx << "   " << m_bxy << "   " << m_bxz << 
            "\n\t\t"  << m_bxy << "   " << m_byy << "   " << m_byz << 
            "\n\t\t"  << m_bxz << "   " << m_byz << "   " << m_bzz << std::endl;
*/
    return true;

}



void ReadBTensor::completeGradientForm() {
    //maybe check for consistency with m_lRampTime
    ReadBTensor::completeGradientForm(m_dRiseTime);
}


void ReadBTensor::completeGradientForm(double dMinRiseTime) {
    Read3Trajs::completeGradientForm(dMinRiseTime);
    if(vfGradR2.back() != 0 || vfGradP2.back() != 0 || vfGradS2.back() != 0) {
        //the vectors only hold a relative amplitude, so don't forget to multiply with the actual
        //replace std::ceil with fSDSRoundUpGRT
        long lNeedRampDownTimeP = ceil(fabs(vfGradP2.back())*m_dAmplP*dMinRiseTime/m_GRT);
        long lNeedRampDownTimeR = ceil(fabs(vfGradR2.back())*m_dAmplR*dMinRiseTime/m_GRT);
        long lNeedRampDownTimeS = ceil(fabs(vfGradS2.back())*m_dAmplS*dMinRiseTime/m_GRT);
        long lMaxLengthNew = vfGradP2.size() + std::max(lNeedRampDownTimeP,std::max(lNeedRampDownTimeR,lNeedRampDownTimeS));

        //now fill the new points
		//it may happen that only one gradient is non-zero:
		//later a check is done if the step size is smaller than the residual gradient, which leads to an reduction, 
		//otherwise the following values are all set to zero
		float fStepSizeP = m_dAmplP; 
		if(lNeedRampDownTimeP > 0) {
            fStepSizeP = vfGradP2.back()/lNeedRampDownTimeP;
		}
		float fStepSizeR = m_dAmplR;
		if(lNeedRampDownTimeR > 0) {
            fStepSizeR = vfGradR2.back()/lNeedRampDownTimeR;
		}
        float fStepSizeS = m_dAmplS;
		if(lNeedRampDownTimeS > 0) {
            fStepSizeS = vfGradS2.back()/lNeedRampDownTimeS;
		}
                         //This is the number of additional steps needed
        for(long lI=vfGradP2.size(); lI< lMaxLengthNew; lI++){
			//if stepsize is smaller than the gradient amplitude substract it, otherwise write zero.
            if(fStepSizeP*fStepSizeP < vfGradP2[lI-1]*vfGradP2[lI-1]) {
                vfGradP2.push_back(vfGradP2[lI-1]-fStepSizeP);
			} else {
                vfGradP2.push_back(0);
			}
            if(fStepSizeR*fStepSizeR < vfGradR2[lI-1]*vfGradR2[lI-1]) {
                vfGradR2.push_back(vfGradR2[lI-1]-fStepSizeR);
			} else {
                vfGradR2.push_back(0);
			} 
            if(fStepSizeS*fStepSizeS < vfGradS2[lI-1]*vfGradS2[lI-1]) {
                vfGradS2.push_back(vfGradS2[lI-1]-fStepSizeS);
			} else {
                vfGradS2.push_back(0);
			}
		} //for lI -> setting the ramp down 
        m_lActualLength = vfGradP2.size();
    }
}

void ReadBTensor::getBTensor(double bTens[3][3]) {
    bTens[0][0] = m_bxx;
    bTens[0][1] = m_bxy;
    bTens[0][2] = m_bxz;
    bTens[1][0] = m_bxy;
    bTens[1][1] = m_byy;
    bTens[1][2] = m_byz;
    bTens[2][0] = m_bxz;
    bTens[2][1] = m_byz;
    bTens[2][2] = m_bzz;
}