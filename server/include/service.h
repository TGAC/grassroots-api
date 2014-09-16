#ifndef WHEATIS_SERVICE_H
#define WHEATIS_SERVICE_H

#include "wheatis_service_manager_library.h"

#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "stream.h"

#include "jansson.h"


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
	struct Service *md_service_p;
} ServiceData;
 

typedef struct Service
{
	struct Plugin *se_plugin_p;
	
	int (*se_run_fn) (const char * const filename_s, ParameterSet *param_set_p);

	bool (*se_match_fn) (const char * const filename_s, Stream *stream_p);

 	/** Get the user-friendly name of the service. */
	const char *(*se_get_service_name_fn) (void);

	/** Get the user-friendly description of the service. */
	const char *(*se_get_service_description_fn) (void);

	ParameterSet * (*se_get_params_fn) (void);	
	
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

WHEATIS_SERVICE_MANAGER_API void InitialiseService (Service * const service_p,
	const char *(*get_service_name_fn) (void),
	const char *(*get_service_description_fn) (void),
	int (*run_fn) (const char * const filename_s, ParameterSet *param_set_p),
	bool (*match_fn) (const char * const filename_s, Stream *stream_p),
	ParameterSet *(*get_parameters_fn) (void),
	ServiceData *data_p);

WHEATIS_SERVICE_MANAGER_API int RunService (Service *service_p, const char * const filename_s, ParameterSet *param_set_p);

WHEATIS_SERVICE_MANAGER_API bool DoesFileMatchService (Service *service_p, const char * const filename_s, Stream *stream_p);

/** Get the user-friendly name of the service. */
WHEATIS_SERVICE_MANAGER_API const char *GetServiceName (const Service *service_p);

/** Get the user-friendly description of the service. */
WHEATIS_SERVICE_MANAGER_API const char *GetServiceDescription (const Service *service_p);

WHEATIS_SERVICE_MANAGER_API ParameterSet *GetServiceParameters (const Service *service_p);	



WHEATIS_SERVICE_MANAGER_API void FreeService (Service *service_p);


WHEATIS_SERVICE_MANAGER_API ServiceNode *AllocateServiceNode (Service *service_p);

WHEATIS_SERVICE_MANAGER_API void FreeServiceNode (ListItem *node_p);


WHEATIS_SERVICE_MANAGER_API LinkedList *LoadMatchingServices (const char * const services_path_s, const char * const pattern_s, Stream *stream_p);


WHEATIS_SERVICE_MANAGER_API json_t *GetServiceAsJSON (const Service * const service_p);


#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef WHEATIS_SERVICE_H */
