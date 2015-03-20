#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

#include "httpd.h"
#include "jansson.h"
#include "byte_buffer.h"


typedef struct KeyValuePair
{
  char *kvp_key_s;
  char *kvp_value_s;
} KeyValuePair;

json_t *GetAllRequestDataAsJSON (request_rec *req_p);

json_t *GetGetRequestParameters (request_rec *req_p);

json_t *GetRequestBodyAsJSON (request_rec *req_p);

int ReadBody (request_rec *req_p, ByteBuffer *buffer_p);

#endif		/* #ifndef KEY_VALUE_PAIR_H */

