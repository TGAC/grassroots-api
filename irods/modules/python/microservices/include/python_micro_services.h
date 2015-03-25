#ifndef PYTHON_MICRO_SERVICES_H
#define PYTHON_MICRO_SERVICES_H
  
  
#include "msParam.h"
#include "reGlobalsExtern.h"
/*#include "irods_ms_plugin.h"*/

#ifdef __cplusplus
extern "C" {
#endif	

/*************************************/
/******** FUNCTION PROTOTYPES ********/
/*************************************/
int ObjectRegistered (msParam_t *user_p, msParam_t *resource_p, msParam_t *collection_p, msParam_t *data_object_p, ruleExecInfo_t *rei);

#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef PYTHON_MICRO_SERVICES_H */


