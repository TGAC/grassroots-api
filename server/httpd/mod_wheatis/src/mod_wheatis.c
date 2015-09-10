/* Include the required headers from httpd */
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

#include "mod_wheatis_config.h"
#include "apr_jobs_manager.h"
#include "apr_servers_manager.h"

/* Define prototypes of our functions in this module */
static void RegisterHooks (apr_pool_t *pool_p);
static int WheatISHandler (request_rec *req_p);
static void WheatISChildInit (apr_pool_t *pool_p, server_rec *server_p);

static int WheatISPreConfig (apr_pool_t *config_pool_p, apr_pool_t *log_pool_p, apr_pool_t *temp_pool_p);
static int WheatISPostConfig (apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);

static const char *SetWheatISRootPath (cmd_parms *cmd_p, void *cfg_p, const char *arg_s);
static const char *SetWheatISCacheProvider (cmd_parms *cmd_p, void *cfg_p, const char *arg_s);

static void *CreateServerConfig (apr_pool_t *pool_p, server_rec *server_p);

static void *MergeServerConfig (apr_pool_t *pool_p, void *base_config_p, void *vhost_config_p);


static void *CreateDirectoryConfig (apr_pool_t *pool_p, char *context_s);
static void *MergeDirectoryConfig (apr_pool_t *pool_p, void *base_config_p, void *new_config_p);


static ModWheatISConfig *CreateConfig (apr_pool_t *pool_p, server_rec *server_p);


static apr_status_t CleanUpOutputStream (void *value_p);


#ifdef _DEBUG
	#define MOD_WHEATIS_DEBUG	(STM_LEVEL_FINE)
#else
	#define MOD_WHEATIS_DEBUG	(STM_LEVEL_NONE)
#endif


static const command_rec s_wheatis_directives [] =
{
	AP_INIT_TAKE1 ("WheatISCache", SetWheatISCacheProvider, NULL, ACCESS_CONF, "The provider for the Jobs Cache"),
	AP_INIT_TAKE1 ("WheatISRoot", SetWheatISRootPath, NULL, ACCESS_CONF, "The path to the WheatIS installation"),
	{ NULL }
};


static APRJobsManager *s_jobs_manager_p = NULL;
static const char * const s_jobs_manager_cache_id_s = "wheatis-jobs-socache";

static APRServersManager *s_servers_manager_p = NULL;
static const char * const s_servers_manager_cache_id_s = "wheatis-servers-socache";


/* Define our module as an entity and assign a function for registering hooks  */
module AP_MODULE_DECLARE_DATA wheatis_module =
{
    STANDARD20_MODULE_STUFF,
    CreateDirectoryConfig,   	// Per-directory configuration handler
    MergeDirectoryConfig,   	// Merge handler for per-directory configurations
    CreateServerConfig,				// Per-server configuration handler
    MergeServerConfig,				// Merge handler for per-server configurations
    s_wheatis_directives,			// Any directives we may have for httpd
    RegisterHooks    					// Our hook registering function
};


const module *GetWheatISModule (void)
{
	return &wheatis_module;
}


JobsManager *GetJobsManager (void)
{
	return (& (s_jobs_manager_p -> ajm_base_manager));
}



ServersManager *GetServersManager (void)
{
	return (& (s_servers_manager_p -> asm_base_manager));
}

/* register_hooks: Adds a hook to the httpd process */
static void RegisterHooks (apr_pool_t *pool_p)
{
	ap_hook_pre_config (WheatISPreConfig, NULL, NULL, APR_HOOK_MIDDLE);

	ap_hook_post_config (WheatISPostConfig, NULL, NULL, APR_HOOK_MIDDLE);

  ap_hook_child_init (WheatISChildInit, NULL, NULL, APR_HOOK_MIDDLE);

	ap_hook_handler (WheatISHandler, NULL, NULL, APR_HOOK_MIDDLE);
}



static int WheatISPreConfig (apr_pool_t *config_pool_p, apr_pool_t *log_pool_p, apr_pool_t *temp_pool_p)
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

		}		/* if (status == APR_SUCCESS) */

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


static ModWheatISConfig *CreateConfig (apr_pool_t *pool_p, server_rec *server_p)
{
	ModWheatISConfig *config_p = apr_palloc (pool_p, sizeof (ModWheatISConfig));

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





static void WheatISChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
//	ModWheatISConfig *config_p = ap_get_module_config (server_p -> module_config, &wheatis_module);

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

							if (log_p)
								{
									OutputStream *error_p = AllocateApacheOutputStream (server_p);

									if (error_p)
										{
											/* Mark the streams for deletion when the server pool expires */
											apr_pool_t *pool_p = server_p -> process -> pool;

											apr_pool_cleanup_register (pool_p, log_p, CleanUpOutputStream, apr_pool_cleanup_null);
											apr_pool_cleanup_register (pool_p, error_p, CleanUpOutputStream, apr_pool_cleanup_null);

											SetDefaultLogStream (log_p);
											SetDefaultErrorStream (error_p);
										}
									else
										{
											FreeOutputStream (log_p);
										}
								}

						}		/* If (InitInformationSystem ()) */

				}		/* if (APRServersManagerChildInit (pool_p, server_p)) */

		}		/* if (APRJobsManagerChildInit (pool_p, server_p)) */
}


