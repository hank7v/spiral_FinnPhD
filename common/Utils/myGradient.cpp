//myGradient class implementations
#include "myGradient.h"
#include <cmath>
#include <iostream>

myGradient::myGradient():
    m_lStartTime(-1)
,   m_lRampTime (-1)
,   m_lDuration (-1)
,   m_dAmpl     (MAX_GRAD_AMPL + 1.)
,   m_dGamma    (-1)
    {}

myGradient::myGradient(long lStartTime, long lRampTime, long lDuration, double dAmplitude, double dLarmor)
: m_lStartTime(lStartTime)
, m_lRampTime (lRampTime)
, m_lDuration (lDuration)
, m_dAmpl     (dAmplitude)
, m_dGamma    (dLarmor)
{}


double myGradient::getBValueAddtition(double k0) {
    return( getBMatrixAddition(1., 1., k0, k0) );
}

double myGradient::getBValueAddtition(double scaleFactor, double k0) {
    return( getBMatrixAddition(scaleFactor, scaleFactor, k0, k0) );
}

double myGradient::getBMatrixAddition(double dDirFactor1, double dDirFactor2, double ki, double kj) {
    double r = double(m_lRampTime)*1.e-6; //in s
    double f = double(m_lDuration)*1.e-6 - r; //in s
    //double gam = m_dGamma;
    //double G = m_dAmpl;
                //gamma*1e3 is in 1/(mT * s) Ampl*1e-3 is in T/m which is mT/mm so we get 1/(s*mm)
    double gi = m_dGamma*m_dAmpl*dDirFactor1;//only the product appears in the formula
    double gj = m_dGamma*m_dAmpl*dDirFactor2;
    double b = 0.;
//(1/mm)^2*s=s/mm^2  (1/mm)*(1/(s*mm))*s^2=s/mm^2   (1/(s*mm))^2*(s^3) = s/(mm^2)
    b += ki*kj*r + ki*gj*r*r/6. + kj*gi*r*r/6. + gi*gj*r*r*r/20.;
      //  1/(s*mm)*s = 1/mm   
    ki += gi*r/2; kj+= gj*r/2;

    b += ki*kj*f + ki*gj*f*f/2. + kj*gi*f*f/2. + gi*gj*f*f*f/3. ;
    ki += gi*f; kj += gj*f;

    b += ki*kj*r + ki*gj*r*r/3. + kj*gi*r*r/3. + gi*gj*r*r*r*2/15;
    return b; //this should now be in s/mm^2 which is the unit in ep2d_diff I suppose
}


double myGradient::getM0Addition() {
    //(mT/m)*us/1e9 = mT/mm * s 
    return m_dAmpl*m_lDuration*1.e-9;
}
    
double myGradient::getM1Addition() {
    double r = double(m_lRampTime)*1.e-6;
    double f = double(m_lDuration)*1.e-6 - r;
    double t0 = double(m_lStartTime)*1.e-6;
// ( (mT/m)/1000)*s*s = mT/mm * s^2
    return (m_dAmpl*1.e-3)*(r*r/2 + r/2*(2*t0+f+r) + f*f/2 + f*(t0+r));
}

//concomitant fields
double myGradient::getConAddition() { 
    //(mT/mm)^2 * s
    return m_dAmpl*m_dAmpl*1.e-12*(m_lDuration - m_lRampTime/3);
}

double myGradient::getEddyAddition(long lTau) {
    double T = double(m_lStartTime + m_lDuration + m_lRampTime)*1.e-6; 
    double dTau = double(lTau)*1.e-6;
    return m_dAmpl/m_lRampTime*lTau*exp(-1.*(T-m_lStartTime)/lTau)*(1-exp(-1.*m_lRampTime/lTau))
        + m_dAmpl/m_lRampTime*lTau*exp(-1.*(T-m_lStartTime-m_lDuration)/lTau)*(exp(-1.*m_lRampTime/lTau)-1);
}

void myGradient::dumpInfo(double dK0) {
    std::cout << "startTime: " << m_lStartTime << ", duration: " << m_lDuration << 
        ", ramp: " << m_lRampTime << "\namplitude: " << m_dAmpl << ", gamma: " << m_dGamma << std::endl;
    std::cout << "B: " << getBMatrixAddition(1., 1., dK0, dK0) << ", M0: " << getM0Addition() <<
        ", M1: " << getM1Addition() << "\neddy: " << getEddyAddition(70000) << ", Conco: " << 
        getConAddition()  << std::endl; 
}

void myGradient::dumpInfo() {
    dumpInfo(0.);
}