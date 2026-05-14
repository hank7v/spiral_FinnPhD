#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadExternalDiffDir.h"
#include "MrServers/MrImaging/seq/common/myHelperClasses/helpfulFunctions.h"
#include <algorithm>
#include <cmath>

ReadExternalDiffDir::ReadExternalDiffDir():
FileReadBase(3),
m_lNoDiffDir(0),
m_lRotType(RotMatScaled),
m_bRotMatsPreppped(false),
m_lCoordSys(PRS)
{}

ReadExternalDiffDir::ReadExternalDiffDir(std::string sFile):
FileReadBase(sFile, 3),
m_lNoDiffDir(0),
m_lRotType(RotMatScaled),
m_bRotMatsPreppped(false),
m_lCoordSys(PRS)
{}
  
//destructor 
ReadExternalDiffDir::~ReadExternalDiffDir()
{}

int ReadExternalDiffDir::readFile() {
    return readFile(FileReadBase::getLength());
}

int ReadExternalDiffDir::readFile(long lLength) {
    if(!interpretHeader(FileReadBase::getFileHeader())) {
        return -100;
    }
    int iRet = FileReadBase::readFile(lLength);
    m_lNoDiffDir = lLength;//FileReadBase::getLength();
    //m_dLargestNorm = getLargestNorm();
    m_dLargestNorm = fillNormVector();
    return iRet;
}

// I use doubles for diffusion direction, but the base class was
// designed with arbitrary gradients in mind which uses float
std::vector<double> ReadExternalDiffDir::getPhaseFactor() {
    std::vector<float> fbuf = FileReadBase::getColumnAsVector(0);
    std::vector<double> dbuf(fbuf.begin(),fbuf.end());
    return dbuf;
}

std::vector<double> ReadExternalDiffDir::getReadFactor() {
    std::vector<float> fbuf = FileReadBase::getColumnAsVector(1);
    std::vector<double> dbuf(fbuf.begin(),fbuf.end());
    return dbuf;
}

std::vector<double> ReadExternalDiffDir::getSliceFactor() {
    std::vector<float> fbuf = FileReadBase::getColumnAsVector(2);
    std::vector<double> dbuf(fbuf.begin(),fbuf.end());
    return dbuf;
}

/* 
double ReadExternalDiffDir::getLargestNorm(){
    double dLargeNorm = 0.;
    for(int i=0; i<getLength(); ++i) {
        dLargeNorm = std::max<double>(dLargeNorm, FileReadBase::getSquareRowNorm(i));
    }
    return dLargeNorm;
}
*/
double ReadExternalDiffDir::fillNormVector(){
    m_vpDiffNorm.clear();
    for(long i=0; i<m_lNoDiffDir; ++i) {
        //fill up the vector of pairs with the diffusion vector norms and indices
        m_vpDiffNorm.push_back(std::make_pair(FileReadBase::getSquareRowNorm(i),i));
    }
    if(m_lNoDiffDir < 1) {//shouldn't happen, but you never know
        m_vpDiffNorm.push_back(std::make_pair(0.,0));
    }
    std::sort(m_vpDiffNorm.begin(),m_vpDiffNorm.end());
    if(m_vpDiffNorm[0].first < 0) return -1;
    return m_vpDiffNorm[0].first;
}

/*long ReadExternalDiffDir::getDirectionsToCheck(long lNumberOfChecks, long lCheckIds[]){
    std::vector<long> vlCheckIds;
    lNumberOfChecks = getDirectionsToCheck(lNumberOfChecks, vlCheckIds);
    if(lNumberOfChecks <= 0) {
        lCheckIds = NULL;
    } else {
        lCheckIds = &vlCheckIds[0];
    }
    return lNumberOfChecks;
}*/

long ReadExternalDiffDir::getDirectionsToCheck(long lNumberOfChecks, std::vector<long>& lCheckIds){
    if(lNumberOfChecks > m_lNoDiffDir) {
        lNumberOfChecks = m_lNoDiffDir;
    }
    if(m_vpDiffNorm.size() < m_lNoDiffDir) { //this should only happen if you haven't read the file yet
        return -1;
    }
    lCheckIds.clear();
    if(lNumberOfChecks > 0) {
        lCheckIds.push_back(m_vpDiffNorm[0].second);
        for(long lI=m_vpDiffNorm.size()-1; lI>(m_vpDiffNorm.size()-lNumberOfChecks); --lI) {
            lCheckIds.push_back(m_vpDiffNorm[lI].second);
        }
        std::sort(lCheckIds.begin(),lCheckIds.end());
    }
    return lNumberOfChecks;
}

