#ifndef WHEATIS_SERVICE_MANAGER_H
#define WHEATIS_SERVICE_MANAGER_H


#include "wheatis_service_manager_library.h"
#include "jansson.h"

#define DEFAULT_SERVER_PORT	("9991")


#define KEY_OPERATIONS	("operations")
#define KEY_INTERVAL		("interval")
#define KEY_IRODS				("irods")
#define	KEY_FILE_DATA		("file_data")


#define KEY_FILENAME	("file")
#define KEY_DIRNAME		("dir")


enum Operations {
	/** Get list of all available services */
	OP_LIST_ALL_SERVICES = 0,
	
	/** Get list of data objects and collections modified within a given time period */
	OP_IRODS_MODIFIED_DATA,

	/** Query services to see if they can work on file */
	OP_LIST_INTERESTED_SERVICES,
	
	OP_NUM_OPERATIONS
};





 
#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Construct a response message based upon a client's message.
 *
 * @param request_s The message from the client.
 * @return The response from the server.
 */
WHEATIS_SERVICE_MANAGER_API json_t *ProcessMessage (const char * const request_s);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_SERVICE_MANAGER_H */
