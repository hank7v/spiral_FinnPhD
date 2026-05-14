#include "MrServers/MrImaging/seq/common/myHelperClasses/ReadSingleColumn.h"

ReadSingleColumn::ReadSingleColumn() :
FileReadBase(1)
{}

ReadSingleColumn::ReadSingleColumn(std::string sFile) :
FileReadBase(sFile, 1)
{}
    
    //destructor 
ReadSingleColumn::~ReadSingleColumn()
{}

std::vector<float> ReadSingleColumn::getValues()
{
    return getColumnAsVector(0);
}

std::vector<long> ReadSingleColumn::getValuesAsLong() {
    std::vector<float> vfV = getColumnAsVector(0);
    std::vector<long> vlV(vfV.begin(),vfV.end());
    return vlV;
}


float ReadSingleColumn::getValue(long lI) {
    std::vector<float> vfV = getColumnAsVector(0);
    return vfV[lI%vfV.size()];
}

