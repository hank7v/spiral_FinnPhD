//Class that is used to read three gradient trajectories
//This assumes three columns in the file for gradient trajectories.
//It allows to give information in a file header.

//Each line starting with '#' is considered potential header
//For the interpretation of the info keywords can be used
//which are listed before the interpretHeader function.

//The first column is taken as phase encoding, the second one 
//as read encoding direction as defined in the sequence.

//So far no checks about the validity of the trajectory is 
//performed. The values should range from -1.0 to 1.0.

//TODO: copy constructor

//double include guard
#ifndef READ_THREE_TRAJECTORIES_H
#define READ_THREE_TRAJECTORIES_H

#include "MrServers/MrImaging/seq/common/myHelperClasses/FileReadBase.h"

class Read3Trajs : public FileReadBase {
public:
    //constructor
    Read3Trajs();
    Read3Trajs(std::string sFile);
    //the minRiseTime (for possible completion of gradient) 
    //is overwritten if on is given in the file)
    Read3Trajs(std::string sFile, double dRiseTime);
    //destructor 
    virtual ~Read3Trajs();


    virtual std::vector<float> getPhaseGradient();
    virtual std::vector<float> getReadGradient();
    virtual std::vector<float> getSliceGradient();
    //here a gradient raster time of 10us is hardcoded in the constructor
    virtual long getGradientDuration() {return m_lActualLength*m_GRT;}
    //The readFile() function might complete gradient trajectory, i.e.
    //if the at least one gradient does not end at zero, both are 
    //prolonged and ramped down with m_dRiseTime. This can lead to a 
    //discrepency between FileReadBase::m_lLenght and the actual array length 
    long getActualLength() {return m_lActualLength;}

    //be careful with the header in your file, keywords should be followed by a number,
    //if the function finds a keyword the next number is assigned to the parameter 
    //no matter how many stuff is between them.
    //header lines start with '#', comment lines with '/' (of course without ').
    //possible header keywords are:
    //"maxGradAmpl", "maxGradAmplP", "maxGradAmplR": if P or R are given without is ignored
    //"rampDownTime": not yet supported, I think
    //"minRiseTime": default is 20. or the one given in the constructor
    //only the first occurence of a keyword is used
    //if no version of "maxGradAmpl" is given, a header line with two numbers is searched,
    //if this can't be found either the return value is false, otherwise it's true
    virtual bool interpretHeader(std::string sHeader);

    double getGradientAmplitudePhase() {return m_dAmplP;}
    double getGradientAmplitudeRead() {return m_dAmplR;}
    double getGradientAmplitudeSlice() {return m_dAmplS;}
    double getGradientAmplitude(long lId) {
        switch(lId) {
            case 0: return m_dAmplP;
            case 1: return m_dAmplR;
            case 2: return m_dAmplS;
        }
        return 0;
    }

    //if these are called after fileRead(), the values in the file
    //are overwritten without any checks or warning
    void setGradientAmplitudePhase(double dAmpl) {m_dAmplP = dAmpl;}
    void setGradientAmplitudeRead(double dAmpl) {m_dAmplR = dAmpl;}
    void setGradientAmplitudeSlice(double dAmpl) {m_dAmplS = dAmpl;}
    void setGradientAmplitudes(double dAmpl) {m_dAmplP = dAmpl; m_dAmplR=dAmpl; m_dAmplS = dAmpl;}
    void setGradientAmplitudes(double dAmpl[3]) {m_dAmplP = dAmpl[0]; m_dAmplR=dAmpl[1]; m_dAmplS = dAmpl[2];}
    //only used for the completion of a gradient form not going to zero
    void setMinRiseTime(double dRT) {m_dRiseTime = dRT;}
    double getMinRiseTime() {return m_dRiseTime;}
    //void setlRampTime(long lRT) {m_lRampTime}

    //maybe: normalize with respect to highest absolute value (only if >1?)
    //       check for highest values
    
    //this additionally can use completeGradientForm
    virtual int readFile(bool bCompleteGradientForm=true, bool bNeverUseFirstValueAsAmplitude=false);
    bool checkExistence() {return FileReadBase::bCheckExistence();}
protected:
    //used to add a ramp down at the end of the gradient if needed
    void completeGradientForm(); 
    void completeGradientForm(double dMinRiseTime);
    void completeGradientForm(long lRampDownTime);
    //long m_lCol = 2; //don't know if this works 
    //make the access private, so no one changes it
    
    double m_dAmplP;
    double m_dAmplR;
    double m_dAmplS;
    double m_dRiseTime;
    long m_lRampTime;
    //This one is needed as readFile() uses m_lLength to limit the reading range
    //which could lead to a bug, as in several cases, the length of the spiral 
    //is changed. If the actual length would recorded in m_lLength, a second 
    //fileRead might read a different length. 
    long m_lActualLength;
    //Those are needed if the gradients don't end at zero.
    std::vector<float> vfGradP;
    std::vector<float> vfGradR;
    std::vector<float> vfGradS;
    //don't have to be class members
    double extractDouble(std::string);
    long extractLong(std::string);

    const long m_GRT;

private:
    using FileReadBase::m_lCol;
    using FileReadBase::setNoColumns; 
};

#endif //double include guard