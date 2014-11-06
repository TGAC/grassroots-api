/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

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

/* Define prototypes of our functions in this module */
static void RegisterHooks (apr_pool_t *pool_p);
static int WheatISHandler (request_rec *req_p);
static const char *SetWheatISRootPath (cmd_parms *cmd_p, void *cfg_p, const char *arg_s);


static const command_rec s_wheatis_directives [] =
{
    AP_INIT_TAKE1 ("WheatISRoot", SetWheatISRootPath, NULL, RSRC_CONF, "The path to the WheatIS installation"),
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
    NULL,            // Per-directory configuration handler
    NULL,            // Merge handler for per-directory configurations
    NULL,            // Per-server configuration handler
    NULL,            // Merge handler for per-server configurations
    NULL,            // Any directives we may have for httpd
    RegisterHooks    // Our hook registering function
};


/* register_hooks: Adds a hook to the httpd process */
static void RegisterHooks (apr_pool_t *pool_p) 
{
  /* Hook the request handler */
  ap_hook_handler (WheatISHandler, NULL, NULL, APR_HOOK_LAST);
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
 			/* Get the posted json data */
			json_t *json_req_p = GetRequestParameters (req_p);
			
			if (json_req_p)
				{
					int socket_fd = -1;
					
					res = OK;

					json_t *res_p = ProcessServerJSONMessage (json_req_p,  socket_fd);
					
					if (res_p)
						{
							char *res_s = json_dumps (res_p, JSON_INDENT (2));
							
							if (res_s)
								{
									ap_rputs (res_s, req_p);
								}		/* if (res_s) */
							
							json_decref (res_p);
						}		/* if (res_p) */
				
					json_decref (json_req_p);
				}		/* if (json_req_p) */

  	}		/* if ((req_p -> handler) && (strcmp (req_p -> handler, "wheatis-handler") == 0)) */
	 
  return res;
}


