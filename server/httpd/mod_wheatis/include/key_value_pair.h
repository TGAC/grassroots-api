#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

#include "httpd.h"
#include "jansson.h"

typedef struct KeyValuePair
{
  char *kvp_key_s;
  char *kvp_value_s;
} KeyValuePair;

json_t *GetAllRequestDataAsJSON (request_rec *req_p);

json_t *GetGetRequestParameters (request_rec *req_p);

json_t *GetRequestBodyAsJSON (request_rec *req_p);


int ReadBody (request_rec *r, const char **rbuf, apr_off_t *size);

#endif		/* #ifndef KEY_VALUE_PAIR_H */

