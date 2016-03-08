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
#ifndef SERVICE_MATCHER_H
#define SERVICE_MATCHER_H

#include "grassroots_service_library.h"
#include "data_resource.h"
#include "handler.h"
#include "service.h"

struct ServiceMatcher;


/** A callback function used to run a ServiceMatcher. */
typedef bool (*RunServiceMatcherCallback) (struct ServiceMatcher *matcher_p, Service *service_p);

/** A callback function used to free a ServiceMatcher. */
typedef void (*DestroyServiceMatcherCallback) (struct ServiceMatcher *matcher_p);


/**
 * @brief A datatype used to find any Services that match given criteria.
 */
typedef struct ServiceMatcher
{
	/**
	 * Callback function to run against a given Service to see if is a match.
	 *
	 * @see RunServiceMatcher
	 */
	RunServiceMatcherCallback sm_match_fn;

	/**
	 * @brief Callback to call when destroying a custom ServiceMatcher.
	 * @param matcher_p The ServiceMatcher to destroy.
	 * @see FreeServiceMatcher
	 */
	DestroyServiceMatcherCallback sm_destroy_fn;
} ServiceMatcher;


/**
 * @brief A ServiceMatcher that matches a Service if it can run on a given Resource.
 *
 * @extends ServiceMatcher
 */
typedef struct ResourceServiceMatcher
{
	/** The base matcher */
	ServiceMatcher rsm_base_matcher;

	/** The Resource to test if a Service can operate upon. */
	Resource *rsm_resource_p;

	/** The Handler to access the Resource. */
	Handler *rsm_handler_p;
} ResourceServiceMatcher;


/**
 * @brief A ServiceMatcher that matches a Service by its name.
 *
 * @extends ServiceMatcher
 */
typedef struct NameServiceMatcher
{
	/** The base matcher */
	ServiceMatcher nsm_base_matcher;

	/** The name of the Service to match */
	const char *nsm_service_name_s;
} NameServiceMatcher;


/**
 * @brief A ServiceMatcher that matches a Service by its Plugin name
 *
 * Find a Service by matching the name of the Plugin that it was loaded from.
 * This is used when working with a ReferredService.
 *
 * @extends ServiceMatcher
 */
typedef struct PluginNameServiceMatcher
{
	/** The base matcher */
	ServiceMatcher pnsm_base_matcher;

	/** The name of the Service's Plugin to match */
	const char *pnsm_plugin_name_s;
} PluginNameServiceMatcher;


/**
 * @brief A ServiceMatcher that matches a Service by its Plugin name and an Service name.
 *
 * Find a Service by matching the name of the Plugin that it was loaded from as well as having
 * a matching Service name too.
 *
 * @extends ServiceMatcher
 */
typedef struct PluginOperationNameServiceMatcher
{
	/** The base matcher for matching by Plugin name */
	PluginNameServiceMatcher ponsm_base_matcher;

	/** The Service name to match */
	const char *ponsm_operation_name_s;
} PluginOperationNameServiceMatcher;


/**
 * @brief A ServiceMatcher that will find any Service with a keyword parameter.
 *
 * @extends ServiceMatcher
 */
typedef struct KeywordServiceMatcher
{
	ServiceMatcher ksm_base_matcher;
} KeywordServiceMatcher;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate an empty ServiceMatcher.
 *
 * @return A newly-allocated ServiceMatcher or <code>NULL</code> upon error.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API ServiceMatcher *AllocateServiceMatcher (void);

