/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef IRODS_RESOURCE_H
#define IRODS_RESOURCE_H

#include "rcConnect.h"
#include "data_resource.h"

#include "irods_handler_library.h"

typedef struct IRodsResource
{
	Resource irr_base_resource;
	char *irr_username_s;
	char *irr_password_s;
	rcComm_t *irr_connection_p;
} IRodsResource;



#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_HANDLER_API Resource *GetIRodsResource (const char * const username_s, const char * const password_s);

IRODS_HANDLER_API Resource *AllocateIRodsResource (rcComm_t *connection_p);

IRODS_HANDLER_API void FreeIRodsResource (Resource *resource_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_RESOURCE_H */