static int WheatISPostConfig (apr_pool_t *config_pool_p, apr_pool_t *log_p, apr_pool_t *temp_p, server_rec *server_p)
{
  void *data_p = NULL;
  const char *userdata_key_s = "wheatis_post_config";
  int ret = HTTP_INTERNAL_SERVER_ERROR;
  apr_pool_t *server_pool_p = server_p -> process -> pool;

  /* Apache loads DSO modules twice. We want to wait until the second
   * load before setting up our global mutex and shared memory segment.
   * To avoid the first call to the post_config hook, we set some
   * dummy userdata in a pool that lives longer than the first DSO
   * load, and only run if that data is set on subsequent calls to
   * this hook. */
  apr_pool_userdata_get (&data_p, userdata_key_s, server_pool_p);

  if (data_p == NULL)
  	{
      /* WARNING: This must *not* be apr_pool_userdata_setn(). The
       * reason for this is because the static symbol section of the
       * DSO may not be at the same address offset when it is reloaded.
       * Since setn() does not make a copy and only compares addresses,
       * the get() will be unable to find the original userdata. */
      apr_pool_userdata_set ((const void *) 1, userdata_key_s, apr_pool_cleanup_null, server_pool_p);

      ret = OK; /* This would be the first time through */
  	}
  else
  	{
  		/*
  		 * We are now in the parent process before any child processes have been started, so this is
  		 * where any global shared memory should be allocated
       */
  		ModWheatISConfig *config_p = (ModWheatISConfig *) ap_get_module_config (server_p -> module_config, &wheatis_module);

  		if (config_p -> wisc_provider_name_s)
  			{
  	  		config_p -> wisc_jobs_manager_p = InitAPRJobsManager (server_p, config_pool_p, config_p -> wisc_provider_name_s);

  	  		if (config_p -> wisc_jobs_manager_p)
  					{
  	  				s_jobs_manager_p = config_p -> wisc_jobs_manager_p;
							apr_pool_cleanup_register (config_pool_p, config_p -> wisc_jobs_manager_p, CleanUpAPRJobsManager, apr_pool_cleanup_null);
							PostConfigAPRJobsManager (s_jobs_manager_p, config_pool_p, server_p, config_p -> wisc_provider_name_s);

							config_p -> wisc_servers_manager_p = InitAPRServersManager (server_p, config_pool_p, config_p -> wisc_provider_name_s);

							if (config_p -> wisc_servers_manager_p)
								{
									s_servers_manager_p = config_p -> wisc_servers_manager_p;
									apr_pool_cleanup_register (config_pool_p, config_p -> wisc_servers_manager_p, CleanUpAPRServersManager, apr_pool_cleanup_null);
									PostConfigAPRServersManager (s_servers_manager_p, config_pool_p, server_p, config_p -> wisc_provider_name_s);

									ret = OK;
								}
							else
								{

								}

  					}		/* if (config_p -> wisc_jobs_manager_p) */

  			}
  		else
  			{
  				ap_log_error (APLOG_MARK, APLOG_CRIT, ret, server_p, "You need to specify an socache module to load for WheatIS to work");
  			}
  	}

  return ret;
}


static apr_status_t CleanUpOutputStream (void *value_p)
{
	OutputStream *stream_p = (OutputStream *) value_p;
	FreeOutputStream (stream_p);

	return APR_SUCCESS;
}


/* Handler for the "WheatISRoot" directive */
static const char *SetWheatISRootPath (cmd_parms *cmd_p, void *cfg_p, const char *arg_s)
{
	SetServerRootDirectory (arg_s);

	return NULL;
}


/* Get the cache provider that we are going to use for the jobs manager storage */
static const char *SetWheatISCacheProvider (cmd_parms *cmd_p, void *cfg_p, const char *arg_s)
{
	ModWheatISConfig *config_p = (ModWheatISConfig *) cfg_p;
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
  		err_msg_s = apr_psprintf (cmd_p -> pool, "WheatISSOCache: %s", err_msg_s);
  	}



  return err_msg_s;
}



/* The handler function for our module.
 * This is where all the fun happens!
 */
static int WheatISHandler (request_rec *req_p)
{
	int res = DECLINED;

  /* First off, we need to check if this is a call for the wheatis handler.
   * If it is, we accept it and do our things, it not, we simply return DECLINED,
   * and Apache will try somewhere else.
   */
  if ((req_p -> handler) && (strcmp (req_p -> handler, "wheatis-handler") == 0))
  	{
  		if ((req_p -> method_number == M_GET) || (req_p -> method_number == M_POST))
  			{
  				/* Get the posted json data */
						json_t *json_req_p = GetAllRequestDataAsJSON (req_p);

						if (json_req_p)
							{
								int socket_fd = -1;
						    ModWheatISConfig *config_p = ap_get_module_config (req_p -> per_dir_config, &wheatis_module);
								json_t *res_p = ProcessServerJSONMessage (json_req_p,  socket_fd);

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

										json_decref (res_p);
									}		/* if (res_p) */
								else
									{
										res = HTTP_INTERNAL_SERVER_ERROR;
									}

								json_decref (json_req_p);
							}		/* if (json_req_p) */

  			}		/* if ((req_p -> method_number == M_GET) || (req_p -> method_number == M_POST)) */
  		else
  			{
  				res = HTTP_METHOD_NOT_ALLOWED;
  			}

  	}		/* if ((req_p -> handler) && (strcmp (req_p -> handler, "wheatis-handler") == 0)) */

  return res;
}



