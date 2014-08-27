#include <stdlib.h>
#include <string.h>

#include "rodsGenQuery.h"
#include "rcMisc.h"

#include "query.h"

#include "string_utils.h"


genQueryOut_t *ExecuteQuery (rcComm_t *connection_p, genQueryInp_t * const in_query_p)
{
	genQueryOut_t *out_query_p = NULL;
	int status;
	
	/* Run the query */
	status = rcGenQuery (connection_p, in_query_p, &out_query_p);

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


void InitQuery (genQueryInp_t *query_p)
{
	memset (query_p, 0, sizeof (genQueryInp_t));
	query_p -> maxRows = MAX_SQL_ROWS;
	query_p -> continueInx = 0;
}


void ClearQuery (genQueryInp_t *query_p)
{
	if (query_p -> selectInp.len)
		{
			free (query_p -> selectInp.inx);
			query_p -> selectInp.inx = NULL;
			
			free (query_p -> selectInp.value);
			query_p -> selectInp.value = NULL;
			
			query_p -> selectInp.len = 0;
		}


	if (query_p -> sqlCondInp.len)
		{
			free (query_p -> sqlCondInp.inx);
			query_p -> sqlCondInp.inx = NULL;
			
			free (query_p -> sqlCondInp.value);
			query_p -> sqlCondInp.value = NULL;
			
			query_p -> sqlCondInp.len = 0;
		}

}


genQueryOut_t *ExecuteQueryString (rcComm_t *connection_p, char *query_s)
{
	genQueryInp_t in_query;
	genQueryOut_t *out_query_p = NULL;
	int status;

	/* Build the query */
	InitQuery (&in_query);

	/* Fill in the iRODS query structure */
	status = fillGenQueryInpFromStrCond (query_s, &in_query);
	
	if (status >= 0)
		{
			out_query_p = ExecuteQuery (connection_p, &in_query);
		}
	
	return out_query_p;
}


char *BuildQueryString (const char **args_ss)
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


void FreeBuiltQueryString (char *query_s)
{
	free (query_s);
}



bool SetQuerySelectClauses (genQueryInp_t *in_query_p, int num_columns, const int * const columns_p, const int * const values_p)
{
	bool success_flag = false;
	
	in_query_p -> selectInp.inx = (int *) malloc (num_columns * sizeof (int));

	if (in_query_p -> selectInp.inx)
		{
			in_query_p -> selectInp.value = (int *) malloc (num_columns * sizeof (int));

			if (in_query_p -> selectInp.value)
				{
					int *ptr = in_query_p -> selectInp.value;
					
					in_query_p -> selectInp.len = num_columns;					
					
					/* fill in the select columns */
					memcpy (in_query_p -> selectInp.inx, columns_p, num_columns * sizeof (int));
					
					/* we just want default selections, so set all of the values to 1 */
					while (num_columns > 0)
						{
							*ptr = 1;
							
							++ ptr;
							-- num_columns;
						}
					
					success_flag = true;
				}		/* if (in_query_p -> selectInp.value) */
			
			if (!success_flag)
				{
					free (in_query_p -> selectInp.inx);
					in_query_p -> selectInp.inx = NULL;
				}
		}
		
	return success_flag;
}


bool SetQueryWhereClauses (genQueryInp_t *in_query_p, int num_columns, const int *columns_p, const char **clauses_ss, const bool *quote_clause_p)
{
	bool success_flag = false;
	
	in_query_p -> sqlCondInp.inx = (int *) malloc (num_columns * sizeof (int));

	if (in_query_p -> sqlCondInp.inx)
		{
			in_query_p -> sqlCondInp.value = (char **) malloc (num_columns * sizeof (char *));

			if (in_query_p -> sqlCondInp.value)
				{
					char **dest_pp = in_query_p -> sqlCondInp.value;
					const char **src_pp = clauses_ss;

					in_query_p -> sqlCondInp.len = num_columns;
					
					/* fill in the select columns */
					memcpy (in_query_p -> sqlCondInp.inx, columns_p, num_columns * sizeof (int));
					
					/* we just want default selections, so set all of the values to 1 */
					while (num_columns > 0)
						{
							char *clause_s;
							
							if (*quote_clause_p)
								{
									char *temp_s = ConcatenateStrings ("= \'", *src_pp);	
									
									if (temp_s)
										{
											clause_s = ConcatenateStrings (temp_s, "\'");										
											FreeCopiedString (temp_s);
										}											
								}
							else
								{
									clause_s = ConcatenateStrings ("= ", *src_pp);										
								}
							
							if (clause_s)	
								{
									*dest_pp = clause_s;
									
									++ dest_pp;
									++ quote_clause_p;
									-- num_columns;											
								}
							else
								{
									while (dest_pp >= in_query_p -> sqlCondInp.value)
										{
											FreeCopiedString (*dest_pp);
											-- dest_pp;
										}
								}
						}
					
					if (num_columns == 0)
						{
							success_flag = true;
						}
					else
						{
							free (in_query_p -> sqlCondInp.value);
							in_query_p -> sqlCondInp.value = NULL;
						}
				}
			
			if (!success_flag)	
				{
					free (in_query_p -> sqlCondInp.inx);
					in_query_p -> sqlCondInp.inx = NULL;
				}
		}
		
	return success_flag;
}


int PrintQueryOutput (FILE *out_f, const genQueryOut_t *query_result_p)
{
	int i;

	for (i = 0;i < query_result_p -> rowCnt; ++ i) 
		{
			int j;
			const sqlResult_t *sql_result_p = query_result_p -> sqlResult;

			fprintf (out_f, "i = %d ----\n", i);	
				
			for (j = 0; j < query_result_p -> attriCnt; ++ j, ++ sql_result_p) 
				{
					char *result_s = sql_result_p -> value;
					
					result_s += i * (sql_result_p -> len);

					if (fprintf (out_f, "j = %d -> %s (%d)\n", j, result_s, sql_result_p -> len) < 0)
						{
							return -1;
						}
				}
		}

	return 0;
}


