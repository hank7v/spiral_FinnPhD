//myGradient class
//used for calculating parameters stemming from the gradients,
//like diffusion weighting, eddy currents, ....

#ifndef MY_GRADIENT_EVENT_CLASS
#define MY_GRADIENT_EVENT_CLASS 1

//maximal gradient amplitude in all scanners
#ifndef MAX_GRAD_AMPL
    #if VX_VERSION == VX_VERSION_VD11D //Connectom
        #define MAX_GRAD_AMPL 300
    #elif VX_VERSION < VX_VERSION_VD //7T
        #define MAX_GRAD_AMPL 70
    #else                               //Prisma
        #define MAX_GRAD_AMPL 80
    #endif
#endif

class myGradientEvent {
public:
    //constructors
    myGradientEvent();
    //myGradient(long lStartTime, long lRampTime, long lDuration, double dAmplitude);
    myGradientEvent(long lStartTime, long lDuration, double dAmplStart, double dAmplEnd, double dLarmor);

    //destructor
    ~myGradientEvent();
//set and get functions for member variables
    void setStartTime(long lT) {m_lStartTime = lT;}
    long getStartTime() {return m_lStartTime;}
    void setDuration(long lD) {m_lDuration = lD;}
    long getDuration() {return m_lDuration;}
    void setAmplitudeStart(double dA) {m_dAmpl1 = dA;}
    double getAmplitudeStart() {return m_dAmpl1;}
    void setAmplitudeEnd(double dA) {m_dAmpl2 = dA;}
    double getAmplitudeEnd() {return m_dAmpl2;}
    void setGamma(double dG) {m_dGamma = dG;}
    double getGamma() {return m_dGamma;}
    //actual Larmor frequency, not the siemens one
    //gam_here = gam_Siemens*2*pi = sSRF01.getLarmorConst()*2*pi
    void setLarmorConst(double dLarmor) {m_dGamma = dLarmor;}
//set functions to set all gradient parameters at once
    
    virtual void setGradientEvent(long lStartTime, long lDuration, double dAmplStart, double dAmplEnd) {
        m_lStartTime = lStartTime;
        m_lDuration=lDuration;
        m_dAmpl1 = dAmplStart;
        m_dAmpl2 = dAmplEnd;
    }
//check if all gradient parameters have been set.
    virtual bool isGradientSet() {
        return (m_lDuration>=0 && m_lStartTime>=0 && m_dGamma>-1 &&
            m_dAmpl1>=-MAX_GRAD_AMPL && m_dAmpl1<=MAX_GRAD_AMPL && 
            m_dAmpl2>=-MAX_GRAD_AMPL && m_dAmpl2<=MAX_GRAD_AMPL);
    };
//for Debugging purposes
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

    //used for sorting a vector of gradient events
    bool operator < (myGradientEvent const& str) const
    {
        return (m_lStartTime < str.m_lStartTime);
    }


protected:
    long m_lStartTime;
    long m_lDuration;
    //for arbitrary gradient do vector<double> m_vdAmpl;
    double m_dAmpl1; //amplitude at the beginning
    double m_dAmpl2; // amplitude at the end
    double m_dGamma;

    double m_dK0; //k-space at the beginning of the gradient.
};

#endif