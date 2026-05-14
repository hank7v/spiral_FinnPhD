//Class that is used to read a single column of values (but not row)
//The reason for columns is that the base class looks for linebreaks
//It allows to give information in a file header.

//Each line starting with '#' is considered potential header
//For the interpretation of the info keywords can be used
//which are listed before the interpretHeader function.


#ifndef READ_SINGLE_COLUMN_H
#define READ_SINGLE_COLUMN_H

#include "MrServers/MrImaging/seq/common/myHelperClasses/FileReadBase.h"

class ReadSingleColumn : public FileReadBase {
public:
    //constructor
    ReadSingleColumn();
    ReadSingleColumn(std::string sFile);
    
    //destructor 
    virtual ~ReadSingleColumn();

    virtual std::vector<float> getValues();
    virtual std::vector<long> getValuesAsLong();

    virtual float getValue(long lI);

private:
    using FileReadBase::m_lCol;
    using FileReadBase::setNoColumns;
};

#endif