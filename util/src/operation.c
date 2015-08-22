/*
 * operation.c
 *
 *  Created on: 21 Aug 2015
 *      Author: billy
 */

#include <string.h>

#include "operation.h"



static const char * const S_FAILED_S = "Failed";
static const char * const S_FAILED_TO_START_S = "Failed to start";
static const char * const S_ERROR_S = "Error";
static const char * const S_IDLE_S = "Idle";
static const char * const S_PENDING_S = "Pending";
static const char * const S_STARTED_S = "Started";
static const char * const S_FINISHED_S = "Finished";
static const char * const S_PARTIALLY_SUCCEEDED_S = "Partially succeeded";
static const char * const S_SUCCEEDED_S = "Succeeded";
static const char * const S_CLEANED_UP_S = "Cleaned up";


const char *GetOperationStatusAsString (const OperationStatus status)
{
	const char *result_s = NULL;

	switch (status)
		{
			case OS_FAILED:
				result_s = S_FAILED_S;
				break;

			case OS_FAILED_TO_START:
				result_s = S_FAILED_TO_START_S;
				break;

			case OS_ERROR:
				result_s = S_ERROR_S;
				break;

			case OS_IDLE:
				result_s = S_IDLE_S;
				break;

			case OS_PENDING:
				result_s = S_PENDING_S;
				break;

			case OS_STARTED:
				result_s = S_STARTED_S;
				break;

			case OS_FINISHED:
				result_s = S_FINISHED_S;
				break;

			case OS_PARTIALLY_SUCCEEDED:
				result_s = S_PARTIALLY_SUCCEEDED_S;
				break;

			case OS_SUCCEEDED:
				result_s = S_SUCCEEDED_S;
				break;

			case OS_CLEANED_UP:
				result_s = S_CLEANED_UP_S;
				break;

			default:
				break;
		}

	return result_s;
}


OperationStatus GetOperationStatusFromString (const char *status_s)
{
	OperationStatus status = OS_NUM_STATUSES;

	if (status_s)
		{
			if (strcmp (status_s, S_FAILED_S) == 0)
				{
					status = OS_FAILED;
				}
			else if (strcmp (status_s, S_FAILED_TO_START_S) == 0)
				{
					status = OS_FAILED_TO_START;
				}
			else if (strcmp (status_s, S_ERROR_S) == 0)
				{
					status = OS_ERROR;
				}
			else if (strcmp (status_s, S_IDLE_S) == 0)
				{
					status = OS_IDLE;
				}
			else if (strcmp (status_s, S_PENDING_S) == 0)
				{
					status = OS_PENDING;
				}
			else if (strcmp (status_s, S_STARTED_S) == 0)
				{
					status = OS_STARTED;
				}
			else if (strcmp (status_s, S_FINISHED_S) == 0)
				{
					status = OS_FINISHED;
				}
			else if (strcmp (status_s, S_PARTIALLY_SUCCEEDED_S) == 0)
				{
					status = OS_PARTIALLY_SUCCEEDED;
				}
			else if (strcmp (status_s, S_SUCCEEDED_S) == 0)
				{
					status = OS_SUCCEEDED;
				}
			else if (strcmp (status_s, S_CLEANED_UP_S) == 0)
				{
					status = OS_CLEANED_UP;
				}
		}		/* if (status_s) */

	return status;
}
