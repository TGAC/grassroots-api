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
#ifndef GRASSROOTS_REFERRED_SERVICE_H
#define GRASSROOTS_REFERRED_SERVICE_H

#include "grassroots_service_library.h"

#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "handler.h"
#include "user_details.h"

#include "jansson.h"
#include "tag_item.h"


/**
 * @brief A datatype detailing the addon services
 * that the grassroots offers. These are the
 * equivalent to iRods microservices.
 */

/* forward declarations */
struct Plugin;
struct Service;

/**
 * @brief The base structure for storing Service configuration data.
 */
typedef struct ReferredServiceData
{
	/** The service that owns this data. */
	struct Service *rsd_service_p;
} ReferredServiceData;


/**
 * A datatype which defines an available service, its capabilities and
 * its parameters.
 */
typedef struct ReferredService
{
	/** The Service that this ReferredService is using. */
	Service *rs_service_p;

	/** The configuration for this ReferredService */
	json_t *rs_config_p;
} ReferredService;


/**
 * A datatype for storing a ReferredService on a LinkedList.
 *
 * @extends ListItem
 */
typedef struct
{
	/** The base Listnode. */
	ListItem rsn_node;

	/** The ReferredService */
	ReferredService *rsn_service_p;
} ReferredServiceNode;



#endif		/* #ifndef GRASSROOTS_REFERRED_SERVICE_H */
