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
/* Include the required headers from httpd */
#include <unistd.h>

#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_config.h"
#include "http_log.h"

#include "apr_strings.h"
#include "apr_network_io.h"
#include "apr_md5.h"
#include "apr_sha1.h"
#include "apr_hash.h"
#include "apr_base64.h"
#include "apr_dbd.h"
#include "apr_file_info.h"
#include "apr_file_io.h"
#include "apr_tables.h"
#include "util_script.h"


#include "key_value_pair.h"
#include "server.h"
#include "jansson.h"
#include "service_config.h"
#include "system_util.h"
#include "streams.h"
#include "apache_output_stream.h"
#include "util_mutex.h"

#include "mod_grassroots_config.h"
#include "apr_jobs_manager.h"
#include "apr_servers_manager.h"

#ifdef _DEBUG
	#define MOD_GRASSROOTS_DEBUG	(STM_LEVEL_FINE)
#else
	#define MOD_GRASSROOTS_DEBUG	(STM_LEVEL_NONE)
#endif



/* Define prototypes of our functions in this module */
static void RegisterHooks (apr_pool_t *pool_p);
static int GrassrootsHandler (request_rec *req_p);
static void GrassrootsChildInit (apr_pool_t *pool_p, server_rec *server_p);

static int GrassrootsPreConfig (apr_pool_t *config_pool_p, apr_pool_t *log_pool_p, apr_pool_t *temp_pool_p);
static int GrassrootsPostConfig (apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);

static const char *SetGrassrootsRootPath (cmd_parms *cmd_p, void *cfg_p, const char *arg_s);
static const char *SetGrassrootsCacheProvider (cmd_parms *cmd_p, void *cfg_p, const char *arg_s);

static void *CreateServerConfig (apr_pool_t *pool_p, server_rec *server_p);

static void *MergeServerConfig (apr_pool_t *pool_p, void *base_config_p, void *vhost_config_p);


static void *CreateDirectoryConfig (apr_pool_t *pool_p, char *context_s);
static void *MergeDirectoryConfig (apr_pool_t *pool_p, void *base_config_p, void *new_config_p);


static ModGrassrootsConfig *CreateConfig (apr_pool_t *pool_p, server_rec *server_p);

static apr_status_t CloseInformationSystem (void *data_p);

static apr_status_t CleanUpOutputStream (void *value_p);

static apr_status_t ClearServerResources (void *value_p);


/*
 * Based on code taken from http://marc.info/?l=apache-modules&m=107669698011831
 * sander@temme.net              http://www.temme.net/sander/
 */
static apr_status_t CleanUpPool (void *data_p);
static int PoolDebug (apr_pool_t *config_pool_p, apr_pool_t *log_pool_p, apr_pool_t *temp_pool_p, server_rec *server_p);


static const command_rec s_grassroots_directives [] =
{
	AP_INIT_TAKE1 ("GrassrootsCache", SetGrassrootsCacheProvider, NULL, ACCESS_CONF, "The provider for the Jobs Cache"),
	AP_INIT_TAKE1 ("GrassrootsRoot", SetGrassrootsRootPath, NULL, ACCESS_CONF, "The path to the Grassroots installation"),
	{ NULL }
};


static APRJobsManager *s_jobs_manager_p = NULL;
static const char * const s_jobs_manager_cache_id_s = "grassroots-jobs-socache";

static APRServersManager *s_servers_manager_p = NULL;
static const char * const s_servers_manager_cache_id_s = "grassroots-servers-socache";


/* Define our module as an entity and assign a function for registering hooks  */
module AP_MODULE_DECLARE_DATA grassroots_module =
{
    STANDARD20_MODULE_STUFF,
    CreateDirectoryConfig,   	// Per-directory configuration handler
    MergeDirectoryConfig,   	// Merge handler for per-directory configurations
    CreateServerConfig,				// Per-server configuration handler
    MergeServerConfig,				// Merge handler for per-server configurations
    s_grassroots_directives,			// Any directives we may have for httpd
    RegisterHooks    					// Our hook registering function
};