//functions used for determining a rotation matrix
double ReadExternalDiffDir::calcRotMat_RotMatScaled(double adVec[3], double adRotMat[3][3]) {
    double adInitVec[3] = {1., 0., 0.};
    double adTargVec[3] = {adVec[0], adVec[1], adVec[2]};
    double adId[3][3] = {{1.,0.,0.}, {0.,1.,0.}, {0.,0.,1.}}; 
    
    double adCV[3];
    double dNorm = vecNormalize(adTargVec);
    double cosAngle = dotProduct(adInitVec, adTargVec);
    crossProduct(adInitVec, adTargVec, adCV);
    //double sinAngle = vecNorm(adCV);
    
    //
    if(cosAngle >0.99999 || dNorm<0.000001) {
        for(int r=0; r<3; ++r) {
            for(int c=0; c<3; ++c) {
                adRotMat[r][c] = adId[r][c];
            }
        }
        return dNorm;
    }
    if(cosAngle < -0.99999) {
        adRotMat[0][0] = -1; adRotMat[0][1] =  0; adRotMat[0][2] = 0;
        adRotMat[1][0] =  0; adRotMat[1][1] = -1; adRotMat[1][2] = 0;
        adRotMat[2][0] =  0; adRotMat[2][1] =  0; adRotMat[2][2] = 1;
        return dNorm;
    }
       
//math taken from https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
    double adVM[3][3];
    adVM[0][0] = 0;        adVM[0][1] = -adCV[2]; adVM[0][2] = adCV[1];
    adVM[1][0] = adCV[2];  adVM[1][1] = 0;        adVM[1][2] = -adCV[0];
    adVM[2][0] = -adCV[1]; adVM[2][1] = adCV[0];  adVM[2][2] = 0;

    double adVM2[3][3];
    MatMult(adVM, adVM, adVM2);
    for(int r=0; r<3; ++r) {
        for(int c=0; c<3; ++c){
            adVM2[r][c] /= (1+cosAngle);
        }
    }
    
    MatAdd(adId, adVM, adRotMat);
    MatAdd(adRotMat, adVM2, adRotMat);

    return dNorm;
}

double ReadExternalDiffDir::calcRotMat_VecAngle(double adVec[3], double adRotMat[3][3]) {
    double u[3] = {adVec[0], adVec[1], adVec[2]}; //rotation axis
    double dNorm = vecNormalize(u);
    double dRotAngle = 2*3.14159265359*dNorm;
    double c = cos(dRotAngle);
    double s = sin(dRotAngle);
        //taken from en.wikipedia.org/wiki/rotation_matrix
    adRotMat[0][0]=c+u[0]*u[0]*(1-c);      adRotMat[0][1]=u[0]*u[1]*(1-c)-u[2]*s; adRotMat[0][2]=u[0]*u[2]*(1-c)+u[1]*s;
    adRotMat[1][0]=u[0]*u[1]*(1-c)+u[2]*s; adRotMat[1][1]=c+u[1]*u[1]*(1-c);      adRotMat[1][2]=u[1]*u[2]*(1-c)-u[1]*s;
    adRotMat[2][0]=u[0]*u[2]*(1-c)-u[1]*s; adRotMat[2][1]=u[1]*u[2]*(1-c)+u[1]*s; adRotMat[2][2]=c+u[2]*u[2]*(1-c);
    return dNorm;
}

