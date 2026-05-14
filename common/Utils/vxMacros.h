//macros for multi idea compilation, to be updated as I go along
//this is mostly taken from the Boland of the idea user meeting in Maastricht 2016


//idea version to be defined either in the makefiles or the sequence as VX_VERSION
//rules for version conversion:
//  first letter -> 2 digit number (e.g. B->02, E->05)
//  number is kept as 2 digit number
//  Last character -> 2 digit number (e.g. A->01)
#ifndef VXMACROS
#define VXMACROS 1
//blatantly stolen from parameter_map3_multiplatform
#if defined(VXWORKS) || defined(BUILD_PLATFORM_LINUX)
#define VX_SCANNER_BUILD
#endif

#define VX_VERSION_VB       020000
#define VX_VERSION_VB17     021700
#define VX_VERSION_VD       040000
#define VX_VERSION_VD11D    041104
#define VX_VERSION_VD13     041300
#define VX_VERSION_VE       050000
#define VX_VERSION_VE11C    051103

#ifndef MAX
#define MAX(a,b)        (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))
#endif

#define OnErrorPrintAndReturn(S, P) if (((S) & NLS_SEV) != NLS_SUCCESS) \
{ fprintf(stdout, "%s Error from %s \n", ptModule, P); return(S); }

#define OnErrorReturn(S) if(((S) & NLS_SEV) != NLS_SUCCESS) return(S)

//changes from VB17 to VD11
#if VX_VERSION < VX_VERSION_VD
#define VX_ARGS1(X)         &X
#define VX_ARGS2(X,Y)       &X, &Y
#define VX_ARGS3(X,Y,Z)     &X, &Y, &Z

#define VX_SIZE(sliceSeries)            sliceSeries.size()
#define VX_MTC_ACTIVE(prot)             prot.preparationPulses().MTC()
#define VX_FATSAT(prot)                 prot.preparationPulses().fatSuppression()
#define VX_FATSAT_MODE(prot)            prot.preparationPulses().fatSatMode()
#define VX_NUMBER_REPETITIONS(prot)     prot.repetitions()
#define VX_ALFREE(prot, pos)            prot.wipMemBlock().alFree[pos]
#define VX_ADFREE(prot, pos)            prot.wipMemBlock().adFree[pos]
#define VX_MDH(adc)                     adc.Mdh
#define VX_ROTMAT(slice)                slice.m_sROT_MATRIX
//#define VX_ROTMAT_FRTEIBINIT(slice)     &slice.m_sROT_MATRIX
#define VX_BASE_SEQUENCE_CLASS          SeqIF
                                        
#else  //at least VD11

#define VX_ARGS1(X)         X
#define VX_ARGS2(X,Y)       X, Y
#define VX_ARGS3(X,Y,Z)     X, Y, Z
//m_mySeqLoop.setpCppSequence(this);
#define VX_SIZE(sliceSeries)            sliceSeries.getlSize()
#define VX_MTC_ACTIVE(prot)             prot.preparationPulses().getucMTC()
#define VX_FATSAT(prot)                 prot.preparationPulses().getucFatSat()
#define VX_FATSAT_MODE(prot)            prot.preparationPulses().getucFatSatMode()
#define VX_NUMBER_REPETITIONS(prot)     prot.getlRepetitions()
#define VX_ALFREE(prot, pos)            prot.getsWipMemBlock().getalFree()[pos]
#define VX_ADFREE(prot, pos)            prot.getsWipMemBlock().getadFree()[pos]
#define VX_MDH(adc)                     adc.getMDH()
#define VX_ROTMAT(slice)                slice.getROT_MATRIX()
//#define VX_ROTMAT_FRTEIBINIT(slice)     slice.getROT_MATRIX()

#define VX_BASE_SEQUENCE_CLASS          StdSeqIF

#endif //VX_VERSION  //changes from VB17 to VD11

//changes from VD11 to VE11, assuming they were done with VD13 (not tested yet)
#if VX_VERSION < VX_VERSION_VD13
#define VX_RF_ENERGY_TYPE                   double
#define VX_SBB_GET_ENERGY_PER_REQUEST(sbb)  sbb.getEnergyPerRequest()
#define VX_RF_GET_ENERGY(rf)                rf.getPulseEnergyWs()
#define VX_SET_SEQEXPO_ENERGY(SE,energy)    SE.setRFEnergyInSequence_Ws(energy)
#define VX_SEQLOOP_GET_ENERGY(seqloop,prot) seqloop.getEnergy(prot)

#else
#define VX_RF_ENERGY_TYPE           MrProtocolData::SeqExpoRFInfo
#define VX_SBB_GET_ENERGY_PER_REQUEST(sbb)  sbb.getRFInfoPerRequest()
#define VX_RF_GET_ENERGY(rf)                rf.getRFInfo()
#define VX_SET_SEQEXPO_ENERGY(SE,energy)    SE.setRFInfo(energy)
#define VX_SEQLOOP_GET_ENERGY(seqloop,prot) seqloop.getRFInfo(prot)

#endif
//tr, ti ?



#endif //VXMACROS double include guard