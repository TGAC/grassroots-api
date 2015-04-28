/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_config.h"

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

/* Define prototypes of our functions in this module */
static void RegisterHooks (apr_pool_t *pool_p);
static int WheatISHandler (request_rec *req_p);
static int WheatISPostConfig (apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);
static const char *SetWheatISRootPath (cmd_parms *cmd_p, void *cfg_p, const char *arg_s);

#ifdef _DEBUG
	#define MOD_WHEATIS_DEBUG	(STM_LEVEL_FINE)
#else
	#define MOD_WHEATIS_DEBUG	(STM_LEVEL_NONE)
#endif


static const command_rec s_wheatis_directives [] =
{
	AP_INIT_TAKE1 ("WheatISRoot", SetWheatISRootPath, NULL, ACCESS_CONF, "The path to the WheatIS installation"),
	{ NULL }
};



typedef struct
{
	const char *wisc_root_path_s;
} WheatISConfig;



static WheatISConfig s_config;


/* Define our module as an entity and assign a function for registering hooks  */
module AP_MODULE_DECLARE_DATA wheatis_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,            			// Per-directory configuration handler
    NULL,            			// Merge handler for per-directory configurations
    NULL,            			// Per-server configuration handler
    NULL,            			// Merge handler for per-server configurations
    s_wheatis_directives,	// Any directives we may have for httpd
    RegisterHooks    			// Our hook registering function
};


/* register_hooks: Adds a hook to the httpd process */
static void RegisterHooks (apr_pool_t *pool_p) 
{
  ap_hook_post_config (WheatISPostConfig, NULL, NULL, APR_HOOK_REALLY_FIRST);
	ap_hook_handler (WheatISHandler, NULL, NULL, APR_HOOK_FIRST);
}



static int WheatISPostConfig (apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
  void *data_p = NULL;
  const char *userdata_key_s = "wheatis_post_config";
  int ret = HTTP_INTERNAL_SERVER_ERROR;

  /* Apache loads DSO modules twice. We want to wait until the second
   * load before setting up our global mutex and shared memory segment.
   * To avoid the first call to the post_config hook, we set some
   * dummy userdata in a pool that lives longer than the first DSO
   * load, and only run if that data is set on subsequent calls to
   * this hook. */
  apr_pool_userdata_get (&data_p, userdata_key_s, s->process->pool);

  if (data_p == NULL)
  	{
      /* WARNING: This must *not* be apr_pool_userdata_setn(). The
       * reason for this is because the static symbol section of the
       * DSO may not be at the same address offset when it is reloaded.
       * Since setn() does not make a copy and only compares addresses,
       * the get() will be unable to find the original userdata. */
      apr_pool_userdata_set ((const void *) 1, userdata_key_s, apr_pool_cleanup_null, s->process->pool);

      ret = OK; /* This would be the first time through */
  	}
  else
  	{
  		if (InitInformationSystem ())
  			{
  				ret = OK;
  			}
  	}

  return ret;
}



/* Handler for the "WheatISRoot" directive */
static const char *SetWheatISRootPath (cmd_parms *cmd_p, void *cfg_p, const char *arg_s)
{
	SetServerRootDirectory (arg_s);

	return NULL;
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
								json_t *res_p = NULL;

								res = OK;

								res_p = ProcessServerJSONMessage (json_req_p,  socket_fd);

								if (res_p)
									{
										char *res_s = json_dumps (res_p, JSON_INDENT (2));

										if (res_s)
											{
												ap_rputs (res_s, req_p);

												free (res_s);
											}		/* if (res_s) */

										json_decref (res_p);
									}		/* if (res_p) */

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



