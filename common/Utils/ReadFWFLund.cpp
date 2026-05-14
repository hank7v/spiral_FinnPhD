#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadFWFLund.h"
#include <algorithm>
#include <iostream>
#include <string>

#define LARMOR_OF_WATER 267.513
#define GRT 10

ReadFWFLund::ReadFWFLund():
m_lInterpTimeOld1(-1),
m_lInterpTimeOld2(-1),
m_lInterpTimeNew1(-1),
m_lInterpTimeNew2(-1),
m_lRefocDuration(-1),
m_bxx(0), 
m_byy(0), 
m_bzz(0), 
m_bxy(0), 
m_bxz(0), 
m_byz(0),
m_dCrushMoment(0),
m_dGamma(LARMOR_OF_WATER),
m_isAsymmetric(false),
m_bReverseSecondGradient(true),
m_sNameBase("")
{
}
    
ReadFWFLund::ReadFWFLund(std::string sFile):
m_lInterpTimeOld1(-1),
m_lInterpTimeOld2(-1),
m_lInterpTimeNew1(-1),
m_lInterpTimeNew2(-1),
m_lRefocDuration(-1),
m_bxx(0), 
m_byy(0), 
m_bzz(0), 
m_bxy(0), 
m_bxz(0), 
m_byz(0),
m_dCrushMoment(0),
m_dGamma(LARMOR_OF_WATER),
m_isAsymmetric(false),
m_bReverseSecondGradient(true)
{
    setFilename(sFile);
}

ReadFWFLund::~ReadFWFLund()
{
}
    
int ReadFWFLund::readFile(){

    std::string sFile1, sFile2;
    if(m_isAsymmetric) {
        sFile1 = m_sNameBase + "_A.txt";
        sFile2 = m_sNameBase + "_B.txt";
    } else {
        sFile1 = m_sNameBase + ".txt";
        sFile2 = m_sNameBase + ".txt";
    }
    //std::cout << "filename1: " << sFile1 << std::endl;    
    readGradient1.setFilename(sFile1);
    readGradient1.setLengthToFileLength();
    int iRet = readGradient1.readFile(false, true); //don't ramp the waveform to zero, never use the first value as an amplitude
    if(iRet < 0) return iRet;
    
    //std::cout << "filename2: " << sFile2 << std::endl;
    readGradient2.setFilename(sFile2);
    readGradient2.setLengthToFileLength();
    iRet = readGradient2.readFile(false, true);
    if(iRet < 0) return iRet;

/*    //make sure the values in the vector are [-1,1]
    m_dAmplP = abs(*std::max_element(vfGradP.begin(), vfGradP.end()));
    m_dAmplP = std::max<double>(m_dAmplP, abs(*std::max_element(vfGradP.begin(), vfGradP.end())));
    m_dAmplR = abs(*std::max_element(vfGradR.begin(), vfGradR.end()));
    m_dAmplR = std::max<double>(m_dAmplR, abs(*std::max_element(vfGradR.begin(), vfGradR.end())));
    m_dAmplS = abs(*std::max_element(vfGradS.begin(), vfGradS.end()));
    m_dAmplS = std::max<double>(m_dAmplS, abs(*std::max_element(vfGradS.begin(), vfGradS.end())));
    //make sure not to divide by 0. 0 in m_dAmplX means the gradient is only zeros, so no need to change it
    double dAmplP = (m_dAmplP < 0.0001) ? 1 : m_dAmplP;
    double dAmplR = (m_dAmplR < 0.0001) ? 1 : m_dAmplR;
    double dAmplS = (m_dAmplS < 0.0001) ? 1 : m_dAmplS;
    for(unsigned int ui = 0; ui<vfGradP.size(); ui++) {
        vfGradP[ui] = vfGradP[ui]/dAmplP;
        vfGradR[ui] = vfGradR[ui]/dAmplR;
        vfGradS[ui] = vfGradS[ui]/dAmplS;
    }
*/
    m_lInterpTimeOld1 = 0;
    m_lInterpTimeOld2 = 0;
    //no completion of the gradient, as we don't know what the time step is
    return iRet;
}

