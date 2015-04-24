#ifndef WHEATIS_SERVICE_H
#define WHEATIS_SERVICE_H

#include "wheatis_service_library.h"

#include "byte_buffer.h"
#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "handler.h"
#include "user_details.h"

#include "uuid/uuid.h"

#include "jansson.h"
#include "tag_item.h"



/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to 
 * 
 * 		extern const char *SERVICE_NAME_S;
 * 
 * however if ALLOCATE_JSON_TAGS is defined then it will 
 * become
 * 
 * 		const char *SERVICE_NAME_S = "path";
 * 
 * ALLOCATE_JSON_TAGS must be defined only once prior to 
 * including this header file. Currently this happens in
 * json_util.c.
 */
#ifdef ALLOCATE_PATH_TAGS
	#define PATH_PREFIX WHEATIS_SERVICE_API
	#define PATH_VAL(x)	= x
#else
	#define PATH_PREFIX extern
	#define PATH_VAL(x)	
#endif



PATH_PREFIX const char *SERVICES_PATH_S PATH_VAL("services");
PATH_PREFIX const char *REFERENCES_PATH_S PATH_VAL("references");


/**
 * A datatype detailing the addon services
 * that the wheatis offers. These are the
 * equivalent to iRods microservices.
 */

/* forward declarations */
struct Plugin;
struct Service;

typedef enum OperationStatus
{
	OS_ERROR = -1,
	OS_IDLE,
	OS_FAILED_TO_START,
	OS_STARTED,
	OS_FINISHED,
	OS_FAILED,
	OS_SUCCEEDED
} OperationStatus;



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

	bool se_is_specific_service_flag;

	json_t *(*se_run_fn) (struct Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

	bool (*se_match_fn) (struct Service *service_p, Resource *resource_p, Handler *handler_p);

	bool (*se_has_permissions_fn) (struct Service *service_p, const UserDetails * const user_p);

 	/**
 	 * Function to get the user-friendly name of the service.
 	 */
	const char *(*se_get_service_name_fn) (struct Service *service_p);

	/**
	 * Function to get the user-friendly description of the service.
	 */
	const char *(*se_get_service_description_fn) (struct Service *service_p);

	/**
	 * Function to get a web address for more information about the service
	 */
	const char *(*se_get_service_info_uri_fn) (struct Service *service_p);


	/**
	 * Function to get the ParameterSet for this Service.
	 */
	ParameterSet *(*se_get_params_fn) (struct Service *service_p, Resource *resource_p, const json_t *json_p);


	json_t *(*se_get_results_fn) (struct Service *service_p, const uuid_t service_id);


	OperationStatus (*se_get_status_fn) (struct Service *service_p, const uuid_t service_id);


	/**
	 * Function to release the ParameterSet for this Service.
	 */
	void (*se_release_params_fn) (struct Service *service_p, ParameterSet *params_p);

	bool (*se_close_fn) (struct Service *service_p);


	/** Is the service currently in an open state? */
	OperationStatus se_status;

	/** Unique Id for this service */
	uuid_t se_id;


	/**w
	 * Any custom data that the service needs to store.
	 */
	ServiceData *se_data_p;

} Service;


typedef struct
{
	ListItem sn_node;
	Service *sn_service_p;
} ServiceNode;


typedef struct ServicesArray
{
	Service **sa_services_pp;
	uint32 sa_num_services;	
} ServicesArray;





#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_SERVICE_API ServicesArray *GetServicesFromPlugin (Plugin * const plugin_p, const json_t *service_config_p);


WHEATIS_SERVICE_API void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (Service *service_p),
	const char *(*get_service_description_fn) (Service *service_p),
	const char *(*se_get_service_info_uri_fn) (struct Service *service_p),
	json_t *(*run_fn) (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p),
	bool (*match_fn) (Service *service_p, Resource *resource_p, Handler *handler_p),
	ParameterSet *(*get_parameters_fn) (Service *service_p, Resource *resource_p, const json_t *json_p),
	void (*release_parameters_fn) (Service *service_p, ParameterSet *params_p),
	bool (*close_fn) (struct Service *service_p),
	json_t *(*get_results_fn) (struct Service *service_p, const uuid_t service_id),
	OperationStatus (*get_status_fn) (Service *service_p, const uuid_t service_id),
	bool specific_flag,
	ServiceData *data_p);

WHEATIS_SERVICE_API json_t *RunService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

WHEATIS_SERVICE_API bool IsServiceMatch (Service *service_p, Resource *resource_p, Handler *handler_p);


/**
 * Get the user-friendly name of the service.
 *
 * @param service_p The Service to get the name for.
 * @return The name of Service.
 */
WHEATIS_SERVICE_API const char *GetServiceName (Service *service_p);


/**
 * Get the user-friendly description of the service.
 *
 * @param service_p The Service to get the description for.
 * @return The description of Service.
 */
WHEATIS_SERVICE_API const char *GetServiceDescription (Service *service_p);



/**
 * Get the address of a web page about the service.
 *
 * @param service_p The Service to get the description for.
 * @return The address of the page or NULL if there isn't one.
 */
