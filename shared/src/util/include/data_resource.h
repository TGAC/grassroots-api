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
#ifndef GRASSROOTS_RESOURCE_H
#define GRASSROOTS_RESOURCE_H

#include "jansson.h"

#include "grassroots_util_library.h"
#include "typedefs.h"

/**
 * @brief A datatype representing a URI.
 *
 * This represents a URI to a particular data object.
 * For example "http://grassroots.tgac.ac.uk" would have
 * a protocol of "http" and a value of "grassroots.tgac.ac.uk".
 * It can have a user-friendly title and an associated json
 * object if needed.
 */
typedef struct Resource
{
	/**
	 * The protocol of this Resource.
	 *
	 * @see PROTOCOL_IRODS_S
	 * @see PROTOCOL_FILE_S
	 * @see PROTOCOL_HTTP_S
	 * @see PROTOCOL_HTTPS_S
	 * @see PROTOCOL_INLINE_S
	 * @see PROTOCOL_SERVICE_S
	 * @see RESOURCE_DELIMITER_S
	 */
	char *re_protocol_s;

	/**
	 * The protocol-specific path to the data object.
	 */
	char *re_value_s;

	/**
	 * An optional user-friendly name for this Resource.
	 */
	char *re_title_s;

	/**
	 * An optional set of json-based data for any
	 * application-specific configuration details.
	 */
	json_t *re_data_p;

	/**
	 * Does this Resource own any data pointed
	 * to by re_data_p? If so, it will delete
	 * re_data_p when this Resource is freed.
	 */
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
	#define RESOURCE_PREFIX GRASSROOTS_UTIL_API
	#define RESOURCE_VAL(x)	= x
#else
	#define RESOURCE_PREFIX extern
	#define RESOURCE_VAL(x)
#endif

/**
 * @brief The string used to separate the protocol and values of a Resource.
 * This is used to build string representations of the form
 * \<PROTOCOL\>\<RESOURCE_DELIMITER_S\>\<VALUE\>
 * @memberof Resource
 */
RESOURCE_PREFIX const char *RESOURCE_DELIMITER_S RESOURCE_VAL("://");

/**
 * @brief The protocol for a data object on an iRODS system.
 * @memberof Resource
 */
RESOURCE_PREFIX const char *PROTOCOL_IRODS_S RESOURCE_VAL("irods");

/**
 * @brief The protocol for a data object for a mounted file.
 * @memberof Resource
 */
RESOURCE_PREFIX const char *PROTOCOL_FILE_S RESOURCE_VAL("file");

/**
 * @brief The protocol for a data object for a web address.
 * @memberof Resource
 */
RESOURCE_PREFIX const char *PROTOCOL_HTTP_S RESOURCE_VAL("http");

/**
 * @brief The protocol for a data object for a secure web address.
 * @memberof Resource
 */
RESOURCE_PREFIX const char *PROTOCOL_HTTPS_S RESOURCE_VAL("https");

/**
 * @brief The protocol for a data object stored within the Grassroots system inline.
 * @memberof Resource
 */
RESOURCE_PREFIX const char *PROTOCOL_INLINE_S RESOURCE_VAL("inline");


/**
 * @brief The protocol for a data object stored within the Grassroots system inline.
 * @memberof Resource
 */
RESOURCE_PREFIX const char *PROTOCOL_TEXT_S RESOURCE_VAL("text");


/**
 * @brief The protocol for a data object for a Grassroots Service.
 * @memberof Resource
 */
RESOURCE_PREFIX const char *PROTOCOL_SERVICE_S RESOURCE_VAL("service");



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
GRASSROOTS_UTIL_API Resource *AllocateResource (const char *protocol_s, const char *value_s, const char *title_s);


/**
 * @brief Initialise a Resource ready for use.
 *
 * @param resource_p The Resource to initialise.
 * @memberof Resource
 */
GRASSROOTS_UTIL_API void InitResource (Resource *resource_p);


/**
 * @brief Free a Resource and its associated data.
 *
 * @param resource_p The Resource to free.
 * @memberof Resource
 */
GRASSROOTS_UTIL_API void FreeResource (Resource *resource_p);


/**
 * @brief Clear a Resource.
 *
 * Clear and free any memory used for the Resource's fields
 * and set them to <code>NULL</code>
 * @param resource_p The Resource to clear.
 * @memberof Resource
 */
GRASSROOTS_UTIL_API void ClearResource (Resource *resource_p);


/**
 * @brief Set the fields of a Resource.
 *
 * @param resource_p The Resource to be updated.
 * @param protocol_s The new value for the protocol for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope. Any previous value will be freed.
 * @param value_s The new value for the value for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope. Any previous value will be freed.
 * @param title_s The new value for the title for the Resource. A deep copy will be made of this
 * so the value passed in dose not need to remain in scope. Any previous value will be freed.
 * @return <code>true</code> if the Resource was updated successfully, <code>false</code> otherwise.
 * @memberof Resource
 */
GRASSROOTS_UTIL_API bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s, const char *title_s);


