/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_log.h"

#include "apr_strings.h"
#include "apr_tables.h"
#include "util_script.h"


#include "key_value_pair.h"
#include "typedefs.h"
#include "byte_buffer.h"

typedef struct JsonRequest
{
	json_t *jr_json_p;
	request_rec *jr_req_p;
} JsonRequest;


/**********************************/
/******** STATIC PROTOTYPES *******/
/**********************************/


static bool ConvertFormPairToKeyValuePair (request_rec *req_p, ap_form_pair_t *pair_p, KeyValuePair *key_value_pair_p);

static json_t *ConvertGetParametersToJSON (request_rec *req_p);

static json_t *ConvertPostParametersToJSON (request_rec *req_p);

static int AddParameter (void *rec_p, const char *key_s, const char *value_s);

static bool AddJsonChild (json_t *parent_p, const char *key_s, const char *value_s, request_rec *req_p);


static int ReadRequestBody (request_rec *req_p, ByteBuffer *buffer_p);

static int ReadRequestBody2 (request_rec *req_p, ByteBuffer *buffer_p);

/**********************************/
/********** API METHODS ***********/
/**********************************/


json_t *GetAllRequestDataAsJSON (request_rec *req_p)
{
	json_t *get_params_p = ConvertGetParametersToJSON (req_p);
	json_t *body_params_p = GetRequestBodyAsJSON (req_p);
	json_t *res_p = NULL;

	if (get_params_p)
		{
			if (body_params_p)
				{
					int i = json_object_update (get_params_p, body_params_p);

					if (i != 0)
						{
							// error
						}

					json_object_clear (body_params_p);
					json_decref (body_params_p);
				}

			res_p = get_params_p;
		}
	else
		{
			res_p = body_params_p;
		}

	return res_p;
}




json_t *GetRequestBodyAsJSON (request_rec *req_p)
{
	json_t *params_p = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);
	
	if (buffer_p)
		{
			int res = ReadRequestBody (req_p, buffer_p);
			
			if (res == 0)
				{
					json_error_t error;
					const char *data_s = GetByteBufferData (buffer_p);
					params_p = json_loads (data_s, 0, &error);
					
					if (!params_p)
						{

						}
				}

			FreeByteBuffer (buffer_p);
		}

	return params_p;
}


/**********************************/
/********* STATIC METHODS *********/
/**********************************/


static bool ConvertFormPairToKeyValuePair (request_rec *req_p, ap_form_pair_t *pair_p, KeyValuePair *key_value_pair_p)
{
	bool success_flag = false;
	apr_off_t len;
	apr_size_t size;
	char *buffer_s = NULL;
	
	apr_brigade_length (pair_p -> value, 1, &len);
	size = (apr_size_t) len;

	buffer_s = apr_palloc (req_p -> pool, size + 1);
	
	if (buffer_s)
		{
			char *name_s = apr_pstrdup (req_p -> pool, pair_p -> name);
			
			if (name_s)
				{
					apr_brigade_flatten (pair_p -> value, buffer_s, &size);
					* (buffer_s + len) = '\0';
					
					key_value_pair_p -> kvp_key_s = name_s;
					key_value_pair_p -> kvp_value_s = buffer_s;										
					
					success_flag = true;
				}
		}
			
	
	return success_flag;
}


static json_t *ConvertGetParametersToJSON (request_rec *req_p)
{
	json_t *root_p = json_object ();
	
	if (root_p)
		{
			apr_table_t *params_p = NULL;
			JsonRequest json_req;
			
			json_req.jr_req_p = req_p;
			json_req.jr_json_p = root_p;
						
			ap_args_to_table (req_p, &params_p);  

			if (apr_table_do (AddParameter, &json_req, params_p, NULL) == TRUE)
				{
					
				}
			else
				{
					
				}
			
		}		/* if (root_p) */
	else
		{
			
		}
		
	return root_p;
}


