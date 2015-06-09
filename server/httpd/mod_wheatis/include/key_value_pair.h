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

/**
 * @brief Get all of the data sent in a request.
 *
 * This will combine any parameters sent in the body
 * and those sent in the headers.
 *
 * @param req_p The request to get all of the data for.
 * @see GetGetRequestParameters
 * @see GetRequestBodyAsJSON
 * @return The json representation of the request parameters
 * or <code>NULL</code> upon error.
 */
json_t *GetAllRequestDataAsJSON (request_rec *req_p);


/**
 * @brief Get a json object from all of the GET request parameters.
 *
 * @param req_p The request to get all of the data for.
 * @return The json representation of the request parameters
 * or <code>NULL</code> upon error.
 */
json_t *GetGetRequestParameters (request_rec *req_p);


/**
 * @brief Get all of the data in a request body.
 *
 * @param req_p The request to get all of the data for.
 * @return The json representation of the request parameters
 * or <code>NULL</code> upon error.
 */
json_t *GetRequestBodyAsJSON (request_rec *req_p);


/**
 * Read the contents of an http body into a ByteBuffer.
 *
 * @param req_p The request to get all of the data for.
 * @param buffer_p The ByteBuffer to write to the data into.
 * @return The number of characters read or -1 upon error.
 */
int ReadBody (request_rec *req_p, ByteBuffer *buffer_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef KEY_VALUE_PAIR_H_ */

