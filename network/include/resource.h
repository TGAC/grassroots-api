#ifndef WHEATIS_RESOURCE_H
#define WHEATIS_RESOURCE_H

#include "jansson.h"

#include "network_library.h"
#include "typedefs.h"

typedef struct Resource
{
	char *re_protocol_s;

	char *re_value_s;

	char *re_title_s;

	json_t *re_data_p;

	bool re_owns_data_flag;
} Resource;

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
 * ALLOCATE_RESOURCE_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * resource.c.
 */
#ifdef ALLOCATE_RESOURCE_TAGS
	#define RESOURCE_PREFIX WHEATIS_NETWORK_API
	#define RESOURCE_VAL(x)	= x
#else
	#define RESOURCE_PREFIX extern
	#define RESOURCE_VAL(x)
#endif

RESOURCE_PREFIX const char *RESOURCE_DELIMITER_S RESOURCE_VAL("://");

RESOURCE_PREFIX const char *PROTOCOL_IRODS_S RESOURCE_VAL("irods");
RESOURCE_PREFIX const char *PROTOCOL_FILE_S RESOURCE_VAL("file");
RESOURCE_PREFIX const char *PROTOCOL_HTTP_S RESOURCE_VAL("http");
RESOURCE_PREFIX const char *PROTOCOL_HTTPS_S RESOURCE_VAL("https");
RESOURCE_PREFIX const char *PROTOCOL_INLINE_S RESOURCE_VAL("inline");


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Allocate a Resource.
 *
 * @param protocol_s The protocol for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope.
 * @param value_s The value for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope.
 * @param title_s The title for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope.
 * @return A newly-allocated Resource or <code>NULL</code> upon error.
 * @memberof Resource
 */
WHEATIS_NETWORK_API Resource *AllocateResource (const char *protocol_s, const char *value_s, const char *title_s);


/**
 * @brief Initialise a Resource ready for use.
 *
 * @param resource_p The Resource to initialise.
 * @memberof Resource
 */
WHEATIS_NETWORK_API void InitResource (Resource *resource_p);


/**
 * @brief Free a Resource and its associated data.
 *
 * @param resource_p The Resource to free.
 * @memberof Resource
 */
WHEATIS_NETWORK_API void FreeResource (Resource *resource_p);


/**
 * @brief Clear a Resource.
 *
 * Clear and free any memory used for the Resource's fields
 * and set them to <code>NULL</code>
 * @param resource_p The Resource to clear.
 * @memberof Resource
 */
WHEATIS_NETWORK_API void ClearResource (Resource *resource_p);


/**
 * @brief Set the fields of a Resource.
 *
 * @param protocol_s The new value for the protocol for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope. Any previous value will be freed.
 * @param value_s The new value for the value for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope. Any previous value will be freed.
 * @param title_s The new value for the title for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope. Any previous value will be freed.
 * @memberof Resource
 */
WHEATIS_NETWORK_API bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s, const char *title_s);


WHEATIS_NETWORK_API bool SetResourceData (Resource *resource_p, json_t *data_p, const bool owns_data_flag);


/**
 * @brief Copy the data from one Resource to another.
 *
 * This will make a deep copy of each field in the source Resource into the
 * equivalent field in the destination Resource.
 * @param src_p The Resource to copy from.
 * @param dest_p The Resource to copy to.
 * @return <code>true</code> if all of the fields were copied correctly, <code>false</code>
 * otherwise. Upon failure, the destination Resource will remain unaltered.
 * @memberof Resource
 */
WHEATIS_NETWORK_API bool CopyResource (const Resource * const src_p, Resource * const dest_p);


WHEATIS_NETWORK_API Resource *ParseStringToResource (const char * const resource_s);


WHEATIS_NETWORK_API bool GetResourceProtocolAndPath (const char * const resource_s, char ** const path_ss, char ** const protocol_ss);


WHEATIS_NETWORK_API json_t *GetResourceAsJSONByParts (const char * const protocol_s, const char * const path_s, const char * const title_s, json_t *data_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef WHEATIS_RESOURCE_H */
