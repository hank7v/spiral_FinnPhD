#ifndef READ_FWF_LUND
#define READ_FWF_LUND

//class for reading in the (minorly modified) Free Wave Form files from Lund

//TODO get some checks and status flags in place 
#include "MrServers/MrImaging/seq/common/myHelperClasses/Read3Trajs.h"
#include<cmath>

class ReadFWFLund  {
public:
    ReadFWFLund();
    ReadFWFLund(std::string sFile);
    //destructor 
    ~ReadFWFLund();
    
    //actual file names must have .txt extension, this extension can be 
    //included in the string passed to the object but doesn't need to. 
    //the name passed here must not include the _A or _B of the external 
    //filename in case of an asymmetric diffusion design.
    //internally the base name is saved, that is without .txt extension or 
    //_A or _B. They are added to the name string when needed. 
    //The filename should be the full path to the file
    virtual int readFile();
    void setFilename(std::string sFilename);
    std::string getFilename();
    
    //interpolate the first gradient to m_lInterpTimeNew (if necessary) and return it
    virtual std::vector<float> getInterpPhaseGradient();
    virtual std::vector<float> getInterpReadGradient();
    virtual std::vector<float> getInterpSliceGradient();
    //time reversal of the first interpolated gradient (do interpolation if necessary)
    virtual std::vector<float> getInterpPhaseGradientReversed();
    virtual std::vector<float> getInterpReadGradientReversed();
    virtual std::vector<float> getInterpSliceGradientReversed();
    //interpolate and return first or second gradient on certain axis
    virtual std::vector<float> getFirstInterpPhaseGradient();
    virtual std::vector<float> getFirstInterpReadGradient();
    virtual std::vector<float> getFirstInterpSliceGradient();
    virtual std::vector<float> getSecondInterpPhaseGradient();
    virtual std::vector<float> getSecondInterpReadGradient();
    virtual std::vector<float> getSecondInterpSliceGradient();   

    //refocusing duration is time between the two diffusion gradients and 
    //needed here for the correct calculation of the b-matrix
    void setRefocDuration(long lDur) {m_lRefocDuration = lDur;}
    //set and get time the gradients are interpolated to
    void setInterpTime(long lDur) {m_lInterpTimeNew1 = floor(lDur/10)*10; m_lInterpTimeNew2 = floor(lDur/10)*10;}
    long getInterpTime() {return m_lInterpTimeOld1;}
    void setFirstInterpTime(long lDur) {m_lInterpTimeNew1 = floor(lDur/10)*10;}
    long getFirstInterpTime() {return m_lInterpTimeOld1;}
    void setSecondInterpTime(long lDur) {m_lInterpTimeNew2 = floor(lDur/10)*10;}
    long getSecondInterpTime() {return m_lInterpTimeOld2;}
    void setInterpTime(long lDur1, long lDur2) {m_lInterpTimeNew1 = floor(lDur1/10)*10; m_lInterpTimeNew2 = floor(lDur2/10)*10;}
    //choose if you want to use asymmetric diffusion gradients
    void setAsymmetric(bool bIsAsym = true) {m_isAsymmetric = bIsAsym;}
    void setSymmetric(bool bIsSym = true) {m_isAsymmetric = !bIsSym;}
    bool setAsymmetricBasedOnFiles(); //returns true if _A and _B of the filename exist and sets m_isAsymmetric accordingly
    bool isAsymmetric() {return m_isAsymmetric;}
    bool isSymmetric() {return !m_isAsymmetric;}
    //for the symmetric designed gradients in the NOW toolbox the second one is the same as the 
    //first, just played out from back to front, that is reversed in time
    void setReverseSecondGradient(bool bReverseIt = true) {m_bReverseSecondGradient = bReverseIt;}
    bool isSecondGradientReversed() {return m_bReverseSecondGradient;}
    
