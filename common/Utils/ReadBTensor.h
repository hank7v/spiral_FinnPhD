//read the trajectory for b-tensor encoding
#ifndef READ_BTENSOR
#define READ_BTENSOR 1
//maybe redesign this whole thing with Read3Trajs as two members
#include "MrServers/MrImaging/seq/common/myHelperClasses/Read3Trajs.h"

class ReadBTensor : public Read3Trajs {
public:
    ReadBTensor();
    ReadBTensor(std::string sFile);
    //the minRiseTime (for possible completion of gradient) 
    //is overwritten if one is given in the file) //I should change that
    ReadBTensor(std::string sFile, double dRiseTime);
    ReadBTensor(std::string sFile, double dRiseTime, double dGamma);
    //destructor 
    virtual ~ReadBTensor();

    //virtual bool interpretHeader(std::string sHeader);

    //maybe include the interpolation in here:
    virtual int readFile(); //necessary for passing to the right interpret header?
    virtual bool interpretHeader(std::string sHeader); 
    virtual std::vector<float> getFirstPhaseGradient() {return vfGradP;}
    virtual std::vector<float> getFirstReadGradient() {return vfGradR;}
    virtual std::vector<float> getFirstSliceGradient() {return vfGradS;}
    virtual std::vector<float> getSecondPhaseGradient() {return vfGradP2;}
    virtual std::vector<float> getSecondReadGradient() {return vfGradR2;}
    virtual std::vector<float> getSecondSliceGradient() {return vfGradS2;} 

    void setFirstGradientAmplitudes(double dAmpl) {m_dAmplP = dAmpl; m_dAmplR=dAmpl; m_dAmplS = dAmpl;}
    void setSecondGradientAmplitudes(double dAmpl) {m_dAmplP2 = dAmpl; m_dAmplR2=dAmpl; m_dAmplS2 = dAmpl;}
    void setFirstGradientAmplitudes(double dAmplP, double dAmplR, double dAmplS) 
        {m_dAmplP = dAmplP; m_dAmplR=dAmplR; m_dAmplS = dAmplS;} 
    void setSecondGradientAmplitudes(double dAmplP, double dAmplR, double dAmplS) 
        {m_dAmplP2 = dAmplP; m_dAmplR2=dAmplR; m_dAmplS2 = dAmplS;} 
    
    double getFirstGradientAmplitudePhase() {return m_dAmplP;}
    double getFirstGradientAmplitudeRead() {return m_dAmplR;}
    double getFirstGradientAmplitudeSlice() {return m_dAmplS;}
    double getSecondGradientAmplitudePhase() {return m_dAmplP2;}
    double getSecondGradientAmplitudeRead() {return m_dAmplR2;}
    double getSecondGradientAmplitudeSlice() {return m_dAmplS2;}
    
    void useAmplitudesFromFile(){
        m_dAmplP = m_dPhaseAmplInFile.at(0);
        m_dAmplR = m_dReadAmplInFile.at(0);
        m_dAmplS = m_dSliceAmplInFile.at(0);
        m_dAmplP2 = m_dPhaseAmplInFile.at(1);
        m_dAmplR2 = m_dReadAmplInFile.at(1);
        m_dAmplS2 = m_dSliceAmplInFile.at(1);
    }

    void setPulseDuration(long lDur) {m_lPulseDuration = lDur;}
    long getPulseDurationInFile() {return m_lPulseDurationInFile;}
    long getFirstLength(){return vfGradP.size();}
    long getSecondLength(){return vfGradP2.size();}

    double getBValue() {return m_bxx+m_byy+m_bzz;}
    void getBTensor(double bTens[3][3]);
    double getCrushMoment(){return m_dCrushMoment;}
    //determine bTensor, gradient moments
    bool calculateGradientIntegrals();

protected:
    virtual std::vector<float> interpolateGradient( std::vector<float> const &  gradient, long noInterp);
    //used to determine minimum interpolation length
    float getLargestStep(std::vector<float> const & gradPhase, std::vector<float> const & gradRead, std::vector<float> const & gradSlice);
    void interpolateAllGradients(std::vector<float> const & gradPhase, std::vector<float> const & gradRead, std::vector<float> const & gradSlice);
    

    //Read3Trajs version only adds to the first gradient set
    void completeGradientForm(); 
    void completeGradientForm(double dMinRiseTime);

    //maybe for checking refocusing
    //float getIntegral(std::vector<float> gradient);
    
    //std::vector <float> m_vfInterpPhase;
    //std::vector <float> m_vfInterpRead;
    //std::vector <float> m_vfInterpSlice;
    long m_lNoInterpSteps;
    std::string m_sFileBaseName;
    std::vector<float> vfGradP2;
    std::vector<float> vfGradR2;
    std::vector<float> vfGradS2;
    bool m_bUseIdenticalGradients;
    //duration for rf between gradients
    long m_lPulseDurationInFile; 
    long m_lPulseDuration;
    double m_bxx, m_byy, m_bzz, m_bxy, m_bxz, m_byz;
    double m_dCrushMoment;
    //I'll probably won't use the value in file but need it as warning
    std::vector<double> m_dPhaseAmplInFile; 
    std::vector<double> m_dReadAmplInFile;
    std::vector<double> m_dSliceAmplInFile;
    std::vector<double> m_dPhaseRiseTimeInFile;
    std::vector<double> m_dReadRiseTimeInFile;
    std::vector<double> m_dSliceRiseTimeInFile;
    unsigned int m_index;
    double m_dAmplP2;
    double m_dAmplR2;
    double m_dAmplS2;

    double m_dGamma;
};

#endif //double include guard