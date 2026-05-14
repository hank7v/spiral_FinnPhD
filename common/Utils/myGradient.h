//myGradient class
//used for calculating parameters stemming from the gradients,
//like diffusion weighting, eddy currents, ....

#ifndef MY_GRADIENT_CLASS
#define MY_GRADIENT_CLASS 1
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"

//maximal gradient amplitude in all scanners
#ifndef MAX_GRAD_AMPL
#define MAX_GRAD_AMPL 300
#endif

class myGradient {
public:
    //constructors
    myGradient();
    //myGradient(long lStartTime, long lRampTime, long lDuration, double dAmplitude);
    //expected units: times in us, gradient amplitudes in mT/m, gyromagnetic ratio (dLarmor) in 1/T/us (because that is what Siemens uses(kind of))
    myGradient(long lStartTime, long lRampTime, long lDuration, double dAmplitude, double dLarmor);

//set and get functions for member variables
    void setStartTime(long lT) {m_lStartTime = lT;}
    long getStartTime() {return m_lStartTime;}
    void setRampTime(long lR) {m_lRampTime = lR;}
    long getRampTime() {return m_lRampTime;}
    void setDuration(long lD) {m_lDuration = lD;}
    long getDuration() {return m_lDuration;}
    void setAmplitude(double dA) {m_dAmpl = dA;}
    long getTotalTime() {return m_lDuration+m_lRampTime;}
    double getAmplitude() {return m_dAmpl;}
    void setGamma(double dG) {m_dGamma = dG;}
    double getGamma() {return m_dGamma;}
    //actual Larmor frequency, not the siemens one
    //gam_here = gam_Siemens*2*pi = sSRF01.getLarmorConst()*2*pi
    void setLarmorConst(double dLarmor) {m_dGamma = dLarmor;}
//set functions to set all gradient parameters at once
    void setGradient(const sGRAD_PULSE& sGrad) {
        m_lRampTime = sGrad.getRampUpTime();
        m_lDuration = sGrad.getDuration();
        m_dAmpl = sGrad.getAmplitude();
    }
    void setGradient(const sGRAD_PULSE& sGrad, long lStartTime) {
        m_lRampTime = sGrad.getRampUpTime();
        m_lDuration = sGrad.getDuration();
        m_dAmpl = sGrad.getAmplitude();
        m_lStartTime = lStartTime;
    }
    void setGradient(long lStartTime, long lRampTime, long lDuration, double dAmplitude) {
        m_lRampTime=lRampTime;
        m_lStartTime = lStartTime;
        m_lDuration=lDuration;
        m_dAmpl = dAmplitude;
    }
//check if all gradient parameters have been set.
    bool isGradientSet() {
        return (m_lRampTime>=0 && m_lStartTime>=0 && m_lDuration>=0 && 
            m_dAmpl>=-MAX_GRAD_AMPL && m_dAmpl<=MAX_GRAD_AMPL && m_dGamma>-1);
    };
//for Debugging purposes
    void dumpInfo();
    void dumpInfo(double dK0);
//each gradient adds to certain sequence parameters, these functions determine this.
    double getBValueAddtition(double k0);
    double getBValueAddtition(double scaleFactor, double k0);
    double getBMatrixAddition(double dDirFactor1, double dDirFactor2, double ki, double kj);
    double getM0Addition();
    double getM1Addition();
    double getConAddition(); //concomitant fields
    double getEddyAddition(long lTau);

    //used for sorting a vector of gradients
    bool operator < (myGradient const& str) const
    {
        return (m_lStartTime < str.m_lStartTime);
    }

protected:
    long m_lStartTime;
    long m_lRampTime;
    long m_lDuration;
    double m_dAmpl;
    double m_dGamma;
};

#endif