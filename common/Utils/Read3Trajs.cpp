#include "MrServers/MrImaging/seq/common/myHelperClasses/Read3Trajs.h"
#include<sstream>
//#include<string>
#include<fstream>
#include<iostream>
//#include<vector>
#include<algorithm> //for stt::max_element, transform
#include<cmath>

#ifndef GRT_FOR_3TRAJS_READ
#define GRT_FOR_3TRAJS_READ 10
#endif
//make sure to fix number of columns and m_GRT
Read3Trajs::Read3Trajs() :
FileReadBase(3),
m_dRiseTime(25.),
m_dAmplP(0.),
m_dAmplR(0.),
m_dAmplS(0.),
m_GRT(GRT_FOR_3TRAJS_READ)
{}

Read3Trajs::Read3Trajs(std::string sFile) :
FileReadBase(sFile, 3),
m_dRiseTime(25.), 
m_dAmplP(0.),
m_dAmplR(0.),
m_dAmplS(0.),
m_GRT(GRT_FOR_3TRAJS_READ)
{}

Read3Trajs::Read3Trajs(std::string sFile, double dRiseTime) :
FileReadBase(sFile, 3),
m_dRiseTime(dRiseTime),
m_dAmplP(0.),
m_dAmplR(0.),
m_dAmplS(0.),
m_GRT(GRT_FOR_3TRAJS_READ)
{}

//destructor 
Read3Trajs::~Read3Trajs() {}


std::vector<float> Read3Trajs::getPhaseGradient() {
    return vfGradP;
}

std::vector<float> Read3Trajs::getReadGradient() {
    return vfGradR;
}

std::vector<float> Read3Trajs::getSliceGradient() {
    return vfGradS;
}

int Read3Trajs::readFile(bool bCompleteGradientForm, bool bNeverUseFirstValueAsAmplitude) {
    //in case we allow the header file to limit the gradient duration
    //interpret Header returns true if it could identify a header with amplitude info.
    //is this is not the case, the first value in the vectors is assumed to be the amplitude
    bool useFirstValueAsAmplitude = !interpretHeader(FileReadBase::getFileHeader());
    if (bNeverUseFirstValueAsAmplitude) {
        useFirstValueAsAmplitude = false;
    }
    //the +1 is for the case that the first entry is to be taken as amplitude and 
    //the length should nevertheless be limmited
    if(m_lLength < 1) m_lLength = FileReadBase::getFileLength();
    //+1 in case the first value 
    long lReadingLength = m_lLength;
    if (useFirstValueAsAmplitude) ++lReadingLength;
    //could happen for an empty file
    if(lReadingLength < 1) 
        return -5;
    //You can't read more than there is
    if(lReadingLength > FileReadBase::getFileLength()) 
        lReadingLength = FileReadBase::getFileLength();
    int iRet = FileReadBase::readFile(lReadingLength);
    if(iRet< 0) return iRet; //if there is an error we can already return here
    
    //fill in the vectors for read nd phase gradients,
    //they are used in case they need to be completed
    vfGradP = FileReadBase::getColumnAsVector(0);
    vfGradR = FileReadBase::getColumnAsVector(1);
    vfGradS = FileReadBase::getColumnAsVector(2);
    //if the header does not set the gradient amplitude, we take
    //the first value of the vector
    if (useFirstValueAsAmplitude && vfGradP.size()>0 && vfGradR.size()>0 && vfGradS.size()>0) {
        //if, God forbid, the value should be negativ (because you forget to set it and the trajectory starts with a negativ?)
        //I guess it might be easier to debug with the error of the gradient prepare method, so I let it slide here
        m_dAmplP = vfGradP[0];
        m_dAmplR = vfGradR[0];
        m_dAmplS = vfGradR[0];
        vfGradP.erase(vfGradP.begin());
        vfGradR.erase(vfGradR.begin());
        vfGradR.erase(vfGradS.begin());
        //--m_lLength;
    }
    //no normalization, as the vector might intentionally be set to smaller than one
    if(bCompleteGradientForm) {
        completeGradientForm();
    }
    m_lActualLength = vfGradP.size();
    return iRet;
}


//this and its sibling extractLong are used for interpreting the header
//They take a string (e.g. header since the last keyword) and return 
//the first number they might find. Since all keywords require positive
//values, -1 is used to indicate no number was found
double Read3Trajs::extractDouble(std::string sSubHeader) {
    std::size_t pos1;
    double dRet = -1.;
    pos1 = sSubHeader.find_first_of("0123456789");
    if (pos1 != std::string::npos) {
        std::size_t pos2 = sSubHeader.find_first_of("#\r\n", pos1);
        std::stringstream iss;
        iss << sSubHeader.substr(pos1,pos2-pos1);
        if(!(iss >> dRet)) return -2;

    }
    return dRet;
}

long Read3Trajs::extractLong(std::string sSubHeader) {
    std::size_t pos1;
    long lRet = -1;
    pos1 = sSubHeader.find_first_of("0123456789");
    if (pos1 != std::string::npos) {
        std::size_t pos2 = sSubHeader.find_first_of("#\r\n", pos1);
        std::stringstream iss;
        iss << sSubHeader.substr(pos1,pos2-pos1);
        if(!(iss >> lRet)) return -2;
    }
    return lRet;
}

