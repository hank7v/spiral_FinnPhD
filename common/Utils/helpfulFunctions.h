//This is for functions that are generally useful over several classes/functions/sequence
#ifndef HELPFUL_FUNCTIONS_H
#define HELPFUL_FUNCTIONS_H

#include <string>

void MatMult(double matA[3][3],double matB[3][3],double matC[3][3]);
void MatMultTrans(double matA[3][3],double matB[3][3],double matC[3][3]);
void MatAdd(double matA[3][3],double matB[3][3],double matC[3][3]);
double dotProduct(double dV1[3], double dV[3]);
void crossProduct(double dV1[3], double dV2[3], double dVOut[3]);
double vecNormalize(double dV1[3]);
void  printMatrix(double adMat[3][3], std::string sTitle = "Matrix to print");

bool calcBMatrix(std::vector< std::vector<float> > vdG, std::vector< std::vector<float> > vdG2,
                    long lSliSelTime, double dGamma, 
                    double dAmpl, double dBMat[3][3], double dKvec[3], long lDT=10) ;

bool calcBMatrix(std::vector<float> vdGP, std::vector<float> vdGR, std::vector<float> vdGS, 
                    std::vector<float> vdGP2, std::vector<float> vdGR2, std::vector<float> vdGS2,
                    long lSliSelTime, double dGamma, double dAmpl, 
                    double dBMat[3][3], double dKVec[3], long lDT);

#endif