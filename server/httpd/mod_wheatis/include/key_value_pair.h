#ifndef KEY_VALUE_PAIR_H_
#define KEY_VALUE_PAIR_H_

#include "httpd.h"
#include "jansson.h"
#include "byte_buffer.h"


typedef struct KeyValuePair
{
  char *kvp_key_s;
  char *kvp_value_s;
} KeyValuePair;


#ifdef __cplusplus
extern "C"
{
#endif

json_t *GetAllRequestDataAsJSON (request_rec *req_p);

json_t *GetGetRequestParameters (request_rec *req_p);

json_t *GetRequestBodyAsJSON (request_rec *req_p);

int ReadBody (request_rec *req_p, ByteBuffer *buffer_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef KEY_VALUE_PAIR_H_ */

