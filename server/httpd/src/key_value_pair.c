#include "key_value_pair.h"


KeyValuePair *GetPostParameters (request_rec *req_p)
{
  KeyValuePair *key_value_pairs_p = NULL;
	apr_array_header_t *pairs_p = NULL;
  int res;
 
 	/* get the form parameters */
  res = ap_parse_form_data (req_p, NULL, &pairs_p, -1, HUGE_STRING_LEN);

  if ((res == OK) && pairs_p) 
  	{
			key_value_pairs_p = apr_pcalloc (r->pool, sizeof (KeyValuePair) * (pairs->nelts + 1));
			
			if (key_value_pairs_p)
				{
					char *buffer_s;
					apr_off_t len;
					KeyValuePair *key_value_pair_p = key_value_pairs_p;
					
					/* Pop each parameter pair and add it to our list */
					while (pairs_p && !apr_is_empty_array (pairs_p)) 
						{
							ap_form_pair_t *pair_p = (ap_form_pair_t *) apr_array_pop (pairs_p);
							apr_brigade_length (pair_p -> value, 1, &len);
							apr_size_t size = (apr_size_t) len;

							buffer_s = apr_palloc (req_p -> pool, size + 1);
							if (buffer_s)
								{
									char *name_s = apr_pstrdup (rec_p -> pool, pair_p -> name);
									
									if (name_s)
										{
											apr_brigade_flatten (pair_p -> value, buffer_s, &size);
											* (buffer + len) = '\0';
											key_value_pair_p -> kvp_key_s = name_s;
											key_value_pair_p -> kvp_value_s = buffer_s;										
											
											++ key_value_pair_p;
											
										}		/* if (name_s) */
									else
										{
										
										}
											
								}		/* if (buffer_s) */
							else
								{
								
								}
						}
				
				}		/* if (key_value_pairs_p) */
			

		}		/* if ((res == OK) && pairs_p) */

  	
  return key_value_pairs_p;
}
