//Class to read diffusion directions from an external file
//in the header you can define a coordinate system with
//   #CoordinateSystem = xyz
//otherwise it is assumed prs. This doesn't affect anything 
//in the class itself, but you can use it in the sequence.
//You can also use calculate a rotation matrix which is needed
//for free gradient waveforms. The methods are designed after
//my understanding of the description of the FWF sequence from
//Lund. 


//Lars Mueller, CUBRIC, 2019-09-01


//misses basically any error handling
//for example empty file or no dir could be read

//double include guard
#ifndef READ_EXTERNAL_DIFFDIR_H
#define READ_EXTERNAL_DIFFDIR_H

#include "MrServers/MrImaging/seq/common/myHelperClasses/FileReadBase.h"
#include <utility> //std::pair

enum eRotType {NoRot, NoRotButScaled, RotMatScaled, RotMatRot, EulerAngles};
enum eCoordSys {XYZ, PRS};
//To rotate the free waveforms we need to define a rotation matrix, 
//there are several ways to do it
//NoRot: No rotation will be performed
//RotMatScaled: rotates vector [1, 0, 0] to target vector and 
//  scales by the target vector norm. (Scaling needs to happen in the sequence)
//  problem: no rotation around [1,0,0]
//RotMatRot: The rotation matrix rotates around the axis of the given vector.
//  The rotation angle is the norm of the vector multiplied by 2*pi
//  problem: no scaling of the gradient amplitudes
//EulerAngles: The three values(*2*pi) are taken as Euler angles
//  taken from mathworld.wolfram.com/EulerAngles.html from the 
//  pitch-roll-yaw convention (x-y-z), adVec = (phi, theta, psi)
//  problem: no scaling of gradient amplitude
//so far only RotMatScaled was tested

class ReadExternalDiffDir : public FileReadBase {
public: 
    //constructor
    ReadExternalDiffDir();
    ReadExternalDiffDir(std::string sFile);
    //destructor 
    ~ReadExternalDiffDir();
    
    std::vector<double> getPhaseFactor();
    std::vector<double> getReadFactor();
    std::vector<double> getSliceFactor();
    long getNoDiffDir(){return m_lNoDiffDir;} 
    //to maybe do one day a normalization or at least 
    //minimal error handling. 
    //double getLargestNorm();
    double fillNormVector();
    
    virtual int readFile();
    virtual int readFile(long lLength);
    virtual bool interpretHeader(std::string sHeader);

    //for free gradient waveforms we need rotation matrices
    void calcRotMats(eRotType Rot);
    void calcRotMats();
    bool getRotMat(long lDir, double adRotMat[3][3]);
    bool isScaledGradients() {return (m_lRotType==NoRotButScaled) || (m_lRotType==RotMatScaled);}
    
    eRotType getRotationType(){return m_lRotType;}
    void setRotationType(eRotType RotIn){m_lRotType = RotIn; m_bRotMatsPreppped=false;}
    eCoordSys getCoordinateSystem(){return m_lCoordSys;}
    void setCoordinationSystem(eCoordSys CSin) {m_lCoordSys = CSin;}

    //excessive checking might lead to problems, e.g. in dMRF
    //these functions give indices of diffusion directions with the one lowest norm and 
    //lNumberOfChecks-1 highest norms in the second argument. If lNumberOfChecks is larger
    //than m_lNoDiffDir only m_lNoDiffDir will be given. The return value is the actual
    //number of checking indices which is not larger than lNumberOfChecks. No guarantee
    //that the array version works and does not create any wild pointers.
    //long getDirectionsToCheck(long lNumberOfChecks, long lCheckIds[]);
    long getDirectionsToCheck(long lNumberOfChecks, std::vector<long>& lCheckIds);

    //Future possibilities
    //Normalization
protected:
    //rotate vector [1, 0, 0] to adVec and return the L2-norm of adVec 
    double calcRotMat_RotMatScaled(double adVec[3], double adRotMat[3][3]);
    //rotate around direction of adVec with an angle ||adVec||*2*pi
    double calcRotMat_VecAngle(double adVec[3], double adRotMat[3][3]);
    //assume the elements of the vector are three euler angles:
    //rotation matrix around axis u = R_u
    //use with extreme care
    double calcRotMat_EulerAngles(double adVec[3], double adRotMat[3][3]);
    
    long m_lNoDiffDir;
    std::vector<double> m_vdRotMat;
    //this should allow us to sort the norms and remember the indices for check functions
    std::vector<std::pair<double,long> > m_vpDiffNorm; 
    eRotType m_lRotType;
    eCoordSys m_lCoordSys;
    bool m_bRotMatsPreppped;
    
    //double RotMat[3][3];
    //for future implementations
    //void setLargestNorm(); 
    //coordSys(PRS, XYZ)
    //slice array for rotations
    double m_dLargestNorm; 
    
    
private:
    using FileReadBase::m_lCol;
    using FileReadBase::setNoColumns;   
};

#endif //double include guard