/**
 * This code is by
 * sander@temme.net              http://www.temme.net/sander/
 *
 * see http://marc.info/?l=apache-modules&m=107669698011831
 */
/* Start of mod_pooltest code */
#include <sys/types.h>
#include <unistd.h>

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include "http_log.h"


static server_rec *s_server_p = NULL;


static apr_status_t pooltest_cleanup(void *data) {
    /* No server_rec to log against, go straight to stderr */
    fprintf(stderr, "Pool cleanup in process %s\n", (char *)data);
    fflush (stderr);

    ap_log_error (APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s_server_p,
                 "Pool cleanup in process %d:  %s\n", getpid(), (char *)data);
    return OK;
}

static void pooltest_child_init(apr_pool_t *pool_p, server_rec *server_p)
{
  ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p,
               "pooltest_child_init, in process %d", getpid());

  ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p,
                   "Registering for pool_p [%#lx]", (void *)pool_p);
	apr_pool_cleanup_register(pool_p, (void *)"Cleaning up pool_p",
														pooltest_cleanup, apr_pool_cleanup_null);

  ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p,
                   "Registering for server_p -> process-> pool [%#lx]", (void *)server_p -> process-> pool);
	apr_pool_cleanup_register(server_p -> process-> pool, (void *)"Cleaning up server_p -> process-> pool",
														pooltest_cleanup, apr_pool_cleanup_null);

  ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, server_p,
                   "Registering for server_p -> process-> pconf [%#lx]", (void *)server_p -> process-> pconf);
	apr_pool_cleanup_register(server_p -> process-> pconf, (void *)"Cleaning up server_p -> process-> pconf",
														pooltest_cleanup, apr_pool_cleanup_null);


}

static int pooltest_post_config(apr_pool_t *pconf, apr_pool_t *plog,
                                apr_pool_t *ptemp, server_rec *s) {

    const char *userdata_key = "pooltest_init_module";
    void *data;
    apr_pool_userdata_get(&data, userdata_key, s->process->pool);
    if (!data) {
        ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s,
                     "First time post_config, in process %d", getpid());
        apr_pool_userdata_set((const void *) 1, userdata_key,
                              apr_pool_cleanup_null, s->process->pool);
    } else {
    		s_server_p = s;

        ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s,
                     "Second time post_config, in process %d", getpid());

        ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s,
                         "Registering for pconf [%#lx]", (void *)pconf);
				apr_pool_cleanup_register(pconf, (void *)"Cleaning up pconf",
																	pooltest_cleanup, apr_pool_cleanup_null);
				ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s,
										 "Registering for plog [%#lx]", (void *)plog);
				apr_pool_cleanup_register(pconf, (void *)"Cleaning up plog",
																	pooltest_cleanup, apr_pool_cleanup_null);
				ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s,
										 "Registering for ptemp [%#lx]", (void *)ptemp);
				apr_pool_cleanup_register(pconf, (void *)"Cleaning up ptemp",
																	pooltest_cleanup, apr_pool_cleanup_null);
				ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s,
										 "Registering for s->process->pool [%#lx]",
										 (void *)s->process->pool);
				apr_pool_cleanup_register(s->process->pool,
																	(void *)"Cleaning up s->process->pool",
																	pooltest_cleanup, apr_pool_cleanup_null);
				ap_log_error(APLOG_MARK, APLOG_NOERRNO | APLOG_DEBUG, 0, s,
										 "Registering for s->process->pconf [%#lx]",
										 (void *)s->process->pconf);
				apr_pool_cleanup_register(s->process->pconf,
																	(void *)"Cleaning up s->process->pconf",
																	pooltest_cleanup, apr_pool_cleanup_null);
    }

    return OK;
}

static void pooltest_register_hooks(apr_pool_t *p)
{
    ap_hook_post_config(pooltest_post_config, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_child_init (pooltest_child_init, NULL, NULL, APR_HOOK_LAST);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA pooltest_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    pooltest_register_hooks  /* register hooks                      */
};
/* End of module code */
