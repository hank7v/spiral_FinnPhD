//myGradientEvent (either ramp or flat)
#include "myGradientEvent.h"

#ifndef LARMOR_OF_WATER
#define LARMOR_OF_WATER 267.513
#endif

myGradientEvent::myGradientEvent() :
m_lStartTime (0),
m_lDuration  (0),
m_dAmpl1     (0.0),
m_dAmpl2     (0.0),
m_dGamma     (LARMOR_OF_WATER)
m_dK0        (0.0)
{}

myGradientEvent::myGradientEvent(long lStartTime, long lDuration, double dAmplStart, double dAmplEnd, double dLarmor) :
m_lStartTime (lStartTime),
m_lDuration  (lDuration),
m_dAmpl1     (dAmplStart),
m_dAmpl2     (dAmplEnd),
m_dGamma     (LARMOR_OF_WATER)
m_dK0        (0.0)
{}

//destructor
~myGradientEvent();

virtual void dumpInfo() {
    std::cout << "Values b-Matrix: "
//virtual void dumpInfo(double dK0);
//each gradient adds to certain sequence parameters, these functions determine this.
//virtual double getBValueAddtition(double k0);
//virtual double getBValueAddtition(double scaleFactor, double k0);
//virtual double getBMatrixAddition(double dDirFactor1, double dDirFactor2, double ki, double kj);
virtual double getM0Addition();
virtual double getM1Addition();
//virtual double getConAddition(); //concomitant fields
virtual double getEddyAddition(long lTau);


