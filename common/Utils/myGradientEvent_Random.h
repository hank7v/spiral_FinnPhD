//random gradient event for random gradients

#ifndef MY_GRADIENT_EVENT_RANDOM_CLASS
#define MY_GRADIENT_EVENT_RANDOM_CLASS
#include "myGradientEvent.h"
#include <vector>
class myGradientEvent_Random : public myGradientEvent {
    myGradientEvent_Random();
    //myGradient(long lStartTime, long lRampTime, long lDuration, double dAmplitude);
    myGradientEvent(long lStartTime, long lDuration, double dAmpl, std::vector<float> vfRelAmpl, double dLarmor);

    void setAmplitude(double dAmpl) {m_dAmpl1 = dAmpl;} 
    double getAmplitude() {return dAmpl;}
    void setRelAmpltude(std::vector<float> vAmpl) {m_vfRelAmpl = vAmpl;}
    std::vector<float> getRelAmplitude() {return m_vfRelAmpl;}

    virtual void dumpInfo();
    virtual void dumpInfo(double dK0);
//each gradient adds to certain sequence parameters, these functions determine this.
    virtual double getBValueAddtition(double k0);
    virtual double getBValueAddtition(double scaleFactor, double k0);
    virtual double getBMatrixAddition(double dDirFactor1, double dDirFactor2, double ki, double kj);
    virtual double getM0Addition();
    virtual double getM1Addition();
    virtual double getConAddition(); //concomitant fields
    virtual double getEddyAddition(long lTau);

    void updateGradientEffects();

protected:
    std::vector<float> m_vfRelAmpl;
    
}; //end of class myGradientEvent_Random

#endif //double inclusion guard