void ReadFWFLund::setFilename(std::string sFilename) {
    //check if filename ends in .txt, then get rid of it
    if(sFilename.size()>4) {
        if(sFilename.compare(sFilename.size()-4,4,".txt")==0) {
            //for(unsigned int i=0; i<4; ++i) {
            //    sFilename.pop_back(); //pop_back for strings was officially introduced in C++11 
            //}
            sFilename.erase(sFilename.size()-4,4);
        }
    }
    m_sNameBase = sFilename;  
}

std::string ReadFWFLund::getFilename() {
    return m_sNameBase + ".txt";
}
    
bool ReadFWFLund::setAsymmetricBasedOnFiles() {
    //check existence of _A and _B versions of the file
    readGradient1.setFilename(m_sNameBase+"_A.txt");
    if(!readGradient1.checkExistence()) {
        m_isAsymmetric = false;
        return false;
    }
    readGradient2.setFilename(m_sNameBase+"_B.txt");
    if(!readGradient2.checkExistence()) {
        m_isAsymmetric = false;
        return false;
    }
    m_isAsymmetric = true;
    return true;
}

std::vector<float> ReadFWFLund::getInterpPhaseGradient() {
    return getFirstInterpPhaseGradient();
}

std::vector<float> ReadFWFLund::getInterpReadGradient() {
    return getFirstInterpReadGradient();
}
    
std::vector<float> ReadFWFLund::getInterpSliceGradient() {
    return getFirstInterpSliceGradient();
}

std::vector<float> ReadFWFLund::getInterpPhaseGradientReversed() {
    std::vector<float> vfGRev = getFirstInterpPhaseGradient();;
    //For the vector to work in my sequence it always has to end on zero, and 
    //the input with the interpolation should make sure of that (files should
    //start and end with 0). The vector doesn't need to start with 0 to work. 
    //The solution for the inversion is to just append a 0 in the end after 
    //inversion and, to keep the length, delete the last zero before inversion.
    if(vfGRev.size()>1) {
        vfGRev.pop_back();
        std::reverse(vfGRev.begin(),vfGRev.end());
        vfGRev.push_back(0);
    }
    return vfGRev;
}

std::vector<float> ReadFWFLund::getInterpReadGradientReversed() {
    std::vector<float> vfGRev = getFirstInterpReadGradient();;
    //For the vector to work in my sequence it always has to end on zero, and 
    //the input with the interpolation should make sure of that (files should
    //start and end with 0). The vector doesn't need to start with 0 to work. 
    //The solution for the inversion is to just append a 0 in the end after 
    //inversion and, to keep the length, delete the last zero before inversion.
    if(vfGRev.size()>1) {
        vfGRev.pop_back();
        std::reverse(vfGRev.begin(),vfGRev.end());
        vfGRev.push_back(0);
    }
    return vfGRev;
}
    
std::vector<float> ReadFWFLund::getInterpSliceGradientReversed() {
    std::vector<float> vfGRev = getFirstInterpSliceGradient();;
    //For the vector to work in my sequence it always has to end on zero, and 
    //the input with the interpolation should make sure of that (files should
    //start and end with 0). The vector doesn't need to start with 0 to work. 
    //The solution for the inversion is to just append a 0 in the end after 
    //inversion and, to keep the length, delete the last zero before inversion.
    if(vfGRev.size()>1) {
        vfGRev.pop_back();
        std::reverse(vfGRev.begin(),vfGRev.end());
        vfGRev.push_back(0);
    }
    return vfGRev;
}

