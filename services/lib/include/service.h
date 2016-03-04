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
#ifndef GRASSROOTS_SERVICE_H
#define GRASSROOTS_SERVICE_H

#include "grassroots_service_library.h"

#include "byte_buffer.h"
#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "handler.h"
#include "user_details.h"
#include "operation.h"
#include "paired_service.h"

#include "uuid/uuid.h"

#include "jansson.h"
#include "tag_item.h"

struct ExternalServer;

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
	#define PATH_PREFIX GRASSROOTS_SERVICE_API
	#define PATH_VAL(x)	= x
#else
	#define PATH_PREFIX extern
	#define PATH_VAL(x)	
#endif



PATH_PREFIX const char *SERVICES_PATH_S PATH_VAL("services");
PATH_PREFIX const char *REFERENCES_PATH_S PATH_VAL("references");


/**
 * A datatype detailing the addon services
 * that the grassroots offers. These are the
 * equivalent to iRods microservices.
 */

/* forward declarations */
struct Plugin;
struct Service;
struct ServiceJob;
struct ServiceJobSet;



typedef struct ServiceData
{
	/** The service that owns this data. */
	struct Service *sd_service_p;

	/**
	 * If this service has an entry in global server config it will be stored here.
	 * The value in the service config will be "services" -> \<service_name\>
	 */
	const json_t *sd_config_p;
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

	/**
	 * @brief Is the service self-contained.
	 *
	 * If a Service has a specific task that it can perform then this
	 * is <code>true</code>. If it is a utility that is reused for different
	 * Services, such as a generic web search service, then this is <code>false</code>
	 */
	bool se_is_specific_service_flag;

	/**
	 * Run this Service.
	 *
	 * @param service_p The Service to run.
	 * @param param_set_p The ParameterSet to run the Service with.
	 * @param credentials_p An optional set of credentials if the Service requires it.
	 * @return A newly-allocated ServiceJobSet with the status of the ServiceJobs started
	 * by this call or <code>NULL</code> if there was an error.
	 * @see RunService
	 */
	struct ServiceJobSet *(*se_run_fn) (struct Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

	ParameterSet *(*se_match_fn) (struct Service *service_p, Resource *resource_p, Handler *handler_p);

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


	/**
	 * Get the JSON fragment for a given job that has been run by this Service.
	 *
	 * @param service_p A pointer to this Service.
	 * @param job_id The uuid_t of the ServiceJob.
	 * @return The JSON fragment or <code>NULL</code> upon error.
	 */
	json_t *(*se_get_results_fn) (struct Service *service_p, const uuid_t job_id);


	/**
	 * Get the OperationStatus for a given job that has been run by this Service.
	 *
	 * @param service_p A pointer to this Service.
	 * @param job_id The uuid_t of the ServiceJob.
	 * @return The OperationStatus for the given job.
	 */
	OperationStatus (*se_get_status_fn) (struct Service *service_p, const uuid_t job_id);

	/**
	 * Function to release the ParameterSet for this Service.
	 */
	void (*se_release_params_fn) (struct Service *service_p, ParameterSet *params_p);

	bool (*se_close_fn) (struct Service *service_p);

	/**
	 * If this is <code>true</code> then when the Service is ran, it will not return
	 * until the job has completed. If <code>false</code>, then the Service will
	 * return straight away and the job will run in the background.
	 */
	bool se_synchronous_flag;

	/** Unique Id for this service */
	uuid_t se_id;

	struct ServiceJobSet *se_jobs_p;

	LinkedList se_paired_services;

	/**
	 * Any custom data that the service needs to store.
	 */
	ServiceData *se_data_p;

} Service;


/**
 * A datatype for storing Services on a LinkedList
 */
typedef struct
{
	/** The List Node */
	ListItem sn_node;

	/* The Service */
	Service *sn_service_p;
} ServiceNode;


/**
 * A datatype for having a set of Services.
 */
typedef struct ServicesArray
{
	/** An array of pointers to Services */
	Service **sa_services_pp;

	/** The number of Services in the array */
	uint32 sa_num_services;	
} ServicesArray;





#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_SERVICE_API ServicesArray *GetServicesFromPlugin (Plugin * const plugin_p, const json_t *service_config_p);



GRASSROOTS_SERVICE_API void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (Service *service_p),
	const char *(*get_service_description_fn) (Service *service_p),
	const char *(*se_get_service_info_uri_fn) (struct Service *service_p),
	struct ServiceJobSet *(*run_fn) (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p),
	ParameterSet *(*match_fn) (Service *service_p, Resource *resource_p, Handler *handler_p),
	ParameterSet *(*get_parameters_fn) (Service *service_p, Resource *resource_p, const json_t *json_p),
	void (*release_parameters_fn) (Service *service_p, ParameterSet *params_p),
	bool (*close_fn) (struct Service *service_p),
	json_t *(*get_results_fn) (struct Service *service_p, const uuid_t service_id),
	OperationStatus (*get_status_fn) (Service *service_p, const uuid_t service_id),
 	bool specific_flag,
	bool synchronous_flag,
	ServiceData *data_p);


