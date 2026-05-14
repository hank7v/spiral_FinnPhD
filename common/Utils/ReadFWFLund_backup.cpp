#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadFWFLund.h"
#include <algorithm>
#include <iostream>

#define LARMOR_OF_WATER 267.513
#define GRT 10

ReadFWFLund::ReadFWFLund():
Read3Trajs(),
m_lInterpTimeOld(-1),
m_lInterpTimeNew(-1),
m_lRefocDuration(-1),
m_bxx(0), 
m_byy(0), 
m_bzz(0), 
m_bxy(0), 
m_bxz(0), 
m_byz(0),
m_dCrushMoment(0),
m_dGamma(LARMOR_OF_WATER)
{
}
    
ReadFWFLund::ReadFWFLund(std::string sFile):
Read3Trajs(sFile),
m_lInterpTimeOld(-1),
m_lInterpTimeNew(-1),
m_lRefocDuration(-1),
m_bxx(0), 
m_byy(0), 
m_bzz(0), 
m_bxy(0), 
m_bxz(0), 
m_byz(0),
m_dCrushMoment(0),
m_dGamma(LARMOR_OF_WATER)
{
}

ReadFWFLund::~ReadFWFLund()
{
}
    
int ReadFWFLund::readFile(){
    if(m_lLength < 1){ 
        m_lLength = FileReadBase::getFileLength();
    }
    int iRet = FileReadBase::readFile(m_lLength);
    if(iRet< 0) return iRet;

    vfGradP = FileReadBase::getColumnAsVector(0);
    vfGradR = FileReadBase::getColumnAsVector(1);
    vfGradS = FileReadBase::getColumnAsVector(2);

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
    m_lInterpTimeOld = 0;
    //no completion of the gradient, as we don't know what the time step is
    return iRet;
}
    
std::vector<float> ReadFWFLund::getInterpPhaseGradient() {
    if(m_lInterpTimeNew!=m_lInterpTimeOld && m_lInterpTimeNew>(m_GRT-1)) {
        if(!m_vfGPInterp.empty() || m_vfGPInterp.size() != m_lInterpTimeNew/10) {
            m_vfGPInterp = interpGradient(vfGradP, m_lInterpTimeNew);
        }
    }
    return m_vfGPInterp;
}

std::vector<float> ReadFWFLund::getInterpReadGradient() {
    if(m_lInterpTimeNew!=m_lInterpTimeOld && m_lInterpTimeNew>(m_GRT-1)) {
        if(!m_vfGRInterp.empty() || m_vfGRInterp.size() != m_lInterpTimeNew/10) {
            m_vfGRInterp = interpGradient(vfGradR, m_lInterpTimeNew);
        }
    }
    return m_vfGRInterp;
}
    
std::vector<float> ReadFWFLund::getInterpSliceGradient() {
    if(m_lInterpTimeNew!=m_lInterpTimeOld && m_lInterpTimeNew>(m_GRT-1)) {
        if(!m_vfGSInterp.empty() || m_vfGSInterp.size() != m_lInterpTimeNew/10) {
            m_vfGSInterp = interpGradient(vfGradS, m_lInterpTimeNew);
        }
    }
    return m_vfGSInterp;
}

bool ReadFWFLund::interpGradients() {
    return interpGradients(m_lInterpTimeNew);
}

bool ReadFWFLund::interpGradients(long lInterpTime) {
    if (lInterpTime < 10) {
        return false;
    }

    if(lInterpTime!=m_lInterpTimeOld) {
        //std::cout << "interp slice ..."; 
        if(!m_vfGSInterp.empty() || m_vfGSInterp.size() != lInterpTime/10) {
            m_vfGSInterp = interpGradient(vfGradS, lInterpTime);
        }
        //std::cout << "interp read ...";
        if(!m_vfGRInterp.empty() || m_vfGRInterp.size() != lInterpTime/10) {
            m_vfGRInterp = interpGradient(vfGradR, lInterpTime);
        }
        //std::cout << "interp phase ...";
        if(!m_vfGPInterp.empty() || m_vfGPInterp.size() != lInterpTime/10) {
            m_vfGPInterp = interpGradient(vfGradP, lInterpTime);
        }
        m_lInterpTimeNew = lInterpTime;
        m_lInterpTimeOld = lInterpTime;
    }
    return true;
}
    

