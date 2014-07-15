#ifndef BLAST_MICRO_SERVICE_H
#define BLAST_MICRO_SERVICE_H
  
  
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"


#ifdef __cplusplus
extern "C" {
#endif	

/*************************************/
/******** FUNCTION PROTOTYPES ********/
/*************************************/
int BlastSequenceData (msParam_t *in1_p, msParam_t *out1_p, ruleExecInfo_t *rule_p);

#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef BLAST_MICRO_SERVICE_H */