/**
 * @brief Run a Service.
 *
 * This is a convenience wrapper around se_run_fn.
 *
 * @param service_p The Service to run.
 * @param param_set_p The ParameterSet to run the Service with.
 * @param credentials_p An optional set of UserDetails as json.
 * @return A newly-allocated ServiceJobSet containing the details for the new jobs or
 * <code>NULL</code> upon error.
 * @memberof Service
 * @see se_run_fn
 */
GRASSROOTS_SERVICE_API struct ServiceJobSet *RunService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);


/**
 * Get a Service by its name.
 *
 * This will match a Service with its reponse from getting GetServiceName.
 *
 * @param service_name_s The name of the Service to find.
 * @return The matching Service or <code>NULL</code> if it could not be found.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API Service *GetServiceByName (const char * const service_name_s);


GRASSROOTS_SERVICE_API ParameterSet *IsServiceMatch (Service *service_p, Resource *resource_p, Handler *handler_p);


/**
 * Get the user-friendly name of the service.
 *
 * @param service_p The Service to get the name for.
 * @return The name of Service.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API const char *GetServiceName (Service *service_p);


/**
 * Get the user-friendly description of the service.
 *
 * @param service_p The Service to get the description for.
 * @return The description of Service.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API const char *GetServiceDescription (Service *service_p);



/**
 * Get the address of a web page about the service.
 *
 * @param service_p The Service to get the description for.
 * @return The address of the page or NULL if there isn't one.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API const char *GetServiceInformationURI (Service *service_p);

/**
 * Get a newly-created ParameterSet describing the parameters for a given Service.
 *
 * @param service_p The Service to get the ParameterSet for.
 * @param resource_p This is the input to the Service and can be NULL.
 * @param json_p Any configuration details that the Service might need. This can be NULL.
 * @return The newly-created ParameterSet or <code>NULL</code> upon error. This
 * ParameterSet will need to be freed once it is no longer needed by calling FreeParameterSet.
 * @see FreeParameterSet.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API ParameterSet *GetServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);


/**
 * Get the unique id of a service object.
 *
 * @param service_p The Service to get the id for.
 * @return The string of the id.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API char *GetServiceUUIDAsString (Service *service_p);


/**
 * Free a Service and its associated Parameters and ServiceData.
 *
 * @param service_p The Service to free.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API void FreeService (Service *service_p);


/**
 * @brief Allocate a ServiceNode pointing to the given Service.
 *
 * @param service_p The Service to store on the ServiceNode.
 * @return A newly-allocated ServiceNode or <code>NULL</code> upon error.
 * @memberof ServiceNode
 */
GRASSROOTS_SERVICE_API ServiceNode *AllocateServiceNode (Service *service_p);


/**
 * @brief Free a ServiceNode.
 *
 * @param node_p The ServiceNode to free.
 * @memberof ServiceNode
 */
GRASSROOTS_SERVICE_API void FreeServiceNode (ListItem *node_p);


/**
 * Load the Service that matches a given service name
 *
 * @param services_p The List of Services that the named Services will get appended to if it is found succesfully.
 * @param services_path_s The directory where the Service modules are stored.
 * @param services_name_s The name of the Service to find.
 * @param json_config_p Any runtime user-based configuration data. This can be <code>NULL</code>.
 */
GRASSROOTS_SERVICE_API void LoadMatchingServicesByName (LinkedList *services_p, const char * const services_path_s, const char *service_name_s, const json_t *json_config_p);

