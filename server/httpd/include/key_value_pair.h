#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

#include "httpd.h"
#include "jansson.h"

typedef struct KeyValuePair
{
  const char *kvp_key_s;
  const char *kvp_value_s;
} KeyValuePair;


json_t *GetRequestParameters (request_rec *req_p);

#endif		/* #ifndef KEY_VALUE_PAIR_H */

