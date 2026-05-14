//inputAmpl: gradient amplitudes in equidistant steps. 
//lAvailableTime: time in us that is used for the gradient
#include <vector>
#include <cmath>
#include <iostream>
#include <string>

//*********************************************************************************************
//      functions for matrix (3x3) and vector (3) operations
//*********************************************************************************************

//brief multiplies the matrices given: matC = matA * matB
//taken from SBBDiffusion.cpp
void MatMult(double matA[3][3],double matB[3][3],double matC[3][3]) {
	for (int r=0;r<3;r++)
	{
		for (int c=0;c<3;c++)
		{
			matC[r][c] =
				matA[r][0] * matB[0][c] +
				matA[r][1] * matB[1][c] +
				matA[r][2] * matB[2][c];
		}
	}
}

//multiplies the matrices given: matC = matA * matB'
//taken from SBBDiffusion.cpp
void MatMultTrans(double matA[3][3],double matB[3][3],double matC[3][3]) {
	for (int r=0;r<3;r++)
	{
		for (int c=0;c<3;c++)
		{
			matC[r][c] =
				matA[r][0] * matB[c][0] + //transposed matB
				matA[r][1] * matB[c][1] +
				matA[r][2] * matB[c][2];
		}
	}
}

//adds the two matrices given: matC = matA+matB
void MatAdd(double matA[3][3],double matB[3][3],double matC[3][3]) {
    for (int r=0;r<3;r++) {
        for (int c=0;c<3;c++){
		    matC[r][c] = matA[r][c]+matB[r][c];
        }
    }
}

double dotProduct(double dV1[3], double dV2[3]) {
    double dP = 0.;
    for(int r=0; r<3; ++r) {
        dP += (dV1[r]*dV2[r]);
    }
    return dP;
}

void crossProduct(double dV1[3], double dV2[3], double dVOut[3]) {
    dVOut[0] = dV1[1]*dV2[2] - dV1[2]*dV2[1];
    dVOut[1] = dV1[2]*dV2[0] - dV1[0]*dV2[2];
    dVOut[2] = dV1[0]*dV2[1] - dV1[1]*dV2[0];
}


double vecNormalize(double dV1[3]) {
    double dNorm = 0;
    int i;
    for(i=0; i<3; ++i) {
        dNorm += (dV1[i]*dV1[i]);
    }
    dNorm = sqrt(dNorm);
    if(dNorm > 0.000001) { //avoid "normalising" zero vector
        for(i=0; i<3; ++i) {
            dV1[i] /= dNorm;
        }
    }
    return dNorm;
}

void printMatrix(double adMat[3][3], std::string sTitle = "Matrix to print") {
    std::cout << sTitle << ": " << std::endl;
    std::cout << "\t" << adMat[0][0] << "\t" << adMat[0][1] << "\t" << adMat[0][2] 
              <<"\n\t"<< adMat[1][0] << "\t" << adMat[1][1] << "\t" << adMat[1][2]
              <<"\n\t"<< adMat[2][0] << "\t" << adMat[2][1] << "\t" << adMat[2][2] << std::endl;
}
//*****************************************************************************************************






//
/* these were moved inside the ReadFWFLund class
std::vector<float> interpolateGradientLinear(std::vector<float> inputAmpl, long lAvailableTime) {
    long lI;
    
    long lGRT = 10; //time steps of the returned gradient
    std::vector<float> interpGrad;
    lAvailableTime -= lGRT; //for a final 0 in the end
    interpGrad.reserve(lAvailableTime/lGRT+1);
    if(inputAmpl.empty() || lAvailableTime<10) {
        return interpGrad;
    }
    if(abs(inputAmpl.back()) > 0.0001) {
        inputAmpl.push_back(0);
    }
    if(inputAmpl.size() < 2) {
        inputAmpl.push_back(0);
    }
    double dTimeStepInitial = 1.0*lAvailableTime/(inputAmpl.size()-1); 
    long lCurrentOrigTimePoint = 0;
    long m = 0.;
    double dSmallValue = 0, dLargeValue = 0;
    for(lI=0; lI<lAvailableTime/lGRT; ++lI) {
         //find the points to interpolate from
        m = static_cast<long>(lI*lGRT/dTimeStepInitial)+1;        
        if(m < 0) {
            dSmallValue = 0;
            dLargeValue = 0; 
        } else if(m == 0) {
            dSmallValue = 0;
            dLargeValue = inputAmpl.at(m);
        } else if(m == inputAmpl.size()) {
            dSmallValue = inputAmpl.at(m-1);
            dLargeValue = 0;
        } else if(m > inputAmpl.size()) {
            dSmallValue = 0;
            dLargeValue = 0;
        } else {
            dSmallValue = inputAmpl.at(m-1);
            dLargeValue = inputAmpl.at(m);
        }
        interpGrad.push_back(dSmallValue + ((dLargeValue-dSmallValue)/dTimeStepInitial)*(lI*lGRT-(m-1)*dTimeStepInitial) );
    } //for(lI=0; lI<lAvailableTime/lGRT; ++lI) 
    interpGrad.push_back(0); //so the gradient always ends at zero
    return interpGrad;
    
}*/


