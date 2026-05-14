#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadExternalSpiral.h"
#include<sstream>
//#include<string>
#include<fstream>
#include<iostream>
//#include<vector>
#include<algorithm> //for stt::max_element, transform
#include<cmath>

#ifndef GRT_FOR_SPIRAL_READ
#define GRT_FOR_SPIRAL_READ 10
#endif
//make sure to fix number of columns and m_GRT
ReadExternalSpiral::ReadExternalSpiral() :
FileReadBase(2),
m_dRiseTime(10.),
m_dAmplP(0.),
m_dAmplR(0.),
m_GRT(GRT_FOR_SPIRAL_READ)
{}

ReadExternalSpiral::ReadExternalSpiral(std::string sFile) :
FileReadBase(sFile, 2),
m_dRiseTime(10.), 
m_dAmplP(0.),
m_dAmplR(0.),
m_GRT(GRT_FOR_SPIRAL_READ)
{}

ReadExternalSpiral::ReadExternalSpiral(std::string sFile, double dRiseTime) :
FileReadBase(sFile, 2),
m_dRiseTime(dRiseTime),
m_dAmplP(0.),
m_dAmplR(0.),
m_GRT(GRT_FOR_SPIRAL_READ)
{}

//destructor 
ReadExternalSpiral::~ReadExternalSpiral() {}


std::vector<float> ReadExternalSpiral::getPhaseGradient() {
    return vfGradP;
}

std::vector<float> ReadExternalSpiral::getReadGradient() {
    return vfGradR;
}


int ReadExternalSpiral::readFile() {
    if(!FileReadBase::bCheckExistence()) {
        return -10;
    }    
    //in case we allow the header file to limit the gradient duration
    //interpret Header returns true if it could identify a header with amplitude info.
    //is this is not the case, the first value in the vectors is assumed to be the amplitude
    bool useFirstValueAsAmplitude = !interpretHeader(FileReadBase::getFileHeader());
    //the +1 is for the case that the first entry is to be taken as amplitude and 
    //the length should nevertheless be limmited
    if(m_lLength < 1) m_lLength = FileReadBase::getFileLength();
    if(m_lLength < 1) return -9;
    //+1 in case the first value
    long lReadingLength = m_lLength;
    if (useFirstValueAsAmplitude) ++lReadingLength;
    //could happen for an empty file
    if(lReadingLength < 1) return -5;
    //You can't read more than there is
    if(lReadingLength > FileReadBase::getFileLength()) 
        lReadingLength = FileReadBase::getFileLength();
    int iRet = FileReadBase::readFile(lReadingLength);
    if(iRet< 0) return iRet; //if there is an error we can already return here
    
    //fill in the vectors for read nd phase gradients,
    //they are used in case they need to be completed
    vfGradP = FileReadBase::getColumnAsVector(0);
    vfGradR = FileReadBase::getColumnAsVector(1);
    //if the header does not set the gradient amplitude, we take
    //the first value of the vector
    if (useFirstValueAsAmplitude && vfGradP.size()>0 && vfGradR.size()>0) {
        //if, God forbid, the value should be negativ (because you forget to set it and the trajectory starts with a negativ?)
        //I guess it might be easier to debug with the error of the gradient prepare method, so I let it slide here
        m_dAmplP = vfGradP[0];
        m_dAmplR = vfGradR[0];
        vfGradP.erase(vfGradP.begin());
        vfGradR.erase(vfGradR.begin());
        //--m_lLength;
    }
    //no normalization, as the vector might intentionally be set to smaller than one
    completeGradientForm();
    m_lActualLength = vfGradP.size();
    return iRet;
}