    double getFirstGradientAmplitudePhase() {return readGradient1.getGradientAmplitudePhase();}
    double getFirstGradientAmplitudeRead() {return readGradient1.getGradientAmplitudeRead();}
    double getFirstGradientAmplitudeSlice() {return readGradient1.getGradientAmplitudeSlice();}
    double getSecondGradientAmplitudePhase() {return readGradient2.getGradientAmplitudePhase();}
    double getSecondGradientAmplitudeRead() {return readGradient2.getGradientAmplitudeRead();}
    double getSecondGradientAmplitudeSlice() {return readGradient2.getGradientAmplitudeSlice();}
    //do the linear interpolation of all gradients individually
    bool interpGradients();
    bool interpGradients(long lInterpTime);
    bool interpGradients(long lInterpTime1, long lInterpTime2);

 
    void setGradientAmplitudes(double dAmpl) {
        readGradient1.setGradientAmplitudes(dAmpl);
        readGradient2.setGradientAmplitudes(dAmpl);
    }
    void setGradientAmplitudes(double dAmpl[3]) {
        readGradient1.setGradientAmplitudes(dAmpl);
        readGradient2.setGradientAmplitudes(dAmpl);
    }
    void setGradientAmplitudes(double dAmpl1, double dAmpl2){
        readGradient1.setGradientAmplitudes(dAmpl1);
        readGradient2.setGradientAmplitudes(dAmpl2);
    }
    void setGradientAmplitudes(double dAmpl1[3], double dAmpl2[3]){
        readGradient1.setGradientAmplitudes(dAmpl1);
        readGradient2.setGradientAmplitudes(dAmpl2);
    }

    bool calculateGradientIntegrals();
    double getBValue() {return m_bxx+m_byy+m_bzz;}
    void getBTensor(double bTens[3][3]){ 
        bTens[0][0]=m_bxx; bTens[0][1]=m_bxy; bTens[0][2]=m_bxz;
        bTens[1][0]=m_bxy; bTens[1][1]=m_byy; bTens[1][2]=m_byz;
        bTens[2][0]=m_bxz; bTens[2][1]=m_byz; bTens[2][2]=m_bzz; 
    }
    double getCrushMoment(){return m_dCrushMoment;}

    double getFirstGradientAmplitude(long lI){
        switch(lI) {
            case 0:
                return readGradient1.getGradientAmplitudePhase();
                break;
            case 1:
                return readGradient1.getGradientAmplitudeRead();
                break;
            case 2:
                return readGradient1.getGradientAmplitudeSlice();
                break;
        }
        return -1;
    }
    double getSecondGradientAmplitude(long lI){
        switch(lI) {
            case 0:
                return readGradient2.getGradientAmplitudePhase();
                break;
            case 1:
                return readGradient2.getGradientAmplitudeRead();
                break;
            case 2:
                return readGradient2.getGradientAmplitudeSlice();
                break;
        }
        return -1;
    }
    //For backward compatibility and maybe symmetric use
    double getGradientAmplitude(long lI){
        return getFirstGradientAmplitude(lI);
    }
    
    long getFirstLength() {
        return readGradient1.getLength();
    }
    long getSecondLength(){
        return readGradient1.getLength();
    }
    
    //mostly for full backwards compatibility, although they shouldn't really be in use for this class
    void setLengthToFileLength() {
        readGradient1.setLengthToFileLength();
        readGradient2.setLengthToFileLength();
    }
    long getLength() {return readGradient1.getLength();}
    long getFileLengthFirst() {return readGradient1.getFileLength();}
    long getFileLengthSecond() {return readGradient2.getFileLength();}
    long getFileLength() {return getFileLengthFirst();}

    double getRemGradMomentPhase() {return m_dLeftoverMoment[0];}
    double getRemGradMomentRead() {return m_dLeftoverMoment[1];}
    double getRemGradMomentSlice() {return m_dLeftoverMoment[2];}
    double getRemGradMoment(long lDir) {
        if(lDir>2||lDir<0)
            return 0;
        else
            return m_dLeftoverMoment[lDir];
    }
protected:
    long m_lInterpTimeOld1;
    long m_lInterpTimeOld2;
    long m_lInterpTimeNew1; //only interpolate if necessary
    long m_lInterpTimeNew2; //only interpolate if necessary
    long m_lRefocDuration;
    double m_bxx, m_byy, m_bzz, m_bxy, m_bxz, m_byz;
    double m_dCrushMoment;
    double m_dLeftoverMoment[3];
    double m_dGamma;

    bool m_isAsymmetric;
    bool m_bReverseSecondGradient;
    bool m_bReverseSecondGradientOld;
    bool m_bCheckGradientVectors;
    std::string m_sNameBase; //filename for reading without A and B
    
    std::vector<float> m_vfGPInterp1;
    std::vector<float> m_vfGRInterp1;
    std::vector<float> m_vfGSInterp1;

    std::vector<float> m_vfGPInterp2;
    std::vector<float> m_vfGRInterp2;
    std::vector<float> m_vfGSInterp2;

    std::vector<float> interpGradient(std::vector<float> vfInput, long lAvailableTime);

    Read3Trajs readGradient1;
    Read3Trajs readGradient2;
private:    

};


#endif //#ifndef READ_FWF_LUND