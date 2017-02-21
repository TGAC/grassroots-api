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
#include "grassroots_config.h"
#include "servers_pool.h"
#include "streams.h"
#include "json_util.h"
#include "service_config.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "json_tools.h"


static json_t *s_config_p = NULL;
static bool s_load_config_tried_flag = false;




static const json_t *GetConfig (void);
static const char *GetProviderElement (const char * const element_s);
static void InitSchemaVersionDetails (void);



bool InitConfig (void)
{
	InitSchemaVersionDetails ();

	return (GetConfig () != NULL);
}


bool DestroyConfig (void)
{
	bool success_flag = true;

	if (s_config_p)
		{
			WipeJSON (s_config_p);
		}

	return success_flag;
}


json_t *GetGlobalConfigValue (const char *key_s)
{
	json_t *value_p = NULL;
	const json_t *config_p = GetConfig ();

	if (config_p)
		{
			value_p = json_object_get (config_p, key_s);
		}

	return value_p;
}


json_t *GetGlobalServiceConfig (const char * const service_name_s, bool *alloc_flag_p)
{
	json_t *res_p = NULL;
	char *conf_s = NULL;
	char sep_s [2];

	*sep_s = GetFileSeparatorChar ();
	* (sep_s + 1) = '\0';

	*alloc_flag_p = false;

	conf_s = ConcatenateVarargsStrings ("config", sep_s, service_name_s, NULL);

	if (conf_s)
		{
			const char *root_path_s = GetServerRootDirectory ();
			char *full_config_path_s = MakeFilename (root_path_s, conf_s);

			if (full_config_path_s)
				{
					if (IsPathValid (full_config_path_s))
						{
							res_p = LoadJSONFile (full_config_path_s);

							if (res_p)
								{
									*alloc_flag_p = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load config filename for %s", full_config_path_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_FINEST, __FILE__, __LINE__, "Separate config file %s does not exist", full_config_path_s);
						}

					FreeCopiedString (full_config_path_s);
				}		/* if (full_config_path_s) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create full config filename for %s", conf_s);
				}

			FreeCopiedString (conf_s);
		}		/* if (conf_s) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create local config filename for %s", service_name_s);
		}

	if (!res_p)
		{
			const json_t *config_p = GetConfig ();

			if (config_p)
				{
					json_t *json_p = json_object_get (config_p, SERVICES_NAME_S);

					if (json_p)
						{
							if (json_is_object (json_p))
								{
									res_p = json_object_get (json_p, service_name_s);
								}		/* if (json_is_object (json_p)) */

						}		/* if (json_p) */

				}		/* if (config_p) */
			else
				{

				}

		}		/* if (!res_p) */


	return res_p;
}


void DisconnectFromExternalServers (void)
{

}

void ConnectToExternalServers (void)
{
	const json_t *config_p = GetConfig ();

	if (config_p)
		{
			json_t *servers_p = json_object_get (config_p, SERVERS_S);

			if (servers_p)
				{
					if (json_is_object (servers_p))
						{
							AddExternalServerFromJSON (servers_p);
						}		/* if (json_is_object (json_p)) */
					else if (json_is_array (servers_p))
						{
							size_t index;
							json_t *element_p;

							json_array_foreach (servers_p, index, element_p)
								{
									AddExternalServerFromJSON (element_p);
								}
						}

				}		/* if (servers_p) */

		}		/* if (s_config_p) */
}


const char *GetServerProviderName (void)
{
	return GetProviderElement (PROVIDER_NAME_S);
}


const char *GetServerProviderDescription (void)
{
	return GetProviderElement (PROVIDER_DESCRIPTION_S);
}


const char *GetServerProviderURI (void)
{
	return GetProviderElement (PROVIDER_URI_S);
}


const json_t *GetProviderAsJSON (void)
{
	const json_t *provider_p = NULL;
	const json_t *config_p = GetConfig ();

	if (config_p)
		{
			provider_p = json_object_get (config_p, SERVER_PROVIDER_S);
		}

	return provider_p;
}


const char *GetJobLoggingURI (void)
{
	const char *uri_s = NULL;
	const json_t *config_p = GetConfig ();

	if (config_p)
		{
			const json_t *jobs_p = GetCompoundJSONObject (config_p, "admin.jobs");

			if (jobs_p)
				{
					uri_s = GetJSONString (jobs_p, "uri");
				}
		}

	return uri_s;
}


bool IsServiceEnabled (const char *service_name_s)
{
	bool enabled_flag = true;
	const json_t *services_config_p = GetGlobalConfigValue (SERVICES_NAME_S);

	if (services_config_p)
		{
			const json_t *service_statuses_p = json_object_get (services_config_p, SERVICES_STATUS_S);

			if (service_statuses_p)
				{
					const json_t *service_p = json_object_get (service_statuses_p, service_name_s);

					GetJSONBoolean (service_statuses_p, SERVICES_STATUS_DEFAULT_S, &enabled_flag);

					if (service_p)
						{
							if (json_is_true (service_p))
								{
									enabled_flag = true;
								}
							else if (json_is_false (service_p))
								{
									enabled_flag = false;
								}

						}		/* if (service_p) */

				}		/* if (service_statuses_p) */

		}		/* if (services_config_p) */

	return enabled_flag;
}




static const json_t *GetConfig (void)
{
	if (!s_config_p)
		{
			if (!s_load_config_tried_flag)
				{
					const char *root_path_s = GetServerRootDirectory ();
					char *full_config_path_s = MakeFilename (root_path_s, "grassroots.config");

					if (full_config_path_s)
						{
							json_error_t error;
							s_config_p = json_load_file (full_config_path_s, 0, &error);

							if (!s_config_p)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load config from %s", full_config_path_s);
								}

							s_load_config_tried_flag = true;

							FreeCopiedString (full_config_path_s);
						}
				}
		}

	return s_config_p;
}


static const char *GetProviderElement (const char * const element_s)
{
	const char *result_s = NULL;
	const json_t *provider_p = GetProviderAsJSON ();

	if (provider_p)
		{
			result_s = GetJSONString (provider_p, element_s);
		}

	return result_s;
}



static void InitSchemaVersionDetails (void)
{
	uint32 major = CURRENT_SCHEMA_VERSION_MAJOR;
	uint32 minor = CURRENT_SCHEMA_VERSION_MINOR;
	const json_t *schema_p = GetGlobalConfigValue (SCHEMA_S);

	if (schema_p)
		{
			GetJSONInteger (schema_p, VERSION_MAJOR_S, (int *) &major);
			GetJSONInteger (schema_p, VERSION_MINOR_S, (int *) &minor);
		}

	SetSchemaVersionDetails (major, minor);
}
