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
	
	OP_NUM_OPERATIONS
} Operation;



#endif		/* #ifndef OPERATION_H */		
