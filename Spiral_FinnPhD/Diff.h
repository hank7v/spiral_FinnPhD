//anything related to diffusion goes here

#ifndef DIFF_H 
#define DIFF_H 1

#include <vector>
#include <string>

//b-value of a monopolar gradient pair
//gradient duration D [us], gradient ramp time R [us], 
//time between end of first and beginning of second gradient T[us]
//gradient amplitude g [mT/m],
//larmor: sSRF1.getLarmorConst() * 2 * 3.1415926536
//copied from aw_epic_iflow (lm_adPFG_7c)
double bValueMono(long D, long R, long T, double g, double larmor);

//This one also considers a slice selection gradient for the refocusing rf pulse on the same axis
//gradient duration D [us], gradient ramp time R [us], 
//time between end of first and beginning of second gradient T[us] (total time of slice sel gradient)
//gradient amplitude g [mT/m],
//larmor: sSRF1.getLarmorConst() * 2 * 3.1415926536
//slice selection ramp time Rs, slise selection amplitude gs
//copied from aw_epic_iflow (lm_adPFG_7c)
double bValueSMono(long D, long R, long Rs, long T, double g, double gs, double larmor);

//calculate the b-value produced by a slice selection gradient
//used by bValueSMono
double bValueSlice(long Rs, long T, double gs, double larmor);

//calculate the first moment of a slice selection gradient starting at t=0
//used by bValueSMono
double m1Slice(long Rs, long T, double gs);

//get the file length -> number of directions -> use fiGetFileLength from SpiralStuff_0_02

/*
//functions for reading the diffusion directions 
//gets the length itself, then calls the other version 
int fiReadDiffDir(std::vector<double>& vdPF, std::vector<double>& vdRF, std::vector<double>& vdSF, std::string sFilename);
//uses the given length
int fiReadDiffDir(std::vector<double>& vdPF, std::vector<double>& vdRF, std::vector<double>& vdSF, int iNoDir, std::string sFilename);
*/

//maybe normalization check

//read the file into an array

//maybe a function for rotation in x,y,z coord system 
#endif