bool calcBMatrix(std::vector<float> vdGP, std::vector<float> vdGR, std::vector<float> vdGS, 
                    std::vector<float> vdGP2, std::vector<float> vdGR2, std::vector<float> vdGS2,
                    long lSliSelTime, double dGamma, 
                    double dAmpl, 
                    double dBMat[3][3], double dKVec[3], long lDT) {
    double dBxx=0, dByy=0, dBzz=0, dBxy=0, dBxz=0, dByz=0;
    double dKx=0, dKy=0, dKz=0;
    if(vdGP.size()!=vdGR.size() || vdGP.size()!=vdGS.size() || vdGP.size()==0)
        return false;
    if(vdGP2.size()!=vdGR2.size() || vdGP2.size()!=vdGS2.size() || vdGP2.size()==0)
        return false;

    long lI;
    //first gradient
    for(lI=0; lI<vdGP.size(); ++lI) {
        dKx += dGamma*vdGP[lI]*dAmpl;//think about units
        dKy += dGamma*vdGR[lI]*dAmpl;
        dKz += dGamma*vdGS[lI]*dAmpl;
        dBxx += dKx*dKx;
        dByy += dKy*dKy;
        dBzz += dKz*dKz;
        dBxy += dKx*dKy;
        dBxz += dKx*dKz;
        dByz += dKy*dKz;
    }
    //180 pulse (gradients not added to b-matrix)
    dBxx += lSliSelTime*dKx*dKx; 
    dByy += lSliSelTime*dKy*dKy;
    dBzz += lSliSelTime*dKz*dKz;
    dBxy += lSliSelTime*dKx*dKy;
    dBxz += lSliSelTime*dKx*dKz;
    dByz += lSliSelTime*dKy*dKz;
    dKx *= -1;
    dKy *= -1;
    dKz *= -1;
    //second gradient
    for(lI=0; lI<vdGP2.size(); ++lI) {
        dKx += dGamma*vdGP2[lI]*dAmpl*lDT;//think about units
        dKy += dGamma*vdGR2[lI]*dAmpl*lDT;
        dKz += dGamma*vdGS2[lI]*dAmpl*lDT;
        dBxx += dKx*dKx*lDT;
        dByy += dKy*dKy*lDT;
        dBzz += dKz*dKz*lDT;
        dBxy += dKx*dKy*lDT;
        dBxz += dKx*dKz*lDT;
        dByz += dKy*dKz*lDT;
    }

    //fill the outputs
    dBMat[0][0]=dBxx; dBMat[0][1]=dBxy; dBMat[0][2]=dBxz;
    dBMat[1][0]=dBxy; dBMat[1][1]=dByy; dBMat[1][2]=dByz;
    dBMat[2][0]=dBxz; dBMat[2][1]=dByy; dBMat[2][2]=dBzz;
    dKVec[0] = dKx;
    dKVec[1] = dKy;
    dKVec[2] = dKz;

    return true;
}

bool calcBMatrix(std::vector< std::vector<float> > vdG, std::vector< std::vector<float> > vdG2,
                    long lSliSelTime, double dGamma, 
                    double dAmpl, double dBMat[3][3], double dKvec[3], long lDT=10) {
    if(vdG.size() < 3) return false;
    return calcBMatrix(vdG[0], vdG[1], vdG[2], vdG2[0], vdG2[1], vdG2[2],
                    lSliSelTime, dGamma, 
                    dAmpl, dBMat, dKvec, lDT);
}
