/*
** Copyright 2014-2016 The Earlham Institute
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

/**
 * @file
 * @brief
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



#ifdef __cplusplus
extern "C" 
{
#endif

/**
 * Get a new Handler suitable for use with iRODS.
 *
 * @param user_p The credentials of the current user.
 * @return The newly-allocated Handler for iRODS data or <code>NULL</code>
 * upon error.
 * @memberof IRodsHandler
 * @ingroup irods_handler_group
 */
IRODS_HANDLER_API Handler *GetHandler (const UserDetails *user_p);


/**
 * Free an iRODS Handler.
 *
 * @param handler_p The Handler to free.
 * @memberof IRodsHandler
 * @ingroup irods_handler_group
 */
IRODS_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_STREAM_H */