std::vector<float> ReadFWFLund::getFirstInterpPhaseGradient(){
    if(m_lInterpTimeNew1!=m_lInterpTimeOld1 && m_lInterpTimeNew1>(GRT-1)) {
        if(!m_vfGPInterp1.empty() || m_vfGPInterp1.size() != m_lInterpTimeNew1/GRT) {
            m_vfGPInterp1 = interpGradient(readGradient1.getPhaseGradient(), m_lInterpTimeNew1);
        }
    }
    return m_vfGPInterp1;
}
std::vector<float> ReadFWFLund::getFirstInterpReadGradient() {
    if(m_lInterpTimeNew1!=m_lInterpTimeOld1 && m_lInterpTimeNew1>(GRT-1)) {
        if(!m_vfGRInterp1.empty() || m_vfGRInterp1.size() != m_lInterpTimeNew1/GRT) {
            m_vfGRInterp1 = interpGradient(readGradient1.getReadGradient(), m_lInterpTimeNew1);
        }
    }
    return m_vfGRInterp1;
}
std::vector<float> ReadFWFLund::getFirstInterpSliceGradient() {
    if(m_lInterpTimeNew1!=m_lInterpTimeOld1 && m_lInterpTimeNew1>(GRT-1)) {
        if(!m_vfGSInterp1.empty() || m_vfGSInterp1.size() != m_lInterpTimeNew1/GRT) {
            m_vfGSInterp1 = interpGradient(readGradient1.getSliceGradient(), m_lInterpTimeNew1);
        }
    }
    return m_vfGSInterp1;
}
//possible problem: change the time reversal without changing the time is without effect //TODO
std::vector<float> ReadFWFLund::getSecondInterpPhaseGradient() {
    if( (m_lInterpTimeNew2!=m_lInterpTimeOld2 && m_lInterpTimeNew2>(GRT-1)) || (m_bReverseSecondGradientOld != m_bReverseSecondGradient) ) {
        if(!m_vfGPInterp2.empty() || m_vfGPInterp2.size() != m_lInterpTimeNew2/GRT) {
            m_vfGPInterp2 = interpGradient(readGradient2.getPhaseGradient(), m_lInterpTimeNew2);
            if(m_bReverseSecondGradient) {
                if(m_vfGPInterp2.size()>1) {
                    m_vfGPInterp2.pop_back();
                    std::reverse(m_vfGPInterp2.begin(),m_vfGPInterp2.end());
                    m_vfGPInterp2.push_back(0);
                }
            }
        }
    }
    m_bReverseSecondGradientOld = m_bReverseSecondGradient;
    return m_vfGPInterp2;
}
std::vector<float> ReadFWFLund::getSecondInterpReadGradient() {
    if( (m_lInterpTimeNew2!=m_lInterpTimeOld2 && m_lInterpTimeNew2>(GRT-1)) || (m_bReverseSecondGradientOld != m_bReverseSecondGradient) ) {
        if(!m_vfGRInterp2.empty() || m_vfGRInterp2.size() != m_lInterpTimeNew2/10) {
            m_vfGRInterp2 = interpGradient(readGradient2.getReadGradient(), m_lInterpTimeNew2);
            if(m_bReverseSecondGradient) {
                if(m_vfGRInterp2.size()>1) {
                    m_vfGRInterp2.pop_back();
                    std::reverse(m_vfGRInterp2.begin(),m_vfGRInterp2.end());
                    m_vfGRInterp2.push_back(0);
                }
            }
        }
    }
    m_bReverseSecondGradientOld = m_bReverseSecondGradient;
    return m_vfGSInterp2;
}
std::vector<float> ReadFWFLund::getSecondInterpSliceGradient() {
    if( (m_lInterpTimeNew2!=m_lInterpTimeOld2 && m_lInterpTimeNew2>(GRT-1)) || (m_bReverseSecondGradientOld != m_bReverseSecondGradient) ) {
        if(!m_vfGSInterp2.empty() || m_vfGSInterp2.size() != m_lInterpTimeNew2/10) {
            m_vfGSInterp2 = interpGradient(readGradient2.getSliceGradient(), m_lInterpTimeNew2);
            if(m_bReverseSecondGradient) {
                if(m_vfGSInterp2.size()>1) {
                    m_vfGSInterp2.pop_back();
                    std::reverse(m_vfGSInterp2.begin(),m_vfGSInterp2.end());
                    m_vfGSInterp2.push_back(0);
                }
            }
        }
    }
    m_bReverseSecondGradientOld = m_bReverseSecondGradient;
    return m_vfGSInterp2;
}


bool ReadFWFLund::interpGradients() {
    if (m_lInterpTimeNew1 < 10|| m_lInterpTimeNew2<10) {
        return false;
    }
    
    getFirstInterpPhaseGradient();
    getFirstInterpReadGradient();
    getFirstInterpSliceGradient();
    getSecondInterpPhaseGradient();
    getSecondInterpReadGradient();
    getSecondInterpSliceGradient();
    m_bReverseSecondGradientOld = m_bReverseSecondGradient;
    m_lInterpTimeOld1 = m_lInterpTimeNew1;
    m_lInterpTimeOld2 = m_lInterpTimeNew2;
    return true;
}