static json_t *ConvertPostParametersToJSON (request_rec *req_p)
{
  json_t *root_p = NULL;
	apr_array_header_t *pairs_p = NULL;
  int res;
	
 	/* get the form parameters */
  res = ap_parse_form_data (req_p, NULL, &pairs_p, -1, HUGE_STRING_LEN);

  if ((res == OK) && pairs_p) 
  	{
			/* 
			 * Check to see it the params are individual json objects 
			 * or if they are a single entry
			 */
			if (pairs_p -> nelts == 1)
				{
					KeyValuePair kvp;
					
					ap_form_pair_t *pair_p = (ap_form_pair_t *) apr_array_pop (pairs_p);
					
					if (ConvertFormPairToKeyValuePair (req_p, pair_p, &kvp))
						{
							if ((kvp.kvp_key_s) && (strcmp (kvp.kvp_key_s, "json") == 0))
								{
									json_error_t error;
									root_p = json_loads (kvp.kvp_value_s, JSON_PRESERVE_ORDER, &error);

									if (!root_p)
										{
											ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Failed to parse \"%s\", error: %s, source %s line %d columd %d offset %d", 
												kvp.kvp_value_s, error.text, error.source, error.line, error.column, error.position);																		
										}
								}
							else
								{
									json_t *root_p = json_object ();
									
									if (root_p)
										{
											AddJsonChild (root_p, kvp.kvp_key_s, kvp.kvp_value_s, req_p);
										}		/* if (root_p) */	
									else
										{
											ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Not enough memory to allocate root json object");									
										}		
								}
								
						}		/* if (ConvertFormPairToKeyValuePair (req_p, pair_p, &kvp)) */
					else
						{
							
						}
				}
			else
				{
					json_t *root_p = json_object ();
							
					if (root_p)
						{
							bool success_flag = true;

							/* Pop each parameter pair and add it to our json array */
							while (pairs_p && !apr_is_empty_array (pairs_p)) 
								{
									KeyValuePair kvp;
									
									ap_form_pair_t *pair_p = (ap_form_pair_t *) apr_array_pop (pairs_p);
									
									if (ConvertFormPairToKeyValuePair (req_p, pair_p, &kvp))
										{
											success_flag = AddJsonChild (root_p, kvp.kvp_key_s, kvp.kvp_value_s, req_p);											
										}
									else
										{
											
										}
										
								}		/* while (pairs_p && !apr_is_empty_array (pairs_p)) */
								
						}		/* if (root_p) */	
					else
						{
							ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Not enough memory to allocate root json object");									
						}		
				
				}
				
		}		/* if ((res == OK) && pairs_p)  */			
	else
		{
			ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Failed to get post parameters from %s", req_p -> uri);									
		}  			 
		 
	return root_p;
}


static int AddParameter (void *rec_p, const char *key_s, const char *value_s)
{
	int res = FALSE;
	JsonRequest *json_req_p = (JsonRequest *) rec_p;

	if (AddJsonChild (json_req_p -> jr_json_p, key_s, value_s, json_req_p -> jr_req_p))
		{
			res = TRUE;
		}

	return res;
}	



	/* 
	 * break 
	 * 	
	 * 	foo.bar.stuff = bob 
	 * 
	 * into 
	 * 
	 * 	foo {
	 * 		bar {
	 * 			stuff = bob
	 *		}
	 * 	}
	 */  
	
static bool AddJsonChild (json_t *parent_p, const char *key_s, const char *value_s, request_rec *req_p)
{
	bool success_flag = true;		
	char *copied_key_s = apr_pstrdup (req_p -> pool, key_s);
	
	if (copied_key_s)
		{
			char *last_p = NULL;
			char *this_token_p = apr_strtok (copied_key_s, ".", &last_p);
			char *next_token_p = NULL;
			bool loop_flag = (this_token_p != NULL);
			json_t *child_p = NULL;
			
			while (loop_flag && success_flag)
				{
					next_token_p = apr_strtok (NULL, ".", &last_p);
					
					child_p = json_object_get (parent_p, this_token_p);
					
					if (!child_p)
						{
							success_flag = false;
							
							if (next_token_p)
								{
									child_p = json_object ();
								}
							else
								{
									child_p = json_string (value_s);									
								}

							
							if (child_p)
								{
									if (json_object_set (parent_p, this_token_p, child_p) == 0)
										{
											success_flag = true;
										}
									else
										{
											ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Couldn't add json value for %s to json parameters", this_token_p);
											
											json_decref (child_p);
											child_p = NULL;
										}
								}
							else
								{
									ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Not enough memory to allocate json child  for %s", this_token_p);
								}
						}		/* if (!child_p) */
					
					if (success_flag)
						{
							parent_p = child_p;

							if (next_token_p)
								{
									this_token_p = next_token_p;
									loop_flag = (this_token_p != NULL);									
								}
							else
								{
									loop_flag = false;
								}
						}

				}		/* while (loop_flag) */
									
		}		/* if (copied_key_s) */
	else
		{
			ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Not enough memory to copy %s", key_s);
			success_flag = false;
		}
	return success_flag;
}