std::vector<float> ReadFWFLund::interpGradient(std::vector<float> vfInput, long lAvailableTime){
    long lI;
    
    std::vector<float> interpGrad;
    lAvailableTime -= m_GRT; //for a final 0 in the end
    interpGrad.reserve(lAvailableTime/m_GRT+1);
    if(vfInput.empty() || lAvailableTime<10) {
        return interpGrad;
    }
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
    //std::cout << "dTimeStepInitial: " << dTimeStepInitial << ", m_GRT: " << m_GRT << std::endl; 
    for(lI=0; lI<lAvailableTime/m_GRT; ++lI) {
         //find the points to interpolate from
        m = static_cast<long>(lI*m_GRT/dTimeStepInitial)+1;        
        if(m < 0) {
            dSmallValue = 0;
            dLargeValue = 0; 
        } else if(m == 0) {
            dSmallValue = 0;
            dLargeValue = vfInput.at(m);
        } else if(m == vfInput.size()) {
            dSmallValue = vfInput.at(m-1);
            dLargeValue = 0;
        } else if(m > vfInput.size()) {
            dSmallValue = 0;
            dLargeValue = 0;
        } else {
            dSmallValue = vfInput.at(m-1);
            dLargeValue = vfInput.at(m);
        }
        double dValueInterp = dSmallValue + ((dLargeValue-dSmallValue)/dTimeStepInitial)*(lI*m_GRT-(m-1)*dTimeStepInitial);
        //if(dValueInterp > 1.0 || dValueInterp < -1.0) {
        //    std::cout << "InterpValue " << dValueInterp << " at pos " << lI << " between values "
        //        << dSmallValue << " and " << dLargeValue << std::endl;
        //}
        interpGrad.push_back(dValueInterp);
    } //for(lI=0; lI<lAvailableTime/lGRT; ++lI) 
    interpGrad.push_back(0); //so the gradient always ends at zero
    return interpGrad;
}

bool ReadFWFLund::calculateGradientIntegrals(){
    if(m_vfGPInterp.empty() || m_vfGRInterp.empty() || m_vfGSInterp.empty()){
        //maybe set some values if necessary
        return false;
    }
    if(m_vfGPInterp.size()!=m_vfGRInterp.size() || m_vfGPInterp.size()!=m_vfGSInterp.size()){
        return false;
    }
    if(m_lInterpTimeNew < 0 || m_lInterpTimeOld < 0 || m_lRefocDuration < 0) {
        return false;
    }
    unsigned int ui = 0;
    double kx=0., ky=0., kz=0.;
    m_bxx=0.,m_byy=0., m_bzz=0., m_bxy=0., m_bxz=0., m_byz=0.;
    
//we assume that there is a 180 pulse after the first gradient, effectively inverting all phase (k-space)
    //effects, hence the -= for k for the first gradient.
    for(ui=0; ui<m_vfGPInterp.size(); ++ui){
        //numerical integration is done by using the rectangle approximation
        kx -= m_vfGPInterp[ui]*m_dAmplP;
        ky -= m_vfGRInterp[ui]*m_dAmplR;
        kz -= m_vfGSInterp[ui]*m_dAmplS;
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
    
    m_bxx += kx*kx*(m_lRefocDuration/m_GRT);
    m_byy += ky*ky*(m_lRefocDuration/m_GRT);
    m_bzz += kz*kz*(m_lRefocDuration/m_GRT);
    m_bxy += kx*ky*(m_lRefocDuration/m_GRT);
    m_bxz += kx*kz*(m_lRefocDuration/m_GRT);
    m_byz += ky*kz*(m_lRefocDuration/m_GRT);

  /*  cout << "\tb:\t " << m_bxx*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bxy*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bxz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << 
            "\n\t\t"  << m_bxy*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byy*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << 
            "\n\t\t"  << m_bxz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_byz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << "   " << m_bzz*m_GRT*m_GRT*m_GRT*m_dGamma*m_dGamma/1.e18 << std::endl;
*/
    for(ui=0; ui<m_vfGPInterp.size(); ++ui){
        //numerical integration is done by using the rectangle approximation
        kx += m_vfGPInterp[ui]*m_dAmplP;
        ky += m_vfGRInterp[ui]*m_dAmplR;
        kz += m_vfGSInterp[ui]*m_dAmplS;
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