bool ReadFWFLund::interpGradients(long lInterpTime) {
    m_lInterpTimeNew1 = lInterpTime;    
    m_lInterpTimeNew2 = lInterpTime;
    return interpGradients();
}
    
bool ReadFWFLund::interpGradients(long lInterpTime1, long lInterpTime2) {
    m_lInterpTimeNew1 = lInterpTime1;    
    m_lInterpTimeNew2 = lInterpTime2;
    return interpGradients();
}

std::vector<float> ReadFWFLund::interpGradient(std::vector<float> vfInput, long lAvailableTime){
    long lI;
    
    std::vector<float> interpGrad;
    //lAvailableTime -= m_GRT; //for a final 0 in the end
    interpGrad.reserve(lAvailableTime/GRT);
    if(vfInput.empty() || lAvailableTime<10) {
        return interpGrad;
    }
    //std::cout << "<<ReadFWFLund::interpGradient>> input vector size: " << vfInput.size() << std::endl;
    //std::cout << "<<ReadFWFLund::interpGradient>> input vector last value: "<< vfInput.back() << std::endl;
    if(abs(vfInput.back()) > 0.0001) {
        vfInput.push_back(0);
    }
    if(vfInput.size()<2) {
        vfInput.push_back(0);
    }
    double dTimeStepInitial = 1.0*lAvailableTime/(vfInput.size()-1); 
    long lCurrentOrigTimePoint = 0;
    long m = 0.;
    double dSmallValue = 0, dLargeValue = 0;
    //std::cout << "<<ReadFWFLund::interpGradient>> dTimeStepInitial: " << dTimeStepInitial << ", GRT: " << GRT << std::endl; 
    for(lI=1; lI<=lAvailableTime/GRT; ++lI) {
         //find the points to interpolate from
        m = static_cast<long>(lI*GRT/dTimeStepInitial)+1; //cast to long to floor and +1 for fun     
        if(m < 0) { //can't happen
            dSmallValue = 0;
            dLargeValue = 0; 
        } else if(m == 0) { //shouldn't happen
            dSmallValue = 0;
            dLargeValue = vfInput.at(m);
        } else if(m == vfInput.size()) { // rounding error at last point
            dSmallValue = vfInput.at(m-1);
            dLargeValue = 0;
        } else if(m > vfInput.size()) { //shouldn't happen
            dSmallValue = 0;
            dLargeValue = 0;
        } else {
            dSmallValue = vfInput.at(m-1);
            dLargeValue = vfInput.at(m);
        }
        double dValueInterp = dSmallValue + ((dLargeValue-dSmallValue)/dTimeStepInitial)*(lI*GRT-(m-1)*dTimeStepInitial);
        //if(dValueInterp > 1.0 || dValueInterp < -1.0) {
        //    std::cout << "<<ReadFWFLund::interpGradient>> InterpValue " << dValueInterp << " at pos " << lI << " between values "
        //        << dSmallValue << " and " << dLargeValue << std::endl;
        //}
        interpGrad.push_back(dValueInterp);
    } //for(lI=0; lI<lAvailableTime/lGRT; ++lI) 
    //interpGrad.push_back(0); //so the gradient always ends at zero
    return interpGrad;
}

