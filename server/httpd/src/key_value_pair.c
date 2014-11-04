#include "apr_strings.h"
#include "http_log.h"


#include "key_value_pair.h"
#include "typedefs.h"
#include "jansson.h"


static bool AddJsonChild (json_t *parent_p, const char *key_s, const char *value_s, request_rec *req_p);


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


KeyValuePair *GetRawPostParameters (request_rec *req_p)
{
  KeyValuePair *key_value_pairs_p = NULL;
	apr_array_header_t *pairs_p = NULL;
  int res;
 
 	/* get the form parameters */
  res = ap_parse_form_data (req_p, NULL, &pairs_p, -1, HUGE_STRING_LEN);

  if ((res == OK) && pairs_p) 
  	{
			key_value_pairs_p = apr_pcalloc (req_p -> pool, sizeof (KeyValuePair) * ((pairs_p -> nelts) + 1));
			
			if (key_value_pairs_p)
				{
					char *buffer_s;
					apr_off_t len;
					KeyValuePair *key_value_pair_p = key_value_pairs_p;
					
					/* Pop each parameter pair and add it to our list */
					while (pairs_p && !apr_is_empty_array (pairs_p)) 
						{
							ap_form_pair_t *pair_p = (ap_form_pair_t *) apr_array_pop (pairs_p);
							
							if (ConvertFormPairToKeyValuePair (req_p, pair_p, key_value_pair_p))
								{
									++ key_value_pair_p;
								}
						}
				
				}		/* if (key_value_pairs_p) */
			

		}		/* if ((res == OK) && pairs_p) */

  	
  return key_value_pairs_p;
}



json_t *ConvertPostParametersToJSON (request_rec *req_p)
{
  json_t *root_p = NULL;
	apr_array_header_t *pairs_p = NULL;
  int res;
	int socket_fd = -1;
	
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
											ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Failed to parse \"%s\", error: %s, source %s line %d columd %d offset %lu", 
												pair_p -> value, error.text, error.source, error.line, error.column, error.position);																		
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
							char *buffer_s;
							apr_off_t len;							

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



static bool AddJsonChild (json_t *parent_p, const char *key_s, const char *value_s, request_rec *req_p)
{
	bool success_flag = false;	
	json_t *child_p = json_string (value_s);
	
	if (child_p)
		{
			if (json_object_set_new (parent_p, key_s, child_p) == 0)
				{
					success_flag = true;
				}
			else
				{
					ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Couldn't add json value for %s to json parameters", key_s);
					json_decref (child_p);
				}
		}
	else
		{
			ap_log_rerror (APLOG_MARK, APLOG_ERR, 0, req_p, "Not enough memory to allocate json child object for %s", key_s);									
		}
	
	return success_flag;
}
