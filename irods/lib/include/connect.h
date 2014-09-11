#ifndef IRODS_CONNECT_H
#define IRODS_CONNECT_H

#include "rcConnect.h"

#include "irods_util_library.h"

#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL_API rcComm_t *CreateConnection (char *username_s, char *password_s);

IRODS_UTIL_API void CloseConnection (rcComm_t *connection_p);

#ifdef __cplusplus
}
#endif

#endif		/* #ifndef IRODS_CONNECT_H */
