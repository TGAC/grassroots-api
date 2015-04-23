#ifndef BLAST_MICRO_SERVICE_H
#define BLAST_MICRO_SERVICE_H
  
  
#include "msParam.h"
#include "reGlobalsExtern.h"
/*#include "irods_ms_plugin.h"*/

#ifdef __cplusplus
extern "C" {
#endif	

/*************************************/
/******** FUNCTION PROTOTYPES ********/
/*************************************/
int BlastSequenceData (msParam_t *in1_p, msParam_t *out1_p, ruleExecInfo_t *rei);

#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef BLAST_MICRO_SERVICE_H */