bool Read3Trajs::interpretHeader(std::string sHeader) {
    bool isAmplitudeSet = false;
    std::size_t pos1; //for position of key word
    std::size_t pos2; //for position of number after keyword
    std::stringstream iss1; //for passing string to class values, in case of no keyword 
    double dBuf1 = 0., dBuf2 = 0., dBuf3 = 0.;//buffers to hold numbers
//check if any keywords were used
    //search for gradient amplitude
    //flags for keeping track of things
    bool isPhaseSet = false;
    bool isReadSet = false;
    bool isSliceSet = false;
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
        pos2 = sHeader.find("maxGradAmplS");
        if(pos2 != std::string::npos) {
            //maxGradAmpl was found, so check for a positive number and write it to member
            dBuf1 = extractDouble(sHeader.substr(pos2));
            if(dBuf1>=0) {
                m_dAmplS = dBuf1;
                isSliceSet = true;
            }
        }
        //check if only one amplitude is given
        if(!isReadSet && !isPhaseSet && !isSliceSet) {
            pos2 = sHeader.find("0123456789", pos1);
            dBuf1 = extractDouble(sHeader.substr(pos1));
            if(dBuf1>=0) {
                m_dAmplR = dBuf1;
                isReadSet = true;
                m_dAmplP = dBuf1;
                isPhaseSet = true;
                m_dAmplS = dBuf1;
                isSliceSet = true;
            }
        } else if(isReadSet && !isPhaseSet && !isSliceSet) { //if only one was set, set the other to the same value
            m_dAmplP = m_dAmplR;
            isPhaseSet = true;
            m_dAmplS = m_dAmplR;
            isSliceSet = true;
        } else if(!isReadSet && isPhaseSet && !isSliceSet) {
            m_dAmplR = m_dAmplP;
            isReadSet = true;
            m_dAmplS = m_dAmplP;
            isSliceSet = true;
        } else if(!isReadSet && !isPhaseSet && isSliceSet) {
            m_dAmplR = m_dAmplS;
            isReadSet = true;
            m_dAmplP = m_dAmplS;
            isPhaseSet = true;
        } else {//if two are set I still have to think about what to do
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
            std::stringstream iss2;
            iss2 << sLine.substr(pos1);
            if(iss2 >> dBuf1 >> dBuf2 >> dBuf3) {
                m_dAmplP = dBuf1;
                m_dAmplR = dBuf2;
                m_dAmplS = dBuf3;
                isReadSet = true;
                isPhaseSet = true;
                isSliceSet = true;
                break;
            }
        }
    } //if sHeader.find("maxGradAmpl") != std::string::npos {} else
    isAmplitudeSet = (isReadSet && isPhaseSet && isSliceSet);            
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

void Read3Trajs::completeGradientForm() {
    //maybe check for consistency with m_lRampTime
    Read3Trajs::completeGradientForm(m_dRiseTime);
}

void Read3Trajs::completeGradientForm(long lRampTime) {
    //maybe use .at() for size checks, maybe not needed (have to check)
    double dFinalAmpl = std::max(vfGradP[m_lLength-1],std::max(vfGradR[m_lLength-1],vfGradS[m_lLength-1]));
    //dMinRiseTime = 1.0*lRampTime/dFinalAmpl;
    Read3Trajs::completeGradientForm(lRampTime/dFinalAmpl);
}

void Read3Trajs::completeGradientForm(double dMinRiseTime) {
    if(vfGradR.back() != 0 || vfGradP.back() != 0 || vfGradS.back() != 0) {
        //the vectors only hold a relative amplitude, so don't forget to multiply with the actual
        //replace std::ceil with fSDSRoundUpGRT
        long lNeedRampDownTimeP = ceil(vfGradP.back()*sgn(vfGradP.back())*m_dAmplP*dMinRiseTime/m_GRT);
        long lNeedRampDownTimeR = ceil(vfGradR.back()*sgn(vfGradR.back())*m_dAmplR*dMinRiseTime/m_GRT);
        long lNeedRampDownTimeS = ceil(vfGradS.back()*sgn(vfGradS.back())*m_dAmplS*dMinRiseTime/m_GRT);
        long lMaxLengthNew = vfGradP.size() + std::max(lNeedRampDownTimeP,std::max(lNeedRampDownTimeR,lNeedRampDownTimeS));

        //now fill the new points
		//it may happen that only one gradient is non-zero:
		//later a check is done if the step size is smaller than the residual gradient, which leads to an reduction, 
		//otherwise the following values are all set to zero
		float fStepSizeP = m_dAmplP; 
		if(lNeedRampDownTimeP > 0) {
            fStepSizeP = vfGradP.back()/lNeedRampDownTimeP;
		}
		float fStepSizeR = m_dAmplR;
		if(lNeedRampDownTimeR > 0) {
            fStepSizeR = vfGradR.back()/lNeedRampDownTimeR;
		}
        float fStepSizeS = m_dAmplS;
		if(lNeedRampDownTimeS > 0) {
            fStepSizeS = vfGradS.back()/lNeedRampDownTimeS;
		}
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
            if(fStepSizeS*fStepSizeS < vfGradS[lI-1]*vfGradS[lI-1]) {
                vfGradS.push_back(vfGradS[lI-1]-fStepSizeS);
			} else {
                vfGradS.push_back(0);
			}
		} //for lI -> setting the ramp down 
        m_lActualLength = vfGradP.size();
    }
}