//This class handles my diffusion encoding stuff
// such as getting the external diffusion directions
// calculating the b-value and b-matrix and writing
// them in the MDH (fields taken from ep2d_diff) 

//at the moment only handle Stejskal-Tanner diffusion
//encoding, but in the future generalize it with 
//myGradient class or go to references of the gradient
//objects from the sequence.

//double inclusion guard
#ifndef MY_DIFFUSION_NUMBERS 
#define MY_DIFFUSION_NUMBERS 1

#include "MrServers/MrProtSrv/MrProt/MrProt.h"                     // MrProt
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"              // sREADOUT
#include "MrServers/MrProtSrv/MrProt/MrSlice.h"                    //Slice
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSLICE_POS.h"             //sSLICE_POS
#include "MrServers/MrImaging/seq/common/myHelperClasses/myGradient.h" 
#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadExternalDiffDir.h" 
#include <vector>

class myDiffusionNumbers {
public:
//constructors: empty, filename, diffusion parameters
    myDiffusionNumbers();
    myDiffusionNumbers(std::string sName);
    myDiffusionNumbers(long lDuration, long lRampTime, long lCapitalDelta, double dAmplitude, double dGamma);
    myDiffusionNumbers(std::string sName, long lDuration, long lRampTime, long lCapitalDelta, double dAmplitude, double dGamma);
    
    long getNoDiffDir(){return m_ReadExtDir.getNoDiffDir();}
    void setFilename(std::string sNewFile);
    std::string getFilename(){ return m_ReadExtDir.getFilename();}

    bool initDirections();
    long initDirections(long anything); //for debugging slightly more informative output is helpful, so 
    virtual double calcBMatrix(long lDir, bool bDumpInfo=false); //return the b-value
    void resetBMatrix(){m_dBpp=0.; m_dBpr=0.; m_dBps=0.; m_dBrr=0.; m_dBrs=0.; m_dBss=0.;}
    void writeBMatrix(long lDir, sREADOUT& sADC,  Slice const& slice, sSLICE_POS const& sSLC, bool bDumpInfo=false);
    //if lDir is larger than the number of directions, the last one is given, but returns false
    bool getDirectionFactors(long lDir, double& dPFactor, double& dRFactor, double& dSFactor);
    bool getDirectionFactors(long lDir, double adFactors[3]);
    double getSquareRowNorm(long lDir) {return m_ReadExtDir.getSquareRowNorm(lDir);}

    //this could be used for every sequence
    //-> don't know how to do it flexible, I would need access to enum of UI
    //maybe derive a class in the sequence
    //virtual bool setGradients(const MrProt& rProt); //read parameters from UI 
    virtual void setGradients(long lDuration, long lRampTime, long lCapitalDelta, double dAmplitude, double dGamma); //this is specific for Stejskal Tanner
    void setAmplitude(double dAmpl) {m_dAmpl = dAmpl;}
    //these two are taken directly from SBBDiffusion.cpp of the sequence a_ep2d_diff
    //void MatMultTrans(double matA[3][3],double matB[3][3],double matC[3][3]);
    //void MatMult(double matA[3][3],double matB[3][3],double matC[3][3]);

    void setExternalBMatrix(double dBMat[3][3]) {
        m_dBpp=dBMat[0][0]; m_dBpr=dBMat[0][1]; m_dBps=dBMat[0][2]; 
        m_dBrr=dBMat[1][1]; m_dBrs=dBMat[1][2]; m_dBss=dBMat[2][2]; 
        m_bUseExternalMatrix = true;
    }
    void setExternalBMatrix(double bpp, double brr, double bss, double bpr, double bps, double brs) {
        m_dBpp=bpp; m_dBpr=bpr; m_dBps=bps; 
        m_dBrr=brr; m_dBrs=brs; m_dBss=bss; 
        m_bUseExternalMatrix = true;
    }
    double getBMatrix(double dBMat[3][3]) {
        dBMat[0][0]=m_dBpp; dBMat[0][1]=m_dBpr; dBMat[0][2]=m_dBps;
        dBMat[1][0]=m_dBpr; dBMat[1][1]=m_dBrr; dBMat[1][2]=m_dBrs;
        dBMat[2][0]=m_dBps; dBMat[2][1]=m_dBrs; dBMat[2][2]=m_dBss;
        return (m_dBpp + m_dBrr + m_dBss);
    }
    double getBValue() {
        return (m_dBpp + m_dBrr + m_dBss);
    }


    void prepRotMats(eRotType RotIn){m_ReadExtDir.calcRotMats(RotIn);}
    void prepRotMats(){m_ReadExtDir.calcRotMats();}
    bool getRotMat(long lDir, double adRotMat[3][3]) {return m_ReadExtDir.getRotMat(lDir, adRotMat);}
    bool isScaledGradients() {return m_ReadExtDir.isScaledGradients();}
    eRotType getRotationType(){return m_ReadExtDir.getRotationType();}
    void setRotationType(eRotType RotIn){m_ReadExtDir.setRotationType(RotIn);}
    eCoordSys getCoordinateSystem(){return m_ReadExtDir.getCoordinateSystem();}
    void setCoordinationSystem(eCoordSys CSin) {m_ReadExtDir.setCoordinationSystem(CSin);}
    
    long getDirectionsToCheck(long lNumberOfChecks, std::vector<long>& lCheckIds) {
        return m_ReadExtDir.getDirectionsToCheck(lNumberOfChecks, lCheckIds);
    }
    //future 
    //sort the gradients for starting time
    //double getLargestNorm();
    //setCoordSystem();
    //normalise direction(enum{No, individual, global} )
    //for completion in Stejskal-Tanner
    /*void setGradientAmplitude(double dAmplitude);
    void setGradientDuration(long lDuration);
    void setGradientRampTime(long lRampTime);
    void setDiffusionTime(long lCapitalDelta);*/
protected:
    std::vector<myGradient> m_vgGrads; //they hold the timing and absolute amplitude
    ReadExternalDiffDir m_ReadExtDir; //this holds the direction 
    std::vector<long> m_vlRefocTimes; //this tells the effective timepoints for refocusing pulses
    double m_dBpp, m_dBpr, m_dBps, m_dBrr, m_dBrs, m_dBss; //save B-matrix in Ph-Re-Sl
    double m_dAmpl;
//    long m_lDur;
//    long m_lRamp;
//    long m_lDiffTime;
    bool m_bUseExternalMatrix;
};

#endif