const module *GetGrassrootsModule (void)
{
	return &grassroots_module;
}



/* register_hooks: Adds a hook to the httpd process */
static void RegisterHooks (apr_pool_t *pool_p)
{
	ap_hook_pre_config (GrassrootsPreConfig, NULL, NULL, APR_HOOK_MIDDLE);

	ap_hook_post_config (GrassrootsPostConfig, NULL, NULL, APR_HOOK_MIDDLE);

  ap_hook_child_init (GrassrootsChildInit, NULL, NULL, APR_HOOK_MIDDLE);

	ap_hook_handler (GrassrootsHandler, NULL, NULL, APR_HOOK_MIDDLE);
}



static int GrassrootsPreConfig (apr_pool_t *config_pool_p, apr_pool_t *log_pool_p, apr_pool_t *temp_pool_p)
{
	int res = 500;
	apr_status_t status = ap_mutex_register (config_pool_p, s_jobs_manager_cache_id_s, NULL, APR_LOCK_DEFAULT, 0);

	if (status == APR_SUCCESS)
		{
			status = ap_mutex_register (config_pool_p, s_servers_manager_cache_id_s, NULL, APR_LOCK_DEFAULT, 0);

			if (status == APR_SUCCESS)
				{
					res = OK;
				}		/* if (status == APR_SUCCESS) */
			else
				{
					ap_log_error (APLOG_MARK, APLOG_CRIT, status, NULL, "ap_mutex_register for s_servers_manager_cache_id_s failed");
				}

		}		/* if (status == APR_SUCCESS) */
	else
		{
			ap_log_error (APLOG_MARK, APLOG_CRIT, status, NULL, "ap_mutex_register for s_jobs_manager_cache_id_s failed");
		}

	return res;
}


static void *CreateServerConfig (apr_pool_t *pool_p, server_rec *server_p)
{
	return ((void *) CreateConfig (pool_p, server_p));
}


static void *CreateDirectoryConfig (apr_pool_t *pool_p, char *context_s)
{
	return ((void *) CreateConfig (pool_p, NULL));
}


static ModGrassrootsConfig *CreateConfig (apr_pool_t *pool_p, server_rec *server_p)
{
	ModGrassrootsConfig *config_p = apr_palloc (pool_p, sizeof (ModGrassrootsConfig));

	if (config_p)
		{
			config_p -> wisc_root_path_s = NULL;
			config_p -> wisc_server_p = server_p;
			config_p -> wisc_provider_name_s = "shmcb";
			config_p -> wisc_jobs_manager_p = NULL;
			config_p -> wisc_servers_manager_p = NULL;
		}

	return config_p;
}


static void *MergeDirectoryConfig (apr_pool_t *pool_p, void *base_config_p, void *new_config_p)
{
	/* currently ignore the vhosts config */
	return base_config_p;
}



static void *MergeServerConfig (apr_pool_t *pool_p, void *base_config_p, void *vhost_config_p)
{
	/* currently ignore the vhosts config */
	return base_config_p;
}


static apr_status_t CloseInformationSystem (void *data_p)
{
	DestroyInformationSystem ();
	return APR_SUCCESS;
}


