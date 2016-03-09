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
#ifndef IRODS_STREAM_H
#define IRODS_STREAM_H

#include <time.h>

#include <jansson.h>


#include "rcConnect.h"
#include "dataObjInpOut.h"
#include "objStat.h"

#include "irods_handler_library.h"


#include "linked_list.h"
#include "typedefs.h"
#include "query.h"
#include "handler.h"
#include "tags.h"


/**
 * A datatype for a Handler that can access data objects
 * and collections within an iRODS zone.
 *
 * @extends Handler
 */
typedef struct IRodsHandler
{
	/** The base handler */
	Handler irh_base_handler;

	/**
	 * @private
	 * The connection to the iRODS server.
	 */
	struct IRodsConnection *irh_connection_p;

	/**
	 * @private
	 * The currently opened data object.
	 */
	openedDataObjInp_t *irh_obj_p;


	/**
	 * @private
	 * Metadata about the currently opened data object.
	 */
	rodsObjStat_t *irh_stat_p;

	/**
	 * @private
	 * The HandlerStatus for this Handler.
	 */
	HandlerStatus irh_status;
} IRodsHandler;



#define TAG_IRODS_BASE 	(TAG_USER | 0x000000F0)

#define TAG_IRODS_USER  (TAG_IRODS_BASE | 0x00000001)
#define TAG_IRODS_PASSWORD  (TAG_IRODS_BASE | 0x00000002)
#define TAG_IRODS_CONNECTION  (TAG_IRODS_BASE | 0x00000003)



#ifdef __cplusplus
extern "C" 
{
#endif

IRODS_HANDLER_API Handler *GetHandler (const json_t *tags_p);

IRODS_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_STREAM_H */
