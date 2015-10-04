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
typedef struct ServiceData
{
	/** The service that owns this data. */
	struct Service *sd_service_p;
} ServiceData;


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
 * A datatype for storing a ReferredService on a LinkedList
 */
typedef struct
{
	/** The base Listnode. */
	ListItem rsn_node;

	/** The ReferredService */
	ReferredService *rsn_service_p;
} ReferredServiceNode;



#endif		/* #ifndef GRASSROOTS_REFERRED_SERVICE_H */