static void GrassrootsChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
//	ModGrassrootsConfig *config_p = ap_get_module_config (server_p -> module_config, &grassroots_module);

	/* Now that we are in a child process, we have to reconnect
	 * to the global mutex and the shared segment. We also
	 * have to find out the base address of the segment, in case
	 * it moved to a new address. */
	if (APRJobsManagerChildInit (pool_p, server_p))
		{
			if (APRServersManagerChildInit (pool_p, server_p))
				{
					if (InitInformationSystem ())
						{
							OutputStream *log_p = AllocateApacheOutputStream (server_p);

							apr_pool_cleanup_register (pool_p, NULL, CloseInformationSystem, apr_pool_cleanup_null);

							if (log_p)
								{
									OutputStream *error_p = AllocateApacheOutputStream (server_p);

									SetDefaultLogStream (log_p);
									apr_pool_cleanup_register (pool_p, log_p, CleanUpOutputStream, apr_pool_cleanup_null);

									if (error_p)
										{
											/* Mark the streams for deletion when the server pool expires */
											//apr_pool_t *pool_p = server_p -> process -> pool;

											SetDefaultErrorStream (error_p);
											apr_pool_cleanup_register (pool_p, error_p, CleanUpOutputStream, apr_pool_cleanup_null);
										}
									else
										{
											FreeOutputStream (log_p);
											ap_log_error (APLOG_MARK, APLOG_CRIT, APR_EGENERAL , server_p, "AllocateApacheOutputStream for errors failed");
										}
								}
							else
								{
									ap_log_error (APLOG_MARK, APLOG_CRIT, APR_EGENERAL , server_p, "AllocateApacheOutputStream for log failed");
								}


						}		/* If (InitInformationSystem ()) */
					else
						{
							ap_log_error (APLOG_MARK, APLOG_CRIT, APR_EGENERAL , server_p, "InitInformationSystem failed");
						}

				}		/* if (APRServersManagerChildInit (pool_p, server_p)) */
			else
				{
					ap_log_error (APLOG_MARK, APLOG_CRIT, APR_EGENERAL , server_p, "APRServersManagerChildInit failed");
				}

		}		/* if (APRJobsManagerChildInit (pool_p, server_p)) */
	else
		{
			ap_log_error (APLOG_MARK, APLOG_CRIT, APR_EGENERAL, server_p, "APRJobsManagerChildInit failed");
		}

}


static apr_status_t ClearServerResources (void *value_p)
{
	FreeServerResources ();

	return APR_SUCCESS;
}

static int GrassrootsPostConfig (apr_pool_t *config_pool_p, apr_pool_t *log_p, apr_pool_t *temp_p, server_rec *server_p)
{
  void *data_p = NULL;
  const char *userdata_key_s = "grassroots_post_config";
  int ret = HTTP_INTERNAL_SERVER_ERROR;
  apr_pool_t *pool_p = config_pool_p;

  /* Apache loads DSO modules twice. We want to wait until the second
   * load before setting up our global mutex and shared memory segment.
   * To avoid the first call to the post_config hook, we set some
   * dummy userdata in a pool that lives longer than the first DSO
   * load, and only run if that data is set on subsequent calls to
   * this hook. */
  apr_pool_userdata_get (&data_p, userdata_key_s, server_p -> process -> pool);

  if (data_p == NULL)
  	{
      /* WARNING: This must *not* be apr_pool_userdata_setn(). The
       * reason for this is because the static symbol section of the
       * DSO may not be at the same address offset when it is reloaded.
       * Since setn() does not make a copy and only compares addresses,
       * the get() will be unable to find the original userdata. */
      apr_pool_userdata_set ((const void *) 1, userdata_key_s, apr_pool_cleanup_null, server_p -> process -> pool);

      ret = OK; /* This would be the first time through */
  	}
  else
  	{
  		/*
  		 * We are now in the parent process before any child processes have been started, so this is
  		 * where any global shared memory should be allocated
       */
  		ModGrassrootsConfig *config_p = (ModGrassrootsConfig *) ap_get_module_config (server_p -> module_config, &grassroots_module);

  		if (config_p -> wisc_provider_name_s)
  			{
  	  		config_p -> wisc_jobs_manager_p = InitAPRJobsManager (server_p, pool_p, config_p -> wisc_provider_name_s);

  	  		if (config_p -> wisc_jobs_manager_p)
  					{
  	  				s_jobs_manager_p = config_p -> wisc_jobs_manager_p;

							PostConfigAPRJobsManager (s_jobs_manager_p, pool_p, server_p, config_p -> wisc_provider_name_s);

							config_p -> wisc_servers_manager_p = InitAPRServersManager (server_p, pool_p, config_p -> wisc_provider_name_s);

							if (config_p -> wisc_servers_manager_p)
								{
									s_servers_manager_p = config_p -> wisc_servers_manager_p;

									PostConfigAPRServersManager (s_servers_manager_p, pool_p, server_p, config_p -> wisc_provider_name_s);

									PoolDebug (config_pool_p, log_p, temp_p, server_p);

									ret = OK;
								}
		  	  		else
		  	  			{
		  	  				ap_log_error (APLOG_MARK, APLOG_CRIT, ret, server_p, "Failed to create servers manager");
		  	  			}


  					}		/* if (config_p -> wisc_jobs_manager_p) */
  	  		else
  	  			{
  	  				ap_log_error (APLOG_MARK, APLOG_CRIT, ret, server_p, "Failed to create jobs manager");
  	  			}
  			}
  		else
  			{
  				ap_log_error (APLOG_MARK, APLOG_CRIT, ret, server_p, "You need to specify an socache module to load for Grassroots to work");
  			}
  	}


	apr_pool_cleanup_register (config_pool_p, NULL, ClearServerResources, apr_pool_cleanup_null);

	ap_log_error (APLOG_MARK, APLOG_DEBUG, ret, server_p, "GrassrootsPostConfig exiting");

  return ret;
}


