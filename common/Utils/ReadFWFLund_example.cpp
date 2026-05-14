//This is an example on how to use ReadFWFLund (copied and slightly modified from one of my sequences)
//for simplicity I was too lazy to include every declaration of every variable
//they should normally follow some naming convention that the type is indicated
//by the first letters (wip comes from the Special Card and the next letter indicates the type) 
//l:long, s:string, d:double, f:float, b:bool, vector<type>: vt, gradients: sG
//member variables have a m_ in front

//The class is set to be located in "MrServers/MrImaging/seq/common/myHelperClasses/"
// together with all my helper classes
// the needed header files are
//"MrServers/MrImaging/seq/common/myHelperClasses/vxMacros.h"
//"MrServers/MrImaging/seq/common/myHelperClasses/ReadFWFLund.h"
//"MrServers/MrImaging/seq/common/myHelperClasses/Read3Trajs.h"
//"MrServers/MrImaging/seq/common/myHelperClasses/FileReadBase.h"

//for the compilation you wil need to include a few .cpp in your makefile
//CPPSOURCESFROM(ReadFWFLund,			../common/myHelperClasses)
//CPPSOURCESFROM(Read3Trajs,			../common/myHelperClasses)
//CPPSOURCESFROM(FileReadBase,		../common/myHelperClasses)

 std::string sFreeDiffFile(getenv("CustomerSeq"));
#ifdef WIN32
        sFreeDiffFile += "\\Spiral_FWF\\FreeWaveforms\\"; 
#else 
        sFreeDiffFile += "/Spiral_FWF/FreeWaveforms/";