//this and its sibling extractLong are used for interpreting the header
//They take a string (e.g. header since the last keyword) and return 
//the first number they might find. Since all keywords require positive
//values, -1 is used to indicate no number was found
double ReadExternalSpiral::extractDouble(std::string sSubHeader) {
    std::size_t pos1;
    double dRet = -1.;
    pos1 = sSubHeader.find_first_of("0123456789");
    if (pos1 != std::string::npos) {
        if( (pos1>1) && (sSubHeader[pos1-1]=='-') ) --pos1;
        std::size_t pos2 = sSubHeader.find_first_of("#\n", pos1);
        std::stringstream iss;
        iss << sSubHeader.substr(pos1,pos2-pos1);
        if(!(iss >> dRet)) return -2;
    }
    return dRet;
}

long ReadExternalSpiral::extractLong(std::string sSubHeader) {
    std::size_t pos1;
    long lRet = -1;
    pos1 = sSubHeader.find_first_of("0123456789");
    if (pos1 != std::string::npos) {
        if( (pos1>1) && (sSubHeader[pos1-1]=='-') ) --pos1;
        std::size_t pos2 = sSubHeader.find_first_of("#\n", pos1);
        std::stringstream iss;
        iss << sSubHeader.substr(pos1,pos2-pos1);
        if(!(iss >> lRet)) return -2;
    }
    return lRet;
}

bool ReadExternalSpiral::interpretHeader(std::string sHeader) {
    bool isAmplitudeSet = false;
    if(sHeader.empty()) return false;
    std::size_t pos1; //for position of key word
    std::size_t pos2; //for position of number after keyword
    std::stringstream iss1; //for passing string to class values, in case of no keyword 
    double dBuf1 = 0., dBuf2 = 0.; //buffers to hold numbers
//check if any keywords were used
    //search for gradient amplitude
    //flags for keeping track of things
    bool isPhaseSet = false;
    bool isReadSet = false;
    pos1 = sHeader.find("maxGradAmpl");
    //if this keyword is found, check if P and R are declared separately
    if(pos1 != std::string::npos) {
        pos2 = sHeader.find("maxGradAmplP");
        if(pos2 != std::string::npos) {
            //maxGradAmplP was found, so check for a positive number and write it to member
            dBuf1 = extractDouble(sHeader.substr(pos2));
            if(dBuf1>=0) {
                m_dAmplP = dBuf1;
                isPhaseSet = true;
            }
        }
        pos2 = sHeader.find("maxGradAmplR");
        if(pos2 != std::string::npos) {
            //maxGradAmpl was found, so check for a positive number and write it to member
            dBuf1 = extractDouble(sHeader.substr(pos2));
            if(dBuf1>=0) {
                m_dAmplR = dBuf1;
                isReadSet = true;
            }
        }
        //check if only one amplitude is given
        if(!isReadSet && !isPhaseSet) {
            pos2 = sHeader.find("0123456789", pos1);
            dBuf1 = extractDouble(sHeader.substr(pos1));
            if(dBuf1>=0) {
                m_dAmplR = dBuf1;
                isReadSet = true;
                m_dAmplP = dBuf1;
                isPhaseSet = true;
            }
        } else if(isReadSet && !isPhaseSet) { //if only one was set, set the other to the same value
            m_dAmplP = m_dAmplR;
            isPhaseSet = true;
        } else if(!isReadSet && isPhaseSet) {
            m_dAmplR = m_dAmplP;
            isReadSet = true;
        }
    } else {//if sHeader.find("maxGradAmpl") != std::string::npos
        //look for a line without keywords
        iss1 << sHeader;
        std::string sLine;
        //double dBuf1 = 0, dBuf2=0;
        while(std::getline(iss1, sLine)) {
            //look for any letter, which leads to not considering the line
            pos1 = sLine.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
            if(pos1 != std::string::npos) {
                continue;
            }
            //check if there is at least one number in the line
            pos1 = sLine.find_first_of("0123456789");
            if (pos1 == std::string::npos) {
                continue;
            }
            if( (pos1>1) && (sLine[pos1-1]=='-') ) --pos1;
            std::stringstream iss2;
            iss2 << sLine.substr(pos1);
            if(iss2 >> dBuf1 >> dBuf2) {
                m_dAmplP = dBuf1;
                m_dAmplR = dBuf2;
                isReadSet = true;
                isPhaseSet = true;
                break;
            }
        }
    } //if sHeader.find("maxGradAmpl") != std::string::npos {} else
    isAmplitudeSet = (isReadSet && isPhaseSet);            
    long lBuf = 0;
    pos1 = sHeader.find("rampDownTime");
    if(pos1 != std::string::npos){
        lBuf = extractLong(sHeader.substr(pos1));
        if(lBuf >= 0) m_lRampTime = lBuf;
    }
    pos1 = sHeader.find("minRiseTime");
    if(pos1 != std::string::npos){
        dBuf1 = extractDouble(sHeader.substr(pos1));
        if(dBuf1 >= 0) m_dRiseTime = dBuf1;
    }
    
    return isAmplitudeSet;
} //ReadExternalSpiral::interpretHeader