/**
 * @brief Set the json_t object of a Resource.
 *
 * @param resource_p The Resource to set the data for.
 * @param data_p The json_t object to add.
 * @param owns_data_flag If this is <code>true</code>, then when this Resource is freed it will clear and free
 * the given json_t object.
 * @return <code>true</code> if the Resource was updated successfully, <code>false</code> otherwise.
 * @memberof Resource
 */
GRASSROOTS_UTIL_API bool SetResourceData (Resource *resource_p, json_t *data_p, const bool owns_data_flag);


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
GRASSROOTS_UTIL_API bool CopyResource (const Resource * const src_p, Resource * const dest_p);


/**
 * @brief Clone the data from one Resource to another.
 *
 * This will make new Resource and make a deep copy of each field in the source Resource into the
 * equivalent field in the destination Resource.
 * @param src_p The Resource to copy from.
 * @return <code>true</code> if all of the fields were copied correctly, <code>false</code>
 * otherwise. Upon failure, the destination Resource will remain unaltered.
 * @memberof Resource
 */
GRASSROOTS_UTIL_API Resource *CloneResource (const Resource * const src_p);


/**
 * @brief Create a Resource from a given string.
 *
 * This will take a string of the form \<PROTOCOL\>://\<PATH\> to generate
 * a Resource.
 *
 * @param resource_s The string representation of the Resource.
 * @return A newly-allocated Resource or <code>NULL</code>
 * upon failure.
 * @memberof Resource
 * @see PROTOCOL_IRODS_S
 * @see PROTOCOL_FILE_S
 * @see PROTOCOL_HTTP_S
 * @see PROTOCOL_HTTPS_S
 * @see PROTOCOL_INLINE_S
 * @see RESOURCE_DELIMITER_S
 */
GRASSROOTS_UTIL_API Resource *ParseStringToResource (const char * const resource_s);


/**
 * @brief Get the protocol and path parts of a Resource.
 *
 * @param resource_s The Resource to get the values from.
 * @param path_ss A pointer to where the path part of the Resource will be copied to. This will
 * need to be freed with <code>FreeCopiedString</code> when it is finished with.
 * @param protocol_ss A pointer to where the protocol part of the Resource will be copied to. This will
 * need to be freed with <code>FreeCopiedString</code> when it is finished with.
 * @return <code>true</code> if all of the fields were copied correctly, <code>false</code>
 * otherwise. Upon failure, the destination path_ss and protocol_ss variables will remain unaltered.
 * @memberof Resource
 * @see FreeCopiedString
 */
GRASSROOTS_UTIL_API bool GetResourceProtocolAndPath (const char * const resource_s, char ** const path_ss, char ** const protocol_ss);


/**
 * @brief Get the json representation of a Resource's fields.
 *
 * @param protocol_s The protocol of the Resource.
 * @param path_s The path of the Resource.
 * @param title_s The title of the Resource.
 * @param data_p The associated json data of the Resource.
 * @return The json_t object representing the Resource or <code>NULL</code>
 * upon failure.
 * @memberof Resource
 */
GRASSROOTS_UTIL_API json_t *GetResourceAsJSONByParts (const char * const protocol_s, const char * const path_s, const char * const title_s, json_t *data_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef GRASSROOTS_RESOURCE_H */