bool ReadFWFLund::calculateGradientIntegrals(){
    if(m_vfGPInterp1.empty() || m_vfGRInterp1.empty() || m_vfGSInterp1.empty() 
        || m_vfGPInterp2.empty() || m_vfGRInterp2.empty() || m_vfGSInterp2.empty()){
        //maybe set some values if necessary
        return false;
    }
    if(m_vfGPInterp1.size()!=m_vfGRInterp1.size() || m_vfGPInterp1.size()!=m_vfGSInterp1.size() 
        || m_vfGPInterp2.size()!=m_vfGRInterp2.size() || m_vfGPInterp2.size()!=m_vfGSInterp2.size()){
        return false;
    }
    if(m_lInterpTimeNew1 < 0 || m_lInterpTimeOld1 < 0 || m_lInterpTimeNew2 < 0 || m_lInterpTimeOld2 < 0 || m_lRefocDuration < 0) {
        return false;
    }
    unsigned int ui = 0;
    double kx=0., ky=0., kz=0.;
    m_bxx=0.,m_byy=0., m_bzz=0., m_bxy=0., m_bxz=0., m_byz=0.;
    double dAmplP = readGradient1.getGradientAmplitudePhase();
    double dAmplR = readGradient1.getGradientAmplitudeRead();
    double dAmplS = readGradient1.getGradientAmplitudeSlice();
//we assume that there is a 180 pulse after the first gradient, effectively inverting all phase (k-space)
    //effects, hence the -= for k for the first gradient.
    for(ui=0; ui<m_vfGPInterp1.size(); ++ui){
        //numerical integration is done by using the rectangle approximation
        kx -= m_vfGPInterp1[ui]*dAmplP;
        ky -= m_vfGRInterp1[ui]*dAmplR;
        kz -= m_vfGSInterp1[ui]*dAmplS;
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
    m_dCrushMoment = sqrt(kx*kx+ky*ky+kz*kz)*GRT*m_dGamma/1000; //1/m
    
    //pulse duration without gradients (ignore slice refocusing gradient)
    //cout << "ReadBTensor::Zwischenstand:\n\tkx = " << kx << "\n\tky = " << ky << "\n\tkz = " << kz << std::endl;
    //cout << "\tm_dCrushMoment = " << m_dCrushMoment << ", " << "m_dGamma" << m_dGamma <<std::endl;
    //cout << "\tamplitudes = " << dAmplP << " / " << dAmplR << " / " << dAmplS << std::endl;
    //cout << "\tamplitudes2= " << dAmplP2 << " / " << dAmplR2 << " / " << dAmplS2 << std::endl;
    //cout << "\tGRT = " << GRT << ", m_lRefocDuration = " << m_lRefocDuration << std::endl;
    //cout << "\tm_vfGPInterp1.size() = " << m_vfGPInterp1.size() << ", m_vfGPInterp2.size() = " << m_vfGPInterp2.size() << std::endl; 
    
    m_bxx += kx*kx*(m_lRefocDuration/GRT);
    m_byy += ky*ky*(m_lRefocDuration/GRT);
    m_bzz += kz*kz*(m_lRefocDuration/GRT);
    m_bxy += kx*ky*(m_lRefocDuration/GRT);
    m_bxz += kx*kz*(m_lRefocDuration/GRT);
    m_byz += ky*kz*(m_lRefocDuration/GRT);

    //cout << "\tb:\t " << m_bxx*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bxy*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bxz*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << 
    //        "\n\t\t"  << m_bxy*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byy*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byz*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << 
    //        "\n\t\t"  << m_bxz*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byz*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bzz*GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18 << std::endl;

    dAmplP = readGradient2.getGradientAmplitudePhase();
    dAmplR = readGradient2.getGradientAmplitudeRead();
    dAmplS = readGradient2.getGradientAmplitudeSlice();
    for(ui=0; ui<m_vfGPInterp2.size(); ++ui){
        //numerical integration is done by using the rectangle approximation
        kx += m_vfGPInterp2[ui]*dAmplP;
        ky += m_vfGRInterp2[ui]*dAmplR;
        kz += m_vfGSInterp2[ui]*dAmplS;
        m_bxx += kx*kx;
        m_byy += ky*ky;
        m_bzz += kz*kz;
        m_bxy += kx*ky;
        m_bxz += kx*kz;
        m_byz += ky*kz;
    }
    //should be 1/m
    //kx *= GRT*m_dGamma/1000;
    //ky *= GRT*m_dGamma/1000; 
    //kz *= GRT*m_dGamma/1000; 

    //this should give us*mT/m
    m_dLeftoverMoment[0] = kx*GRT;
    m_dLeftoverMoment[1] = ky*GRT;
    m_dLeftoverMoment[2] = kz*GRT;

    m_bxx *= GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18;
    m_byy *= GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18;
    m_bzz *= GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18;
    m_bxy *= GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18;
    m_bxz *= GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18;
    m_byz *= GRT*GRT*GRT*m_dGamma*m_dGamma/1.e18;

    //cout << "ReadBTensor::Endstand:\n\tkx = " << kx << "\n\tky = " << ky << "\n\tkz = " << kz << std::endl;
    //cout << "\tb:\t " << m_bxx << "   " << m_bxy << "   " << m_bxz << 
    //        "\n\t\t"  << m_bxy << "   " << m_byy << "   " << m_byz << 
    //        "\n\t\t"  << m_bxz << "   " << m_byz << "   " << m_bzz << std::endl;

    return true;

}