double ReadExternalDiffDir::calcRotMat_EulerAngles(double adVec[3], double adRotMat[3][3]){
    double cPhi = cos(2*3.14159265359*adVec[0]), sPhi = sin(2*3.14159265359*adVec[0]);
    double cThe = cos(2*3.14159265359*adVec[1]), sThe = sin(2*3.14159265359*adVec[1]);
    double cPsi = cos(2*3.14159265359*adVec[2]), sPsi = sin(2*3.14159265359*adVec[2]);
    //taken from mathworld.wolfram.com/EulerAngles.html from the pitch-roll-yaw convention (x-y-z)
    //adVec = (phi, theta, psi)
    adRotMat[0][0]=cThe*cPhi;                adRotMat[0][1]=cThe*sPhi;                adRotMat[0][2]=-sThe;
    adRotMat[1][0]=sPsi*sThe*cPhi-cPsi*sPhi; adRotMat[1][1]=sPsi*sThe*sPhi+cPsi*cThe; adRotMat[1][2]=cThe*sPsi;
    adRotMat[2][0]=cPsi*sThe*cPhi+sPsi*sPhi; adRotMat[2][1]=cPsi*sThe*sPhi-sPsi*cPhi; adRotMat[2][2]=cThe*cPsi;
    
    return sqrt(adVec[0]*adVec[0]+adVec[1]*adVec[1]+adVec[2]*adVec[2]);
}

void ReadExternalDiffDir::calcRotMats(eRotType RotIn = RotMatScaled) {
    //if(m_bRotMatsPrepped) return;
    m_vdRotMat.resize(9*m_lNoDiffDir);
    double adRotMat[3][3] = {{1.,0.,0.},{0.,1.,0.},{0.,0.,1.}};
    float fTargetVec[3];
    double dTargetVec[3];
    long lI,lJ;
    for(long lDir=0; lDir<m_lNoDiffDir; ++lDir) {
        if(!getRowAsArray(lDir, fTargetVec, 3));
        dTargetVec[0] = fTargetVec[0]; dTargetVec[1] = fTargetVec[1]; dTargetVec[2] = fTargetVec[2];
        switch(RotIn) {
            case NoRot:
                //std::cout << "------ External Diff: no rotation ---------" << std::endl;
                break;
            case RotMatScaled:
                //std::cout << "------ External Diff: rotation matrix scaled ---------" << std::endl;
                calcRotMat_RotMatScaled(dTargetVec, adRotMat);
                break;
            case RotMatRot:
                //std::cout << "------ External Diff: rotation around axis by angle ---------" << std::endl;
                calcRotMat_VecAngle(dTargetVec, adRotMat);
                break;
            case EulerAngles:
                //std::cout << "------ External Diff: euler angles ---------" << std::endl;
                calcRotMat_EulerAngles(dTargetVec, adRotMat);
                break;
        };
        //std::cout << "------ External Diff: lDir = " << lDir << "-----------"<< std::endl;
        //printMatrix(adRotMat, "External Diff rot mat");
        for (lI=0; lI<3; ++lI) {
            for(lJ=0; lJ<3; ++lJ) {
                m_vdRotMat[9*lDir + 3*lI + lJ] = adRotMat[lI][lJ];
            }
        }
    }
    m_bRotMatsPreppped = true;
}

void ReadExternalDiffDir::calcRotMats() {
    calcRotMats(m_lRotType);
}

bool ReadExternalDiffDir::getRotMat(long lDir, double adRotMat[3][3]){
    if(lDir >= m_lNoDiffDir || !m_bRotMatsPreppped) {
        return false;
    }
    for (long lI=0; lI<3; ++lI) {
        for(long lJ=0; lJ<3; ++lJ) {
             adRotMat[lI][lJ] = m_vdRotMat[9*lDir + 3*lI + lJ];
        }
    }
    return true;
}

//return true if <<CoordinateSystem = xyz>> is somewhere in the header (case sensitive) 
//assume if this is true we don't want rotation therefore set m_lRotType = NoRot;
bool ReadExternalDiffDir::interpretHeader(std::string sHeader) {
    //CoordinateSystem = xyz
    std::size_t pos1; //for position of key word
    std::size_t pos2; //for position of number after keyword
    pos1 = sHeader.find("CoordinateSystem");
    if(pos1 != std::string::npos) {
        pos2 = sHeader.find("xyz");
        if(pos2 != std::string::npos) {
            m_lCoordSys = XYZ;
            m_lRotType = NoRot;
        } else {
            m_lCoordSys = PRS;
        }
    }
    return true;

}