void ReadExternalSpiral::completeGradientForm() {
    //maybe check for consistency with m_lRampTime
    ReadExternalSpiral::completeGradientForm(m_dRiseTime);
}

void ReadExternalSpiral::completeGradientForm(long lRampTime) {
    //maybe use .at() for size checks, maybe not needed (have to check)
    double dFinalAmpl = std::max(vfGradP[m_lLength-1],vfGradR[m_lLength-1]);
    //dMinRiseTime = 1.0*lRampTime/dFinalAmpl;
    ReadExternalSpiral::completeGradientForm(lRampTime/dFinalAmpl);
}

void ReadExternalSpiral::completeGradientForm(double dMinRiseTime) {
    if(vfGradR.back() != 0 || vfGradP.back() != 0) {
        //the vectors only hold a relative amplitude, so don't forget to multiply with the actual
        //replace std::ceil with fSDSRoundUpGRT
        long lNeedRampDownTimeP = ceil(fabs(vfGradP.back()*m_dAmplP)*dMinRiseTime/m_GRT);
        long lNeedRampDownTimeR = ceil(fabs(vfGradR.back()*m_dAmplR)*dMinRiseTime/m_GRT);
		long lMaxLengthNew = vfGradP.size() + std::max(lNeedRampDownTimeP,lNeedRampDownTimeR);
        //now fill the new points
		//it may happen that only one gradient is non-zero:
		//later a check is done if the step size is smaller than the residual gradient, which leads to an reduction, 
		//otherwise the following values are all set to zero
		float fStepSizeP = 1.0; 
		if(lNeedRampDownTimeP > 0) {
            fStepSizeP = vfGradP.back()/lNeedRampDownTimeP;
		}
		float fStepSizeR = 1.0;
		if(lNeedRampDownTimeR > 0) {
            fStepSizeR = vfGradR.back()/lNeedRampDownTimeR;
		}
        //std::ofstream ofs;
        //ofs.open(m_sFilename.c_str(), std::ios::app);
        //ofs << "\n\n//---ReadExternalSpiral completed the trajectory---//" << std::endl;
                         //This is the number of additional steps needed
        for(long lI=vfGradP.size(); lI< lMaxLengthNew; lI++){
			//if stepsize is smaller than the gradient amplitude substract it, otherwise write zero.
            if(fStepSizeP*fStepSizeP < vfGradP[lI-1]*vfGradP[lI-1]) {
                vfGradP.push_back(vfGradP[lI-1]-fStepSizeP);
			} else {
                vfGradP.push_back(0);
			}
            if(fStepSizeR*fStepSizeR < vfGradR[lI-1]*vfGradR[lI-1]) {
                vfGradR.push_back(vfGradR[lI-1]-fStepSizeR);
			} else {
                vfGradR.push_back(0);
			}           
            //ofs << "// " << vfGradP.back() << " " << vfGradR.back() << std::endl;
		} //for lI -> setting the ramp down 
        //ofs.close();
        m_lActualLength = vfGradP.size();
    }
}