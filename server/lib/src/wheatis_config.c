#include "wheatis_config.h"
#include "servers_pool.h"
#include "streams.h"
#include "json_util.h"

static json_t *s_config_p = NULL;
static bool s_load_config_tried_flag = false;


static const json_t *GetConfig (void);


bool InitConfig (void)
{
	return (GetConfig () != NULL);
}


bool DestroyConfig (void)
{
	bool success_flag = true;

	if (s_config_p)
		{
			success_flag = (json_object_clear (s_config_p) == 0);

			json_decref (s_config_p);
		}

	return success_flag;
}


const json_t *GetGlobalConfigValue (const char *key_s)
{
	const json_t *value_p = NULL;

	if (s_config_p)
		{
			value_p = json_object_get (s_config_p, key_s);
		}

	return value_p;
}


const json_t *GetGlobalServiceConfig (const char * const service_name_s)
{
	const json_t *res_p = NULL;

	if (s_config_p)
		{
			json_t *json_p = json_object_get (s_config_p, SERVICES_NAME_S);

			if (json_p)
				{
					if (json_is_object (json_p))
						{
							res_p = json_object_get (json_p, service_name_s);
						}		/* if (json_is_object (json_p)) */

				}		/* if (json_p) */

		}		/* if (s_config_p) */

	return res_p;
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


static const json_t *GetConfig (void)
{
	if (!s_config_p)
		{
			if (!s_load_config_tried_flag)
				{
					const char *root_path_s = GetServerRootDirectory ();
					char *full_config_path_s = MakeFilename (root_path_s, "wheatis.config");

					if (full_config_path_s)
						{
							json_error_t error;
							s_config_p = json_load_file (full_config_path_s, 0, &error);

							if (!s_config_p)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to load config from %s", full_config_path_s);
								}

							s_load_config_tried_flag = true;

							FreeCopiedString (full_config_path_s);
						}
				}
		}

	return s_config_p;
}