static int ReadRequestBody2 (request_rec *req_p, ByteBuffer *buffer_p)
{
	apr_status_t status;
	bool has_input_flag = false;
	const char *value_s = NULL;
	int ret = OK;

	/* Is there any input? */
	if (apr_table_get (req_p -> headers_in, "Content-Length"))
		{
			has_input_flag = true;
		}
	else if ((value_s = apr_table_get (req_p -> headers_in, "Transfer-Encoding")) != NULL)
		{
			if (strcasecmp (value_s, "chunked") == 0)
				{
					has_input_flag = true;
				}
			else
				{
					ap_rprintf (req_p, "<p>Unknown transfer encoding: %s</p>", ap_escape_html (req_p -> pool, value_s));

				}
		}

	if (has_input_flag)
		{
			#define BUFFER_SIZE (256)

			/* Create a brigade where we will store the data */
			apr_bucket_brigade *brigade_p = apr_brigade_create (req_p -> pool, req_p -> connection -> bucket_alloc);
			bool continue_flag = true;

			/* Loop until we get an EOS */
			do
				{
					/* Read a chunk */
					status = ap_get_brigade (req_p -> input_filters, brigade_p, AP_MODE_READBYTES, APR_BLOCK_READ, BUFFER_SIZE);

					if (status == APR_SUCCESS)
						{
							apr_bucket *bucket_p = APR_BRIGADE_FIRST (brigade_p);

							/* loop through all of the buckets */
							while (continue_flag && (bucket_p != APR_BRIGADE_SENTINEL (brigade_p)))
								{
									if (APR_BUCKET_IS_EOS (bucket_p))
										{
											continue_flag = false;
										}
									else if (!APR_BUCKET_IS_METADATA (bucket_p))
										{
											const char buffer_s [BUFFER_SIZE];
											apr_size_t bytes_read = BUFFER_SIZE;
											const char *buf_p = buffer_s;

											/* read the data */
											status = apr_bucket_read (bucket_p, &buf_p, &bytes_read, APR_BLOCK_READ);

											if (bytes_read)
												{
													continue_flag = AppendToByteBuffer (buffer_p, buffer_s, bytes_read);
												}
										}

									if (continue_flag)
										{
											bucket_p = APR_BUCKET_NEXT (bucket_p);
										}
								}

						}		/* if (status == APR_SUCCESS) */

					/* We've finished with the brigade so clean it up */
					apr_brigade_cleanup (brigade_p);
				}
			while (continue_flag && (status == APR_SUCCESS));

		}
	else
		{
			ap_rputs ("<p>Empty request body</p>\n", req_p);
		}


	if (ret == OK)
		{
			if (status != APR_SUCCESS)
				{
					ret = HTTP_INTERNAL_SERVER_ERROR;
				}
		}

	return ret;
}


static int ReadRequestBody (request_rec *req_p, ByteBuffer *buffer_p)
{
	apr_off_t size = 0;
	const char *buffer_s = NULL;
	int res = ReadBody (req_p,  &buffer_s, &size);

	if (res == OK)
		{
			AppendToByteBuffer (buffer_p, buffer_s, size);
		}

	return res;
}



int ReadBody (request_rec *req_p, const char **buffer_ss, apr_off_t *size_p)
{
	int ret = ap_setup_client_block (req_p, REQUEST_CHUNKED_ERROR);

	if (ret == OK)
		{
	    if (ap_should_client_block (req_p))
	    	{
	        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	        char         temp_s [HUGE_STRING_LEN];
	        apr_off_t    rsize, len_read, rpos = 0;
	        apr_off_t length = req_p->remaining;
	        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	        *buffer_ss = (const char *) apr_pcalloc (req_p -> pool, (apr_size_t) (length + 1));
	        *size_p = length;

	        while((len_read = ap_get_client_block (req_p, temp_s, sizeof(temp_s))) > 0)
	        	{
	            if((rpos + len_read) > length)
	            	{
	                rsize = length - rpos;
	            	}
	            else
	            	{
	                rsize = len_read;
	            	}

	            memcpy ((char *) *buffer_ss + rpos, temp_s, (size_t) rsize);
	            rpos += rsize;
	        }
	    }
		}

    return (ret);
}


