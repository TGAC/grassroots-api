#include <stdlib.h>
#include <string.h>

#include "specificQuery.h"

#include "query.h"


genQueryOut_t *ExecuteQuery (rcComm_t *connection_p, char *query_s)
{
	specificQueryInp_t in_query;
	genQueryOut_t *out_query_p = NULL;
	int status;

	/* Build the query */
	memset (&in_query, 0, sizeof (specificQueryInp_t));
	in_query.maxRows = MAX_SQL_ROWS;
	in_query.continueInx = 0;
	in_query.sql = query_s;

	/* Run the query */
	status = rcSpecificQuery (connection_p, &in_query, &out_query_p);

	/* Did we run it successfully? */
	if (status == 0)
		{
			
		}
	else if (status == CAT_NO_ROWS_FOUND) 
		{
			printf ("No rows found\n"); 
		}
	else if (status < 0 ) 
		{
			printf ("error status: %d\n", status);
		}
	else
		{
			//printBasicGenQueryOut (out_query_p, "result: \"%s\" \"%s\"\n");
		}
	
	return out_query_p;
}


char *BuildQuery (const char **args_ss)
{
	char *buffer_p = NULL;
	const char **arg_pp = args_ss;
	size_t len = 0;
	
	/* Determine the length of string that we need */
	while (*arg_pp)
		{
			/* could cache these lengths to use later on... */
			len += strlen (*arg_pp);
			++ arg_pp;
		}
		
	buffer_p = (char *) malloc ((len + 1) * sizeof (char));
	if (buffer_p)
		{
			char *current_p = buffer_p;
			arg_pp = args_ss;
			
			while (*arg_pp)
				{
					size_t l = strlen (*arg_pp);
					strncpy (current_p, *arg_pp, l);
					
					current_p += l;
					++ arg_pp;
				}
					
			/* terminate the string */
			*current_p = '\0';
		}

	return buffer_p;
}


void FreeBuiltQuery (char *query_s)
{
	free (query_s);
}


