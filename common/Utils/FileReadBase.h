//Base class for reading data from external files:
//The files can contain comments, marked by '/', and 
//header lines, marked by '#'. If a derived class uses
//header informations, it must implement a function for
//interpreting it, but extraction can be done with 
//base class functions. 
//The data should be formatted in columns separated by
//whtie space. It is saved in a std::vector of floats.
//Floats were chosen as GRAD_PULSE_ARB uses floats for
//the ramp shape. 

//Lars Mueller, CUBRIC, 2018-12-01


//maybe include a clear() function


//double include guard
#ifndef FILEREAD_BASE_H
#define FILEREAD_BASE_H

#include<vector>
#include<string>
//#include<fstream>

//TODO check if this is needed 
//function for getting the sign
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class FileReadBase {
public:
    //constructors with different inputs
    FileReadBase();
    //input string with filename
    FileReadBase(std::string sFile);
    //any additional columns are ignored
    FileReadBase(long lColumns);
    FileReadBase(std::string sFile, long lColumns);
    //destructor
    virtual ~FileReadBase();
    
    //set and get functions for member variables
    long getNoColumns() {return m_lCol;}
    //returns length of one column that is used at the moment, not necessarily filelength
    long getLength() {return m_lLength;}
    //if you want to limit the number of rows
    void setLength(long lNewLength) {m_lLength = lNewLength; m_bFileReadOk = false;} 
    
    void setLengthToFileLength() {m_lLength = getFileLength(); m_bFileReadOk=false;}
    std::string getFilename() {return m_sFilename;}
    //does not trigger a new readfile, but resets bool to indicate that the file was not read
    void setFilename(std::string sNewFile) {m_sFilename = sNewFile; m_bFileReadOk=false; }
   
    bool isFileReadAlright(){return m_bFileReadOk;}
    //void setFilename(char* pcFilename){muss ich noch nachschauen}
    void setNoColumns(long lNoCol) {m_lCol = lNoCol; m_bFileReadOk = false;} 
    //This one can be used to get a certain column (starting at 0)
    std::vector<float> getColumnAsVector(long lColumnNo);
    //lLength for checking the size against the m_lLength 
    bool getColumnAsArray(long lColumnNo, float fArr[], long lLength);

    //returns the euclidian norm of the row given by lRowNo (beginning at 0)
    virtual float getSquareRowNorm(long lRowNo);
    //for the array versions, maybe switch to std::array
    virtual std::vector<float> getRowAsVector(long lRowNo);
    //fArr[]: pointer to array reserved in calling function
    virtual bool getRowAsArray(long lRowNo, float fArr[], long lLength);
    
    //actually read the input file
    //values < 0 indicate an error
    virtual int readFile();
    //read first lLength lines of data of the file
    virtual int readFile(long lLength);
    //read the file to get the number of rows of actual data, not including comments, header, blank lines
    virtual long getFileLength();
    //assemble all lines beginning with '#' in a string and return it
    virtual std::string getFileHeader();

protected:
    
    bool bCheckExistence();
    
    //number of columns
    long m_lCol; //This is the one that is changed in derived classes (atually it is fixed in my cases)
    //number of rows
    long m_lLength;
    std::string m_sFilename;
    
    //long m_lFileLength;

    //more flexible, worse perfomance
    //std::vector<std::vector<float>> m_vvfContents;
    //needs better handling. calculating the indices 
    std::vector<float> m_vfContents; 

    //std::string m_sHeader;
    //std::fstream m_sFileStream;

    //indicate if readfile() returned with >=0 since the 
    //last change to any parameter that would influence 
    //the data
    bool m_bFileReadOk;

private:

};


#endif //double include guard