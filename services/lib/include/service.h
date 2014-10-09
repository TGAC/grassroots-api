#ifndef WHEATIS_SERVICE_H
#define WHEATIS_SERVICE_H

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
typedef struct Service
{
	/**
	 * The platform-specific plugin that the code for the Service is
	 * stored in.
	 */
	struct Plugin *se_plugin_p;


	int (*se_run_fn) (ServiceData *service_data_p, ParameterSet *param_set_p);

	bool (*se_match_fn) (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p);

	bool (*se_has_permissions_fn) (ServiceData *service_data_p, const UserDetails * const user_p);

 	/**
 	 * Function to get the user-friendly name of the service.
 	 */
	const char *(*se_get_service_name_fn) (void);

	/**
	 * Function to get the user-friendly description of the service.
	 */
	const char *(*se_get_service_description_fn) (void);

	/**
	 * Function to get the ParameterSet for this Service.
	 */
	ParameterSet *(*se_get_params_fn) (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p);


	/**
	 * Any custom data that the service needs to store.
	 */
	ServiceData *se_data_p;

} Service;


typedef struct
{
	ListItem sn_node;
	Service *sn_service_p;
} ServiceNode;


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_SERVICE_API struct Service *GetServiceFromPlugin (struct Plugin * const plugin_p);

WHEATIS_SERVICE_API void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (void),
	const char *(*get_service_description_fn) (void),
	int (*run_fn) (ServiceData *service_data_p, ParameterSet *param_set_p),
	bool (*match_fn) (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p),
	ParameterSet *(*get_parameters_fn) (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p),
	ServiceData *data_p);

WHEATIS_SERVICE_API int RunService (Service *service_p, ParameterSet *param_set_p);

WHEATIS_SERVICE_API bool IsServiceMatch (Service *service_p, Resource *resource_p, Handler *handler_p);


/**
 * Get the user-friendly name of the service.
 *
 * @param service_p The Service to get the name for.
 * @return The name of Service.
 */
WHEATIS_SERVICE_API const char *GetServiceName (const Service *service_p);


/**
 * Get the user-friendly description of the service.
 *
 * @param service_p The Service to get the description for.
 * @return The description of Service.
 */
WHEATIS_SERVICE_API const char *GetServiceDescription (const Service *service_p);


/**
 * Get a newly-created ParameterSet describing the parameters for a given Service.
 *
 * @param service_p The Service to get the ParameterSet for.
 * @return The newly-created ParameterSet or <code>NULL</code> upon error. This
 * ParameterSet will need to be freed once it is no longer needed by calling FreeParameterSet.
 * @see FreeParameterSet.
 */
WHEATIS_SERVICE_API ParameterSet *GetServiceParameters (const Service *service_p, Resource *resource_p, const json_t *json_p);


/**
 * Free a Service and its associated Parameters and ServiceData.
 *
 * @param service_p The Service to free.
 */
WHEATIS_SERVICE_API void FreeService (Service *service_p);


WHEATIS_SERVICE_API ServiceNode *AllocateServiceNode (Service *service_p);


WHEATIS_SERVICE_API void FreeServiceNode (ListItem *node_p);


WHEATIS_SERVICE_API LinkedList *LoadMatchingServices (const char * const services_path_s, Resource *resource_p, Handler *handler_p);


/**
 * Generate a json-based description of a Service. This uses the Swagger definitions
 * as much as possible.
 *
 * @param service_p The Service to generate the description for.
 * @return The json-based representation of the Service or <code>NULL</code> if there was
 * an error.
 */
WHEATIS_SERVICE_API json_t *GetServiceAsJSON (const Service * const service_p, Resource *resource_p, const json_t *json_p);



WHEATIS_SERVICE_API const char *GetServiceDescriptionFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API const char *GetServiceNameFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API struct Service *GetServiceFromPlugin (Plugin * const plugin_p);


WHEATIS_SERVICE_API bool DeallocatePluginService (Plugin * const plugin_p);


WHEATIS_SERVICE_API json_t *GetServicesListAsJSON (LinkedList *services_list_p, Resource *resource_p, const json_t *json_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_SERVICE_H */
