#ifndef WHEATIS_REFERRED_SERVICE_H
#define WHEATIS_REFERRED_SERVICE_H

#include "wheatis_service_library.h"

#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "handler.h"
#include "user_details.h"

#include "jansson.h"
#include "tag_item.h"


/**
 * A datatype detailing the addon services
 * that the wheatis offers. These are the
 * equivalent to iRods microservices.
 */

/* forward declarations */
struct Plugin;
struct Service;

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
	Service *rs_service_p;
	json_t *rs_config_p;
} ReferredService;


typedef struct
{
	ListItem rsn_node;
	ReferredService *rsn_service_p;
} ReferredServiceNode;


#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_SERVICE_API struct Service *GetServiceFromPlugin (struct Plugin * const plugin_p, const json_t *service_config_p);




#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_REFERRED_SERVICE_H */