/**
 * Allocate a ResourceServiceMatcher.
 *
 * @return A newly-allocated ServiceMatcher or <code>NULL</code> upon error.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API ServiceMatcher *AllocateResourceServiceMatcher (Resource *resource_p, Handler *handler_p);


/**
 * Allocate a NameServiceMatcher.
 *
 * @param name_s The name of the Service to match.
 * @return A newly-allocated NameServiceMatcher or <code>NULL</code> upon error.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API ServiceMatcher *AllocateOperationNameServiceMatcher (const char *name_s);


/**
 * Allocate a PluginNameServiceMatcher.
 *
 * @param plugin_name_s The name of the Plugin for a Service to match.
 * @return A newly-allocated ServiceMatcher or <code>NULL</code> upon error.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API ServiceMatcher *AllocatePluginNameServiceMatcher (const char *plugin_name_s);


/**
 * Allocate a PluginOperationNameServiceMatcher.
 *
 * @param plugin_name_s The name of the Plugin for a Service to match.
 * @param service_name_s The name of the Service to match.
 * @return A newly-allocated ServiceMatcher or <code>NULL</code> upon error.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API ServiceMatcher *AllocatePluginOperationNameServiceMatcher (const char *plugin_name_s, const char *service_name_s);


/**
 * Allocate a KeywordServiceMatcher.
 *
 * @return A newly-allocated ServiceMatcher or <code>NULL</code> upon error.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API ServiceMatcher *AllocateKeywordServiceMatcher (void);


/**
 * Initialise a ServiceMatcher.
 *
 * @param matcher_p The ServiceMatcher to initialise.
 * @param match_fn The callback function to use for determining matching Services.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_LOCAL void InitServiceMatcher (ServiceMatcher *matcher_p, RunServiceMatcherCallback match_fn);


GRASSROOTS_SERVICE_LOCAL void InitResourceServiceMatcher (ResourceServiceMatcher *matcher_p, Resource *resource_p, Handler *handler_p);

GRASSROOTS_SERVICE_LOCAL void InitOperationNameServiceMatcher (NameServiceMatcher *matcher_p, const char *name_s);

GRASSROOTS_SERVICE_LOCAL void InitPluginNameServiceMatcher (PluginNameServiceMatcher *matcher_p, const char *plugin_name_s);


GRASSROOTS_SERVICE_LOCAL void InitPluginOperationNameServiceMatcher (PluginOperationNameServiceMatcher *matcher_p, const char *plugin_name_s, const char *operation_name_s);


GRASSROOTS_SERVICE_LOCAL void InitKeywordServiceMatcher (KeywordServiceMatcher *matcher_p);


/**
 * Set the Plugin name to match for a PluginNameServiceMatcher.
 *
 * @param matcher_p The PluginNameServiceMatcher to update.
 * @param plugin_name_s The Plugin name to search for.
 * @memberof PluginNameServiceMatcher
 */
GRASSROOTS_SERVICE_API void SetPluginNameForServiceMatcher (PluginNameServiceMatcher *matcher_p, const char *plugin_name_s);


/**
 * @brief Run a ServiceMatcher on a given Service.
 * @param matcher_p The ServiceMatcher to run
 * @param service_p The Service to check.
 * @return <code>true</code> if the Service was a match, <code>false</code> otherwise.
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API bool RunServiceMatcher (ServiceMatcher *matcher_p, Service *service_p);

GRASSROOTS_SERVICE_LOCAL bool MatchServiceByName (ServiceMatcher *matcher_p, Service *service_p);

GRASSROOTS_SERVICE_LOCAL bool MatchServiceByResource (ServiceMatcher *matcher_p, Service *service_p);

GRASSROOTS_SERVICE_LOCAL bool MatchServiceByPluginName (ServiceMatcher *matcher_p, Service *service_p);

GRASSROOTS_SERVICE_LOCAL bool MatchServiceByPluginAndOperationsName (ServiceMatcher *matcher_p, Service *service_p);

GRASSROOTS_SERVICE_LOCAL bool MatchServiceByKeyword (ServiceMatcher *matcher_p, Service *service_p);


/**
 * @brief Free a ServiceMatcher
 * @param matcher_p The ServiceMatcher to free
 * @memberof ServiceMatcher
 */
GRASSROOTS_SERVICE_API void FreeServiceMatcher (ServiceMatcher *matcher_p);

GRASSROOTS_SERVICE_LOCAL void FreeResourceServiceMatcher (ServiceMatcher *matcher_p);

GRASSROOTS_SERVICE_LOCAL void FreeNameServiceMatcher (ServiceMatcher *matcher_p);

GRASSROOTS_SERVICE_LOCAL void FreePluginNameServiceMatcher (ServiceMatcher *matcher_p);

GRASSROOTS_SERVICE_LOCAL void FreePluginOperationNameServiceMatcher (ServiceMatcher *matcher_p);

GRASSROOTS_SERVICE_LOCAL void FreeKeywordServiceMatcher (ServiceMatcher *matcher_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SERVICE_MATCHER_H */
