/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <stdio.h>

#include "query.h"
#include "connect.h"
#include "user.h"

/*** STATIC PROTOTYPES ***/

static void TestGetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s);

static void TestGetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);

static void TestGetAllZonenames (rcComm_t *connection_p);

static void TestExecuteQueryString (rcComm_t *connection_p);

static void TestGetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from , const time_t to);

/*** METHODS ***/

int main (int argc, char *argv [])
{
	int result = 0;
	char *username_s = NULL;
	char *password_s = NULL;
	rcComm_t *connection_p = NULL;

	if (argc >= 3)
		{
			username_s = argv [1];
			password_s = argv [2];
		}

	connection_p = CreateIRODSConnection (username_s, password_s);

	if (connection_p)
		{
			struct tm from;
			struct tm to;
			time_t from_time;
			time_t to_time;

			/* 00:00:00 21/08/2014 */
			from.tm_sec = 0;
			from.tm_min = 0;
			from.tm_hour = 0;   /* hours since midnight (0 to 23) */
			from.tm_mday = 21;   /* day of the month (1 to 31) */
			from.tm_mon = 7;    /* months since January (0 to 11) */
			from.tm_year = 114;   /* years since 1900 */


			/* 00:00:00 22/08/2014 */
			to.tm_sec = 0;
			to.tm_min = 0;
			to.tm_hour = 0;   /* hours since midnight (0 to 23) */
			to.tm_mday = 22;   /* day of the month (1 to 31) */
			to.tm_mon = 7;    /* months since January (0 to 11) */
			to.tm_year = 114;   /* years since 1900 */


			from_time = mktime (&from);
			to_time = mktime (&to);

//			TestGetAllCollectionsForUsername (connection_p, "tyrrells");


//			TestGetAllDataForUsername (connection_p, "tyrrells");
/*
			TestGetAllZonenames (connection_p);
*/
			TestGetAllModifiedDataForUsername (connection_p, "tyrrells", from_time, to_time);

			//TestExecuteQueryString (connection_p);

			CloseIRODSConnection (connection_p);
		}		/* if (connection_p) */

	return result;
}



/*** STATIC METHODS ***/

