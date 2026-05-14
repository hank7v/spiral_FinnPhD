//Class that is used to read readout trajectories
//This assumes two columns in the file for gradient trajectories.
//It allows to give information in a file header.

//Each line starting with '#' is considered potential header
//For the interpretation of the info keywords can be used
//which are listed before the interpretHeader function.

//The first column is taken as phase encoding, the second one 
//as read encoding direction as defined in the sequence.

//So far no checks about the validity of the trajectory is 
//performed. The values should range from -1.0 to 1.0.

//double include guard
#ifndef READ_EXTERNAL_SPIRAL_H
#define READ_EXTERNAL_SPIRAL_H

#include "MrServers/MrImaging/seq/common/myHelperClasses/FileReadBase.h"

class ReadExternalSpiral : public FileReadBase {
public:
    //constructor
    ReadExternalSpiral();
    ReadExternalSpiral(std::string sFile);
    //the minRiseTime (for possible completion of gradient) 
    //is overwritten if on is given in the file)
    ReadExternalSpiral(std::string sFile, double dRiseTime);
    //destructor 
    ~ReadExternalSpiral();


    std::vector<float> getPhaseGradient();
    std::vector<float> getReadGradient();
    //here a gradient raster time of 10us is hardcoded in the constructor
    long getGradientDuration() {return m_lActualLength*m_GRT;}
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
    //"maxGradAmpl", "maxGradAmplP", "maxGradAmplR": if maxGradAmplP or maxGradAmplR are given maxGradAmpl is ignored
    //"rampDownTime": not yet supported, I think
    //"minRiseTime": default is 10. or the one given in the constructor
    //only the first occurence of a keyword is used
    //if no version of "maxGradAmpl" is given, a header line with two numbers is searched,
    //if this can't be found the first entry pair is used
    virtual bool interpretHeader(std::string sHeader);

    double getGradientAmplitudePhase() {return m_dAmplP;}
    double getGradientAmplitudeRead() {return m_dAmplR;}
    //if these are called after fileRead(), the values in the file
    //are overwritten without any checks or warning
    void setGradientAmplitudePhase(double dAmpl) {m_dAmplP = dAmpl;}
    void setGradientAmplitudeRead(double dAmpl) {m_dAmplR = dAmpl;}
    //only used for the completion of a gradient form not going to zero
    void setMinRiseTime(double dRT) {m_dRiseTime = dRT;}
    double getMinRiseTime() {return m_dRiseTime;}
    //void setlRampTime(long lRT) {m_lRampTime}

    //maybe: normalize with respect to highest absolute value (only if >1?)
    //       check for highest values
    
    //used to add a ramp down at the end of the gradient if needed
    void completeGradientForm(); 
    void completeGradientForm(double dMinRiseTime);
    void completeGradientForm(long lRampDownTime);
    //this additionally uses completeGradientForm
    virtual int readFile();

//future
    //bool/int/NLS_STATUS checkSpiral() -> need min rise time and max grad amplitude

private:
    //long m_lCol = 2; //don't know if this works 
    //make the access private, so no one changes it
    using FileReadBase::m_lCol;
    using FileReadBase::setNoColumns; 
    double m_dAmplP;
    double m_dAmplR;
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
    //don't have to be class members
    double extractDouble(std::string);
    long extractLong(std::string);

    const long m_GRT;
};

#endif //double include guard