#endif
        sFreeDiffFile += wipsFwfFile;
        sFreeDiffFile += ".txt";
        
        m_readFreeDiff.setFilename(sFreeDiffFile);
        m_readFreeDiff.setAsymmetricBasedOnFiles();
        m_readFreeDiff.setReverseSecondGradient(m_readFreeDiff.isSymmetric()); //if we read only one file, we want the second to be time reversed
        m_readFreeDiff.setLengthToFileLength();//make sure to read the whole file (e.g. if it changed)            
        if (m_readFreeDiff.readFile() < 0) {      
            return SEQU_ERROR;
        }

        m_readFreeDiff.setGradientAmplitudes(dDiffGradAmpl1, dDiffGradAmpl2); //for calculating the integrals, e.g. b-matrix
        m_readFreeDiff.interpGradients(wiplSmallDelta, wiplSmallDelta2); //Duration of first and second gradient
        m_readFreeDiff.setRefocDuration(wiplCapitalDelta - wiplSmallDelta - lTimeRefoc - 2*lCrushTime); //time from end of first to start of second gradient
        
        //do dummy prep of the gradients to check them
        m_sGFwfP1.setMaxMagnitude(dDiffGradAmpl1);//(SysProperties::getGradMaxAmplAbsolute());
        m_sGFwfR1.setMaxMagnitude(dDiffGradAmpl1);
        m_sGFwfS1.setMaxMagnitude(dDiffGradAmpl1);
        
        m_sGFwfP1.setMinRiseTime(dDiffMinRiseTime1);//(SysProperties::getGradMinRiseTimeAbsolute());
        m_sGFwfR1.setMinRiseTime(dDiffMinRiseTime1);
        m_sGFwfS1.setMinRiseTime(dDiffMinRiseTime1);

        m_vfGFwfP1 = m_readFreeDiff.getFirstInterpPhaseGradient(); //I think m_vfGFwfP1 needs to exist in prep and run and so i made it a member of my sequence
        m_sGFwfP1.setRampShape(&m_vfGFwfP1[0], wiplSmallDelta/10);//, 0, true);
        m_sGFwfP1.setAmplitude(dDiffGradAmpl1);
        m_sGFwfP1.setRampUpTime(wiplSmallDelta);
        m_sGFwfP1.setDuration(wiplSmallDelta);
        if(!m_sGFwfP1.prep()) return GFwfP1.getNLSStatus();
        if(!m_sGFwfP1.check()) return m_sGFwfP1.getNLSStatus();

        m_vfGFwfR1 = m_readFreeDiff.getFirstInterpReadGradient();
        m_sGFwfR1.setRampShape(&m_vfGFwfR1[0], wiplSmallDelta/10);//, 0, true);
        m_sGFwfR1.setAmplitude(dDiffGradAmpl1);
        m_sGFwfR1.setRampUpTime(wiplSmallDelta);
        m_sGFwfR1.setDuration(wiplSmallDelta);
        if(!m_sGFwfR1.prep()) return m_sGFwfR1.getNLSStatus();
        if(!m_sGFwfR1.check()) return m_sGFwfR1.getNLSStatus();

        m_vfGFwfS1 = m_readFreeDiff.getFirstInterpSliceGradient();
        m_sGFwfS1.setRampShape(&m_vfGFwfS1[0], wiplSmallDelta/10);//, 0, true);
        m_sGFwfS1.setAmplitude(dDiffGradAmpl1);
        m_sGFwfS1.setRampUpTime(wiplSmallDelta);
        m_sGFwfS1.setDuration(wiplSmallDelta);
        if(!m_sGFwfS1.prep()) return m_sGFwfS1.getNLSStatus();
        if(!m_sGFwfS1.check()) return m_sGFwfS1.getNLSStatus();
        
        m_sGFwfP2.setMaxMagnitude(dDiffGradAmpl2);//(SysProperties::getGradMaxAmplAbsolute());
        m_sGFwfR2.setMaxMagnitude(dDiffGradAmpl2);
        m_sGFwfS2.setMaxMagnitude(dDiffGradAmpl2);
        
        m_sGFwfP2.setMinRiseTime(dDiffMinRiseTime2);//(SysProperties::getGradMinRiseTimeAbsolute());
        m_sGFwfR2.setMinRiseTime(dDiffMinRiseTime2);
        m_sGFwfS2.setMinRiseTime(dDiffMinRiseTime2);

        m_vfGFwfP2 = m_readFreeDiff.getSecondInterpPhaseGradient();
        m_sGFwfP2.setRampShape(&m_vfGFwfP2[0], wiplSmallDelta2/10);//, 0, true);
        m_sGFwfP2.setAmplitude(dDiffGradAmpl2);
        m_sGFwfP2.setRampUpTime(wiplSmallDelta2);
        m_sGFwfP2.setDuration(wiplSmallDelta2);
        if(!m_sGFwfP2.prep()) return m_sGFwfP2.getNLSStatus();
        if(!m_sGFwfP2.check()) return m_sGFwfP2.getNLSStatus();

        m_vfGFwfR2 = m_readFreeDiff.getSecondInterpReadGradient();
        m_sGFwfR2.setRampShape(&m_vfGFwfR2[0], wiplSmallDelta2/10);//, 0, true);
        m_sGFwfR2.setAmplitude(dDiffGradAmpl2);
        m_sGFwfR2.setRampUpTime(wiplSmallDelta2);
        m_sGFwfR2.setDuration(wiplSmallDelta2);
        if(!m_sGFwfR2.prep()) return m_sGFwfR2.getNLSStatus();
        if(!m_sGFwfR2.check()) return m_sGFwfR2.getNLSStatus();

        m_vfGFwfS2 = m_readFreeDiff.getSecondInterpSliceGradient();
        m_sGFwfS2.setRampShape(&m_vfGFwfS2[0], wiplSmallDelta2/10);//, 0, true);
        m_sGFwfS2.setAmplitude(dDiffGradAmpl2);
        m_sGFwfS2.setRampUpTime(wiplSmallDelta2);
        m_sGFwfS2.setDuration(wiplSmallDelta2);
        if(!m_sGFwfS2.prep()) return m_sGFwfS2.getNLSStatus();
        if(!m_sGFwfS2.check()) return m_sGFwfS2.getNLSStatus();

        if( !m_readFreeDiff.calculateGradientIntegrals() ){
                return SEQU_ERROR;
        }
        double dBMat[3][3] = {0};
        m_readFreeDiff.getBTensor(dBMat);
        dBValue = dBMat[0][0] + dBMat[1][1] + dBMat[2][2];