static apr_status_t CleanUpOutputStream (void *value_p)
{
	OutputStream *stream_p = (OutputStream *) value_p;
	FreeOutputStream (stream_p);

	return APR_SUCCESS;
}


/* Handler for the "GrassrootsRoot" directive */
static const char *SetGrassrootsRootPath (cmd_parms *cmd_p, void *cfg_p, const char *arg_s)
{
	SetServerRootDirectory (arg_s);

	return NULL;
}


/* Get the cache provider that we are going to use for the jobs manager storage */
static const char *SetGrassrootsCacheProvider (cmd_parms *cmd_p, void *cfg_p, const char *arg_s)
{
	ModGrassrootsConfig *config_p = (ModGrassrootsConfig *) cfg_p;
  const char *err_msg_s = NULL; //ap_check_cmd_context (cmd_p, GLOBAL_ONLY);

  if (!err_msg_s)
  	{
  	  /* Argument is of form 'name:args' or just 'name'. */
  	  const char *sep_s = ap_strchr_c (arg_s, ':');

  	  if (sep_s)
  	  	{
  	  		config_p -> wisc_provider_name_s = apr_pstrmemdup (cmd_p -> pool, arg_s, sep_s - arg_s);
  	      ++ sep_s;
  	  	}
  	  else
  	  	{
  	  		config_p -> wisc_provider_name_s = apr_pstrdup (cmd_p -> pool, arg_s);
  	  	}

  	}		/* if (!err_msg_s)*/
  else
  	{
  		err_msg_s = apr_psprintf (cmd_p -> pool, "GrassrootsSOCache: %s", err_msg_s);
  	}

  if (err_msg_s)
  	{
  		ap_log_error (APLOG_MARK, APLOG_CRIT, APR_EGENERAL, NULL, "SetGrassrootsCacheProvider failed: \"%s\" from arg \"%s\"", err_msg_s, arg_s);
  	}

  return err_msg_s;
}



/* The handler function for our module.
 * This is where all the fun happens!
 */
