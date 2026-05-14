#include<sstream>
//#include<string>
#include<fstream>
#include<iostream>
//#include<vector>
#include<algorithm> //for stt::max_element, transform
#include<cmath>
#include "MrServers/MrImaging/seq/common/myHelperClasses/FileReadBase.h"
#include "MrServers/MrImaging/seq/common/myHelperClasses/vxMacros.h"
// <sys/stat> doesn't exist in VB17 VXWORKS but at this point the file should exist anyways

#if (!defined(VX_SCANNER_BUILD)) || (VX_VERSION > VX_VERSION_VD)
#include <sys/stat.h> //to check the file existence 
#endif
FileReadBase::FileReadBase() {
}

FileReadBase::FileReadBase(std::string sFile) :
m_sFilename(sFile), 
m_lCol(1),
m_lLength(-1),
m_bFileReadOk(false)
{
}

FileReadBase::FileReadBase(long lCol) :
m_lCol(lCol),
m_lLength(-1),
m_bFileReadOk(false)
{
}

FileReadBase::FileReadBase(std::string sFile, long lCol) :
m_sFilename(sFile) ,
m_lCol(lCol),
m_lLength(-1),
m_bFileReadOk(false)
{
}

//destructor
FileReadBase::~FileReadBase() {
}

long FileReadBase::getFileLength() {
    long i = 0;
    if(!bCheckExistence()) {
        return -10;
    }
	std::ifstream ifsGradAmpl(m_sFilename.c_str());
    if(!ifsGradAmpl.good()) return -1;
	std::string sLine;
    //loop through the file and count lines with more than whitespace
    //discard lines that start with /(comments) or #(infos for the sequence)
	while (std::getline(ifsGradAmpl, sLine)) {
		//disgard any comment line
		if (sLine.empty() || sLine[0] == '#' || sLine[0] == '/' || sLine.find_first_not_of(" \t\n\f\v\r") == std::string::npos) continue;
		i++;
	}
	//check if the reading worked
	if (ifsGradAmpl.bad()) {
		return -3;
	}
	ifsGradAmpl.close(); //should not be needed

	return i;
}

std::string FileReadBase::getFileHeader() {
    std::string sOut;
    if (!bCheckExistence()) {
        return sOut;
    }
	std::ifstream ifsGradAmpl(m_sFilename.c_str());
	if(!(ifsGradAmpl.good())) return sOut;
	std::string sLine;
    
    while (std::getline(ifsGradAmpl, sLine)) {
        if (sLine[0] == '#') {
            //getline throws away the newline, so we have to add it again.
            //interpret header function can then use getline as well
            sOut += sLine + "\n";
        }
    }
    //remove the last newline (would not really be necessary)
    if(sOut.size()>0) sOut.erase(sOut.end()-1);
    return sOut;
}

int FileReadBase::readFile(long lLength) {
    //checks for consitency:
    //not needed as getline returns fail anyways
    //if(lLength > getFileLength()) lLength = getFileLength();
    if(!bCheckExistence()) {
        return -10;
    }
    if(lLength < 1) {
        return -11;
    }
    int iL=0, iC=0; //iterate over Lines and Columns
    float fBuf = 0.;
    //the data is written in a 1-D vector
    m_vfContents.resize(lLength*m_lCol);
    std::ifstream ifsGradAmpl(m_sFilename.c_str());
    if(!ifsGradAmpl.good()) {
        ifsGradAmpl.close();
		return -1;
    }

	std::string sLine;
    //go through the file line by line
	while (std::getline(ifsGradAmpl, sLine) && iL<lLength) {
		//skip header, comments and empty lines
		if (sLine.empty() || sLine[0] == '#' || sLine[0] == '/' || sLine.find_first_not_of(" \t\n\f\v\r") == std::string::npos) {
			continue;
		}
        //stringstream is a easy way to pass the data into the vector
		std::stringstream iss;
		iss << sLine;
        //the contents are saved columnwise, but read rowwise which 
        //leads to the need calculate the index
        for(iC=0; iC<m_lCol; ++iC) {
            if (!(iss >> fBuf)) return -2;      
            m_vfContents.at(iL + iC*lLength) = fBuf;          
        }
        ++iL;
    }
    //m_lLength = iL;
    if (ifsGradAmpl.bad()) {
        ifsGradAmpl.close();
		return -3;
    }
    
    m_bFileReadOk = true;

    //this happens if the m_lLength is limited by the user
    if (!ifsGradAmpl.eof()) { 
        ifsGradAmpl.close();
		return 1;
    }
	ifsGradAmpl.close();
	return 0;
}

//at some point it turned out to be advantegeous to have a 
//readFile() function with a length parameter, but the 
//intuitiv use would be without input, so I provided both
int FileReadBase::readFile(){
    if(!bCheckExistence()) 
        return -10;
    //if the length was not set yet
    if(m_lLength < 0) m_lLength = getFileLength();
    return readFile(m_lLength);
}

std::vector<float> FileReadBase::getColumnAsVector(long lColumnNo) {
    std::vector<float> vBuf;
    if (m_bFileReadOk && lColumnNo < m_lCol) {
        vBuf.assign(m_vfContents.begin()+lColumnNo*m_lLength, m_vfContents.begin()+(lColumnNo+1)*m_lLength);
    }
    return vBuf;
}

//fArr[] should be defined outside the function
bool FileReadBase::getColumnAsArray(long lColumnNo, float fArr[], long lLength) {
    if(!m_bFileReadOk || lLength != m_lLength) return false;
    //fArr = &m_vfContents[lColumnNo*m_lLength];
    for(int i=0; i<m_lLength; ++i) {
        fArr[i] = m_vfContents[lColumnNo*m_lLength+i];
    }
    return true;
}

//this one would be easier for vector of vectors, but I guess it's not worth it
//somwtime you might want the row, e.g. for calulating  norm
std::vector<float> FileReadBase::getRowAsVector(long lRowNo){
    std::vector<float> vBuf;
    if(m_bFileReadOk && lRowNo < m_lLength) {
        vBuf.resize(m_lCol);
        for(int i=0; i< m_lCol; ++i) {
            vBuf[i] = m_vfContents[lRowNo+i*m_lLength];
        }
    }
    return vBuf;
}

bool FileReadBase::getRowAsArray(long lRowNo, float fArr[], long lLength) {
    if(!m_bFileReadOk || lLength != m_lCol) return false;
    for(int i=0; i<lLength; ++i) {
        fArr[i] = m_vfContents[lRowNo+i*m_lLength];
    }
    return true;
}

//here it is: the calulation of the norm of a row
float FileReadBase::getSquareRowNorm(long lRowNo) {
    if(lRowNo >= m_lLength) return -1;
    float* fArr = new float[m_lCol];
    //fails if the file was not read ok
    if(!getRowAsArray(lRowNo, fArr, m_lCol)) return -2;
    float fNorm = 0.;
    for (int i=0; i<m_lCol; ++i) {
        fNorm += (fArr[i]*fArr[i]);
    }
    return sqrt(fNorm);
}

bool FileReadBase::bCheckExistence() {
   //check if a filename was set
    if (m_sFilename.empty()) 
        return false;
#if defined(VX_SCANNER_BUILD) && (VX_VERSION < VX_VERSION_VD)
    return true;
#else
    struct stat buf; 
    return (stat(m_sFilename.c_str(), &buf) != -1);
#endif
}
