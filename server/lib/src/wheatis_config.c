#include "wheatis_config.h"
#include "servers_pool.h"
#include "streams.h"
#include "json_util.h"

static json_t *s_config_p = NULL;


bool InitConfig (const char *filename_s)
{
	bool success_flag = false;
	json_error_t error;

	s_config_p = json_load_file (filename_s, 0, &error);

	if (s_config_p)
		{
			success_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to load config from %s", filename_s);
		}

	return success_flag;
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
	if (s_config_p)
		{
			json_t *servers_p = json_object_get (s_config_p, SERVERS_S);

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