GRASSROOTS_SERVICE_API void LoadMatchingServices (LinkedList *services_p, const char * const services_path_s, Resource *resource_p, Handler *handler_p, const json_t *json_config_p);


/**
 * Load all Services that can be run upon a keyword parameter.
 *
 * @param services_p The List of Services that any keyword-aware Services will get appended to.
 * @param services_path_s The directory where the Service modules are stored.
 * @param json_config_p Any runtime user-based configuration data. This can be <code>NULL</code>.
 */
GRASSROOTS_SERVICE_API void LoadKeywordServices (LinkedList *services_p, const char * const services_path_s, const json_t *json_config_p);



GRASSROOTS_SERVICE_API void AddReferenceServices (LinkedList *services_p, const char * const references_path_s, const char * const services_path_s, const char *operation_name_s, const json_t *config_p);


/**
 * @brief Close a Service
 *
 * @param service_p The Service to close.
 * @return <code>true</code> if the Service was closed successfully, <code>false</code> otherwise.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API bool CloseService (Service *service_p);


/**
 * Has the Service got any jobs still running?
 *
 * @param service_p The Service to check.
 * @return <code>true</code> if the Service still has active jobs, <code>false</code> otherwise.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API  bool IsServiceLive (Service *service_p);


/**
 * Get the results from a long running service
 *
 * @param service_p The Service to get the results for
 * @return The results or NULL if they are not any.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API json_t *GetServiceResults (Service *service_p, const uuid_t service_id);

/**
 * Generate a json-based description of a Service. This uses the Swagger definitions
 * as much as possible.
 *
 * @param service_p The Service to generate the description for.
 * @return The json-based representation of the Service or <code>NULL</code> if there was
 * an error.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API json_t *GetServiceAsJSON (Service * const service_p, Resource *resource_p, const json_t *json_p, const bool add_id_flag);


/**
 * @brief Get the description of a Service
 *
 * @param root_p The json_t representation of a Service.
 * @return The description or <code>NULL</code> if it could not be found.
 */
GRASSROOTS_SERVICE_API const char *GetServiceDescriptionFromJSON (const json_t * const root_p);


/**
 * @brief Get the name of a Service
 *
 * @param root_p The json_t representation of a Service.
 * @return The name or <code>NULL</code> if it could not be found.
 */
GRASSROOTS_SERVICE_API const char *GetServiceNameFromJSON (const json_t * const root_p);


/**
 * @brief Get the description of an Operation
 *
 * @param root_p The json_t representation of a Service.
 * @return The description or <code>NULL</code> if it could not be found.
 */
GRASSROOTS_SERVICE_API const char *GetOperationDescriptionFromJSON (const json_t * const root_p);


/**
 * @brief Get the name of an Operation
 *
 * @param root_p The json_t representation of a Service.
 * @return The name or <code>NULL</code> if it could not be found.
 */
GRASSROOTS_SERVICE_API const char *GetOperationNameFromJSON (const json_t * const root_p);


/**
 * @brief Get the URI of an Operation
 *
 * @param root_p The json_t representation of a Service.
 * @return The URI or <code>NULL</code> if it could not be found.
 */
GRASSROOTS_SERVICE_API const char *GetOperationInformationURIFromJSON (const json_t * const root_p);


/**
 * @brief Get the path to an icon for a Service.

 * @param root_p The json_t representation of a Service.
 * @return The path to the icon or <code>NULL</code> if it could not be found.
 */
GRASSROOTS_SERVICE_API const char *GetIconPathFromJSON (const json_t * const root_p);



GRASSROOTS_SERVICE_API bool DeallocatePluginService (Plugin * const plugin_p);



GRASSROOTS_SERVICE_API json_t *GetServicesListAsJSON (LinkedList *services_list_p, Resource *resource_p, const json_t *json_p, const bool add_service_ids_flag);


/**
 * @brief Free a ParameterSet that was got from a call to <code>GetServiceParameters</code>.
 *
 * @param service_p The Service used for the previous call to <code>GetServiceParameters</code>
 * @param params_p The ParameterSet to free.s
 * @memberof Service
 * @see GetServiceParameters
 */
GRASSROOTS_SERVICE_API void ReleaseServiceParameters (Service *service_p, ParameterSet *params_p);


/**
 * Free a ServicesArray and each of its Services.
 *
 * @param services_p The ServicesArray to free.
 * @memberof ServicesArray
 */
