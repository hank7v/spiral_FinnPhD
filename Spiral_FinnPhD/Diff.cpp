#include "Diff.h"
#include<sstream>
#include<fstream>
double m1Slice(long Rs, long T, double gs) {

    return (4. * Rs * Rs - 6. * Rs * T + 3. * T * T) * gs / 12.;
}

double bValueSlice(long Rs, long T, double gs, double larmor) {

    return (-4. * Rs * Rs * Rs + 15. * Rs * Rs * T - 15. * Rs * T * T + 5. * T * T * T) * gs * gs
        * larmor * larmor / 60e18;
}

double bValueMono(long D, long R, long T, double g, double larmor) {

    return (20.* D * D * D - 5. * D * R * R + 1. * R * R * R + 30. * D * D * (R + T)) * g * g
        * larmor * larmor / 30e18;
}

double bValueSMono(long D, long R, long Rs, long T, double g, double gs, double larmor) {

    return (bValueMono(D, R, T, g, larmor) + bValueSlice(Rs, T, gs, larmor) + m1Slice(Rs, T, gs) * 2.
        * D * g * larmor * larmor / 1e18);
}
/*
int fiReadDiffDir(std::vector<double>& vdPF, std::vector<double>& vdRF, std::vector<double>& vdSF, std::string sFilename) {
    int iNoDir = fiGetFileLength(sFilename);
    return fiReadDiffDir(vdPF, vdRF, vdSF, iNoDir, sFilename);
}
//uses the given length
int fiReadDiffDir(std::vector<double>& vdPF, std::vector<double>& vdRF, std::vector<double>& vdSF, int iNoDir, std::string sFilename){
    long i = 0;
	//std::cout << "### in fiLoadExternalGradientForm, with file: " << sFilename << std::endl;
	std::ifstream ifsDiffDir(sFilename.c_str());
	if(!ifsDiffDir.is_open()) 
		return -1;
	std::string sLine;
	std::size_t pos;
    //check vector size
    if (vdPF.size() < iNoDir) {
        vdPF.resize(iNoDir);
        vdRF.resize(iNoDir);
        vdSF.resize(iNoDir);
    }
	while (std::getline(ifsDiffDir, sLine) && i<iNoDir) {
		//use # for parameters of the sequence and '/' for comments
		if (sLine[0] == '#' || sLine[0] == '/') {
			continue;
		}
		std::stringstream iss;
		iss << sLine;
		float a, b, c;
		if (!(iss >> a >> b >> c)) {return -2; } // error
        vdPF.at(i) = a;
        vdRF.at(i) = b;
        vdSF.at(i) = c;
        //vdPF.push_back(a);
		//vdRF.push_back(b);
        //vdSF.push_back(c);
		i++;
	}
	//check if the reading worked
	if (ifsDiffDir.bad()) 
		return -3;

	if (!ifsDiffDir.eof()) 
		return 1;

	ifsDiffDir.close();
	return 0;
}*/