static int GrassrootsHandler (request_rec *req_p)
{
	int res = DECLINED;

  /* First off, we need to check if this is a call for the grassroots handler.
   * If it is, we accept it and do our things, it not, we simply return DECLINED,
   * and Apache will try somewhere else.
   */
  if ((req_p -> handler) && (strcmp (req_p -> handler, "grassroots-handler") == 0))
  	{
  		if ((req_p -> method_number == M_GET) || (req_p -> method_number == M_POST))
  			{
  				/* Get the posted json data */
						json_t *json_req_p = GetAllRequestDataAsJSON (req_p);

						if (json_req_p)
							{
								int socket_fd = -1;
						    ModGrassrootsConfig *config_p = ap_get_module_config (req_p -> per_dir_config, &grassroots_module);
						    const char *error_s = NULL;
								json_t *res_p = ProcessServerJSONMessage (json_req_p, socket_fd, &error_s);

								if (res_p)
									{
										char *res_s = json_dumps (res_p, JSON_INDENT (2));

										if (res_s)
											{
												res = OK;

												ap_rputs (res_s, req_p);

												free (res_s);
											}		/* if (res_s) */
										else
											{
												res = HTTP_INTERNAL_SERVER_ERROR;
											}

										#if MOD_GRASSROOTS_DEBUG >= STM_LEVEL_FINER
										PrintJSONRefCounts (res_p, "pre decref res_p", STM_LEVEL_FINER, __FILE__, __LINE__);
										#endif

										json_decref (res_p);

									}		/* if (res_p) */
								else
									{
										ap_rprintf (req_p, "Error processing request: %s", error_s);
										res = HTTP_BAD_REQUEST;
									}

								#if MOD_GRASSROOTS_DEBUG >= STM_LEVEL_FINER
								PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "json_req_p -> refcount %ld", json_req_p -> refcount);
								#endif
								json_decref (json_req_p);
							}		/* if (json_req_p) */
						else
							{
								ap_rprintf (req_p, "Error getting input data from request");
								res = HTTP_BAD_REQUEST;
							}
  			}		/* if ((req_p -> method_number == M_GET) || (req_p -> method_number == M_POST)) */
  		else
  			{
  				res = HTTP_METHOD_NOT_ALLOWED;
  			}

  	}		/* if ((req_p -> handler) && (strcmp (req_p -> handler, "grassroots-handler") == 0)) */

  return res;
}




/*
 * Based on code taken from http://marc.info/?l=apache-modules&m=107669698011831
 * sander@temme.net              http://www.temme.net/sander/
 */

static apr_status_t CleanUpPool (void *data_p)
{
  //ap_log_error (APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, NULL, "Pool cleanup in process %d: %s\n", getpid (), (char *) data_p);
	fprintf (stdout, "Pool cleanup in process %d: %s\n", getpid (), (char *) data_p);
	fflush (stdout);

	return OK;
}


static int PoolDebug (apr_pool_t *config_pool_p, apr_pool_t *log_pool_p, apr_pool_t *temp_pool_p, server_rec *server_p)
{
  ap_log_error (APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p, "Registering for config_pool_p [%#lx]", (unsigned long int) config_pool_p);
	apr_pool_cleanup_register (config_pool_p, (void *) "Cleaning up config_pool_p", CleanUpPool, apr_pool_cleanup_null);

  ap_log_error (APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p, "Registering for log_pool_p [%#lx]", (unsigned long int) log_pool_p);
	apr_pool_cleanup_register (log_pool_p, (void *) "Cleaning up log_pool_p", CleanUpPool, apr_pool_cleanup_null);

  ap_log_error (APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p, "Registering for temp_pool_p [%#lx]", (unsigned long int) temp_pool_p);
	apr_pool_cleanup_register (temp_pool_p, (void *) "Cleaning up temp_pool_p", CleanUpPool, apr_pool_cleanup_null);

  ap_log_error (APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p, "Registering for server_p -> process -> pool [%#lx]", (unsigned long int) server_p -> process -> pool);
	apr_pool_cleanup_register (server_p -> process -> pool, (void *) "Cleaning up server_p -> process -> pool", CleanUpPool, apr_pool_cleanup_null);

  ap_log_error (APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p, "Registering for server_p -> process -> pconf [%#lx]", (unsigned long int) server_p -> process -> pconf);
	apr_pool_cleanup_register (server_p -> process -> pconf, (void *) "Cleaning up server_p -> process -> pconf", CleanUpPool, apr_pool_cleanup_null);

	return OK;
}