static void TestGetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s)
{
	QueryResults *results_p = GetAllCollectionsForUsername (connection_p, username_s);

	if (results_p)
		{
			printf ("\nTestGetAllCollectionsForUsername : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}
}


static void TestGetAllDataForUsername (rcComm_t *connection_p, const char * const username_s)
{
	QueryResults *results_p = GetAllDataForUsername (connection_p, username_s, "10039");

	if (results_p)
		{
			printf ("\nTestGetAllDataForUsername : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}
}

static void TestGetAllZonenames (rcComm_t *connection_p)
{
	QueryResults *results_p = GetAllZoneNames (connection_p);

	if (results_p)
		{
			printf ("\nTestGetAllZonenames : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}
}


static void TestGetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from, const time_t to)
{
	QueryResults *results_p = GetAllModifiedDataForUsername (connection_p, username_s, from, to);

	if (results_p)
		{
			printf ("\nTestGetAllModifiedDataForUsername : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}
}


static void TestExecuteQueryString (rcComm_t *connection_p)
{
	const char **query_args_pp = (const char **) AllocMemoryArray (6, sizeof (const char *));

	if (query_args_pp)
		{
			char *full_query_s = NULL;

			*query_args_pp = "SELECT DATA_NAME, DATA_ID, COLL_NAME, COLL_ID where ";
			* (query_args_pp + 1) = "USER_NAME";
			* (query_args_pp + 2) = " = '";
			* (query_args_pp + 3) = "tyrrells";
			* (query_args_pp + 4) = "' AND USER_ID = '10039'";
			* (query_args_pp + 5) = NULL;

			full_query_s = BuildQueryString (query_args_pp);

			if (full_query_s)
				{
					genQueryOut_t *query_result_p = ExecuteQueryString (connection_p, full_query_s);

					if (query_result_p)
						{
							PrintQueryOutput (stdout, query_result_p);
						}		/* if (query_result_p) */

					FreeBuiltQueryString (full_query_s);
				}		/* if (full_query_s) */

			FreeMemory (query_args_pp);
		}		/* if (query_args_pp) */
}



char *ReadFileChunk (const char * const filename_s)
{
	FILE *in_f = fopen (filename_s, "rb");

	if (in_f)
		{
			/* Scroll 20 bytes into the file */
			if (fseek (in_f, 20, SEEK_SET) == 0)
				{
					/*
					 * We are going to try and read in 16 bytes so allocate
					 * the memory needed including the extra byte for the
					 * terminating \0
					 */
					const size_t buffer_size = 16;
					char *buffer_s = (char *) malloc ((buffer_size + 1) * sizeof (char));

					if (buffer_s)
						{
							/* Read in the data */
							if (fread (buffer_s, 1, buffer_size, in_f) == buffer_size)
								{
									/* Add the terminating \0 */
									* (buffer_s + buffer_size) = '\0';

									return buffer_s;
								}		/* if (fread (buffer_s, 1, buffer_size, in_f) == buffer_size) */
							else
								{
									fprintf (stderr, "Failed to read value from %s", filename_s);
								}

							free (buffer_s);
						}		/* if (buffer_s) */
					else
						{
							fprintf (stderr, "Failed to allocate memory for buffer when reading %s", filename_s);
						}

				}		/* if (fseek (in_f, 20, SEEK_SET) == 0) */
			else
				{
					fprintf (stderr, "Failed to seek in file %s", filename_s);
				}

			if (fclose (in_f) != 0)
				{
					fprintf (stderr, "Failed to close  file %s", filename_s);
				}		/* if (fclose (in_f) != 0) */

		}		/* if (in_f) */
	else
		{
			fprintf (stderr, "Failed to open %s", filename_s);
		}

	return NULL;
}


char *ReadFileChunk (const char * const filename_s)
{
	/* The value that we will return */
	char *result_s = NULL;

	/*
	 * Allocate the Resource.
	 * We know that it's a file so we use the protocol for a file,
	 * PROTOCOL_FILE_S, and we do not need a title for the Resource
	 * so we send it NULL.
	 */
	Resource res_p = AllocateResource (PROTOCOL_FILE_S, filename_s, NULL);

	if (res_p)
		{
			/*
			 * Now that we have the Resource, let the Grassroots system find
			 * the appropriate Handler.
			 */
			Handler *handler_p = GetResourceHandler (res_p, NULL);

			if (handler_p)
				{
					/* Scroll 20 bytes into the file */
					if (SeekHandler (handler_p, 20, SEEK_SET))
						{
							/*
							 * We are going to try and read in 16 bytes so allocate
							 * the memory needed including the extra byte for the
							 * terminating \0
							 */
							const size_t buffer_size = 16;
							char *buffer_s = (char *) AllocMemory ((buffer_size + 1) * sizeof (char));

							if (buffer_s)
								{
									/* Read in the data */
									if (ReadFromHandler (handler_p, buffer_s, buffer_size) == buffer_size)
										{
											/* Add the terminating \0 */
											* (buffer_s + buffer_size) = '\0';

											/* Store the value for returning */
											result_s = buffer_s;
										}		/* if (fread (buffer_s, 1, buffer_size, in_f) == buffer_size) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to read value from %s", filename_s);

											/* We failed to read in the value so we can release the buffer memory */
											FreeMemory (buffer_s);
										}

								}		/* if (buffer_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for buffer when reading %s", filename_s);
								}

						}		/* if (SeekHandler (handler_p, 20, SEEK_SET)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to seek in %s", filename_s);
						}

					CloseHandler (handler_p);
				}		/* if (handler_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to find a Handler for %s", filename_s);
				}

			FreeResource (res_p);
		}		/* if (res_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate Resource for %s", filename_s);
		}

	return result_s;
}


