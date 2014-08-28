#include <stdlib.h>
#include <string.h>

#include "rodsGenQueryNames.h"
#include "rcMisc.h"

#include "query.h"

#include "string_utils.h"



static const columnName_t *GetColumnById (const int id);


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


bool SetQueryWhereClauses (genQueryInp_t *in_query_p, int num_columns, const int *columns_p, const char **clauses_ss)
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
							char *clause_s = NULL;							
							char *temp_s = ConcatenateStrings ("= \'", *src_pp);	
							
							if (temp_s)
								{
									clause_s = ConcatenateStrings (temp_s, "\'");										
									FreeCopiedString (temp_s);
								}											
							
							if (clause_s)	
								{
									*dest_pp = clause_s;
									
									++ dest_pp;
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

					if (fprintf (out_f, "j = %d (%d) -> %s (%d)\n", j, sql_result_p -> attriInx, result_s, sql_result_p -> len) < 0)
						{
							return -1;
						}
				}
		}

	return 0;
}


/*
 * 
 * 	const columnName_t *qr_column_p;
	char **qr_values_pp;
	int qr_num_values;
	*/

QueryResult *AllocateQueryResult (int num_rows, const columnName_t *column_p)
{
	char **values_pp = (char **) calloc (num_rows, sizeof (char *));
	
	if (values_pp)
		{
			QueryResult *result_p = (QueryResult *) malloc (sizeof (QueryResult));
			
			if (result_p)
				{
					result_p -> qr_num_values = num_rows;
					result_p -> qr_column_p = column_p;
					result_p -> qr_values_pp = values_pp;
					
					return result_p;
				}
				
			free (values_pp);
		}

	return NULL;
}


bool InitQueryResult (QueryResult *result_p, int num_rows, const columnName_t *column_p)
{
	bool success_flag = false;
	char **values_pp = (char **) calloc (num_rows, sizeof (char *));
	
	if (values_pp)
		{
			result_p -> qr_num_values = num_rows;
			result_p -> qr_column_p = column_p;
			result_p -> qr_values_pp = values_pp;

			success_flag = true;
		}

	return success_flag;
}


bool SetQueryResultValue (QueryResult *result_p, int index, const char *value_s)
{
	bool success_flag = false;
	char *copied_value_s = strdup (value_s);
	
	if (copied_value_s)
		{
			char **value_pp = (result_p -> qr_values_pp) + index;
			
			if (*value_pp)
				{
					free (*value_pp);
				}			
				
			*value_pp = copied_value_s;
		
			success_flag = true;
		}

	
	return success_flag;
}


const columnName_t *GetColumnById (const int id)
{
	const columnName_t *column_p = columnNames;
  int i;
  
  for (i = NumOfColumnNames; i > 0; -- i, ++ column_p) 
		{
			if (column_p -> columnId == id)
				{
					return column_p;
				}
		}

#ifdef EXTENDED_ICAT
	column_p = extColumnNames;

  for (i = NumOfExtColumnNames; i > 0; -- i, ++ column_p) 
		{
			if (column_p -> columnId == id)
				{
					return column_p;
				}
		}
#endif

  return NULL;
}



QueryResults *GenerateQueryResults (const genQueryOut_t *query_result_p)
{
	QueryResults *results_p = (QueryResults *) malloc (sizeof (QueryResults));
	
	if (results_p)
		{
			const int num_rows = query_result_p -> rowCnt;
			const int num_columns = query_result_p -> attriCnt;
			
			if (num_rows > 0)
				{
					QueryResult *value_p = (QueryResult *) malloc (num_columns * sizeof (QueryResult));
					
					if (value_p)
						{
							QueryResults *results_p = NULL;
							const sqlResult_t *sql_result_p = query_result_p -> sqlResult;
							bool success_flag = true;
							int i;
							
							results_p -> qr_num_results = num_rows;
							results_p -> qr_values_p = value_p;
							
							/* Initialise the query results from the 1st row */							
							for (i = 0; i < num_columns; ++ i, ++ value_p, ++ sql_result_p)
								{
									const columnName_t *column_p = GetColumnById (sql_result_p -> attriInx);
									
									if (column_p)
										{
											if (InitQueryResult (value_p, num_rows, column_p))
												{
													char *result_s = sql_result_p -> value;
																										
													value_p = (results_p -> qr_values_p) + i;

													if (!SetQueryResultValue (value_p, 0, result_s))
														{
															success_flag = false;
														}
												}		
											else
												{
													success_flag = false;
												}
										}
									else
										{
											success_flag = false;
										}
										
								}		/* for (i = l; i > 0; -- i, ++ value_p) */
														
							
							/* Now fill in the remaining values */
							if (success_flag)
								{
									for (i = 1; i < num_rows; ++ i, ++ sql_result_p)
										{
											int j;
											
											for (j = 0; j < num_columns; ++ j) 
												{
													char *result_s = sql_result_p -> value;
													
													result_s += i * (sql_result_p -> len);

													value_p = (results_p -> qr_values_p) + j;


													if (!SetQueryResultValue (value_p, i, result_s))
														{
															success_flag = false;
														}
												}																					
											
										}		/* for (i = num_rows; i > 1; -- i, ++ sql_result_p) */
										
								}		/* if (success_flag) */
							
							
							if (!success_flag)
								{
									free (value_p);
								}
						}		/* if (value_p) */
					
				}		/* if (num_rows > 0) */
			
						
			
		}		/* if (results_p) */
		

	return NULL;
}