GRASSROOTS_SERVICE_API void FreeServicesArray (ServicesArray *services_p);


/**
 * Allocate an empty ServicesArray.
 *
 * @param num_services The number of potential Services that the ServicesArray will hold.
 * @return A newly-allocated ServicesArray with space for the given number of Services or
 * <code>NULL</code> upon error.
 * @memberof ServicesArray
 */
GRASSROOTS_SERVICE_API ServicesArray *AllocateServicesArray (const uint32 num_services);



GRASSROOTS_SERVICE_LOCAL void AssignPluginForServicesArray (ServicesArray *services_p, Plugin *plugin_p);


GRASSROOTS_SERVICE_API bool AddServiceResponseHeader (Service *service_p, json_t *result_json_p);


GRASSROOTS_SERVICE_API ServicesArray *GetReferenceServicesFromJSON (json_t *config_p, const char *plugin_name_s, Service *(*get_service_fn) (json_t *config_p, size_t i));


/**
 * @brief Get the OperationStatus for an operation in a Service.
 *
 * @param service_p The Service to query.
 * @param service_id The uuid_t for the Operation whose OperationStatus is wanted.
 * @return The OperationStatus for the given Operation
 * @memberof Service
 */
GRASSROOTS_SERVICE_API OperationStatus GetCurrentServiceStatus (Service *service_p, const uuid_t service_id);


GRASSROOTS_SERVICE_LOCAL OperationStatus DefaultGetServiceStatus (Service *service_p, const uuid_t service_id);



GRASSROOTS_SERVICE_API const json_t *GetProviderFromServiceJSON (const json_t *service_json_p);



/**
 * Create a new PairedService and add it to a Service.
 *
 * @param service_p The Service to add the PairedService to.
 * @param external_server_p The ExternalServer that the PairedService runs on.
 * @param remote_service_name_s The name of the PairedService on the ExternalServer.
 * @param op_p The JSON fragment to create the PairedService's ParameterSet from.
 * @return <code>true</code> if the PairedService was created and added successfully, <code>false</code> otherwise.
 * @see AddPairedService.
 * @see AllocatePairedService.
 * @see CreateParameterSetFromJSON
 * @memberof Service.
 */
GRASSROOTS_SERVICE_API bool CreateAndAddPairedService (Service *service_p, struct ExternalServer *external_server_p, const char *remote_service_name_s, const json_t *op_p);


/**
 * Add a PairedService to a Service.
 *
 * @param service_p The Service to add the PairedService to.
 * @param paired_service_p The PairedService to add.
 * @return <code>true</code> if the PairedService was added successfully, <code>false</code> otherwise.
 * @memberof Service
 */
GRASSROOTS_SERVICE_API bool AddPairedService (Service *service_p, PairedService *paired_service_p);


/**
 * Get the JSON fragment for sending to a Grassroots Server detailing whether and how a user
 * wants a Service to run. This object needs to be within a JSON array which is what the Server
 * requires.
 *
 * @param service_p The Service to get the JSON fragment for.
 * @param params_p The ParameterSet to use. If run_flag is false, then this can be NULL.
 * @param run_flag Whether the Service should be run or not.
 * @return The JSON fragment to be added to an array to send to the Server or <code>NULL
 * </code> upon error.
 */
GRASSROOTS_SERVICE_API json_t *GetServiceRunRequest (const char * const service_name_s, const ParameterSet *params_p, const bool run_flag);



/**
 * Get the JSON fragment for a job result list registering that a Service
 * could be run against the user's parameters.
 *
 * This is of use when running a keyword search and specifying that a Service
 * knows of this keyword but cannot run on it directly. For example, a Blast
 * Service might know that the keyword refers to some if its databases.
 *
 * @param service_name_s The name of the Service that has registered an interest in the given keyword.
 * @param keyword_s The keyword.
 * @param params_p The parameters to encode within the JSON fragment. This will be set up based upon the
 * keyword used to check whether the Service was interested.
 * @return The JSON fragment to send to the Server or <code>NULL</code> upon error.
 * @see IsServiceMatch
 */
GRASSROOTS_SERVICE_API json_t *GetInterestedServiceJSON (const char *service_name_s, const char *keyword_s, const ParameterSet * const params_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_SERVICE_H */
