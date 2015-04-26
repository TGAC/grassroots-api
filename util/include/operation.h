#ifndef OPERATION_H
#define OPERATION_H

typedef enum Operation {
	OP_NONE = -1,
	
	/** Get list of all available services */
	OP_LIST_ALL_SERVICES,
	
	/** Get list of data objects and collections modified within a given time period */
	OP_IRODS_MODIFIED_DATA,

	/** Query services to see if they can work on file */
	OP_LIST_INTERESTED_SERVICES,
	
	/** Call any services that have a keyword parameter with a given keyword */
	OP_RUN_KEYWORD_SERVICES,

	/** Get list of services matching the given names */
	OP_GET_NAMED_SERVICES,

	/** Get status of running services */
	OP_CHECK_SERVICE_STATUS,

	/** Get results from completed job */
	OP_GET_SERVICE_RESULTS,

	OP_NUM_OPERATIONS
} Operation;


typedef enum OperationStatus
{
	OS_ERROR = -1,
	OS_IDLE,
	OS_FAILED_TO_START,
	OS_STARTED,
	OS_FINISHED,
	OS_FAILED,
	OS_SUCCEEDED
} OperationStatus;



#endif		/* #ifndef OPERATION_H */		