WHEATIS_SERVICE_API const char *GetServiceInformationURI (Service *service_p);

/**
 * Get a newly-created ParameterSet describing the parameters for a given Service.
 *
 * @param service_p The Service to get the ParameterSet for.
 * @return The newly-created ParameterSet or <code>NULL</code> upon error. This
 * ParameterSet will need to be freed once it is no longer needed by calling FreeParameterSet.
 * @see FreeParameterSet.
 */
WHEATIS_SERVICE_API ParameterSet *GetServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);


/**
 * Get the unique id of a service object.
 *
 * @param service_p The Service to get the id for.
 * @return The string of the id.
 */
WHEATIS_SERVICE_API char *GetServiceUUIDAsString (Service *service_p);


/**
 * Free a Service and its associated Parameters and ServiceData.
 *
 * @param service_p The Service to free.
 */
WHEATIS_SERVICE_API void FreeService (Service *service_p);


WHEATIS_SERVICE_API ServiceNode *AllocateServiceNode (Service *service_p);


WHEATIS_SERVICE_API void FreeServiceNode (ListItem *node_p);


WHEATIS_SERVICE_API void LoadMatchingServicesByName (LinkedList *services_p, const char * const services_path_s, const char *service_name_s, const json_t *json_config_p);

WHEATIS_SERVICE_API void LoadMatchingServices (LinkedList *services_p, const char * const services_path_s, Resource *resource_p, Handler *handler_p, const json_t *json_config_p);


WHEATIS_SERVICE_API void LoadKeywordServices (LinkedList *services_p, const char * const services_path_s, const json_t *json_config_p);


WHEATIS_SERVICE_API void AddReferenceServices (LinkedList *services_p, const char * const references_path_s, const char * const services_path_s, const char *operation_name_s, const json_t *config_p);


WHEATIS_SERVICE_API bool CloseService (Service *service_p);


/**
 * Get the results from a long running service
 *
 * @param service_p The Service to get the results for
 * @return The results or NULL if they are not any.
 */
WHEATIS_SERVICE_API json_t *GetServiceResults (Service *service_p, uuid_t service_id);

/**
 * Generate a json-based description of a Service. This uses the Swagger definitions
 * as much as possible.
 *
 * @param service_p The Service to generate the description for.
 * @return The json-based representation of the Service or <code>NULL</code> if there was
 * an error.
 */
WHEATIS_SERVICE_API json_t *GetServiceAsJSON (Service * const service_p, Resource *resource_p, const json_t *json_p, const bool add_id_flag);


WHEATIS_SERVICE_API const char *GetServiceDescriptionFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API const char *GetServiceNameFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API const char *GetOperationDescriptionFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API const char *GetOperationNameFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API const char *GetOperationInformationURIFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API const char *GetIconPathFromJSON (const json_t * const root_p);


WHEATIS_SERVICE_API bool DeallocatePluginService (Plugin * const plugin_p);


WHEATIS_SERVICE_API json_t *GetServicesListAsJSON (LinkedList *services_list_p, Resource *resource_p, const json_t *json_p, const bool add_service_ids_flag);


WHEATIS_SERVICE_API void ReleaseServiceParameters (Service *service_p, ParameterSet *params_p);


/**
 * Free a ServicesArray and each of its Services.
 *
 * @param services_p The ServicesArray to free.
 */
WHEATIS_SERVICE_API void FreeServicesArray (ServicesArray *services_p);


/**
 * Allocate an empty ServicesArray.
 *
 * @param num_services The number of potential Services that the ServicesArray will hold.
 */
WHEATIS_SERVICE_API ServicesArray *AllocateServicesArray (const uint32 num_services);

/**
 * Free a ServicesArray and each of its Services.
 *
 * @param services_p The ServicesArray to free.
 */
WHEATIS_SERVICE_API void FreeServicesArray (ServicesArray *services_p);


/**
 * Allocate an empty ServicesArray.
 *
 * @param num_services The number of potential Services that the ServicesArray will hold.
 */
WHEATIS_SERVICE_API ServicesArray *AllocateServicesArray (const uint32 num_services);


WHEATIS_SERVICE_LOCAL void AssignPluginForServicesArray (ServicesArray *services_p, Plugin *plugin_p);


WHEATIS_SERVICE_API json_t *CreateServiceResponseAsJSON (Service *service_p, OperationStatus status, json_t *result_json_p, const uuid_t service_id);


WHEATIS_SERVICE_API ServicesArray *GetReferenceServicesFromJSON (json_t *config_p, const char *plugin_name_s, Service *(*get_service_fn) (json_t *config_p, size_t i));


WHEATIS_SERVICE_API OperationStatus GetCurrentServiceStatus (Service *service_p, const uuid_t service_id);


WHEATIS_SERVICE_API void SetCurrentServiceStatus (Service *service_p, const uuid_t service_id, const OperationStatus status);


WHEATIS_SERVICE_LOCAL OperationStatus DefaultGetServiceStatus (Service *service_p, const uuid_t service_id);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_SERVICE_H */
