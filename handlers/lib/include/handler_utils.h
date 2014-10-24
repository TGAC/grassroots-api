#ifndef HANDLE_UTILS_H
#define HANDLE_UTILS_H


#include "wheatis_handler_library.h"
#include "parameter.h"
#include "handler.h"




#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_HANDLER_API Handler *GetResourceHandler (const Resource *resource_p, const json_t *tags_p);


/**
 * This allocates the internal structures used by the Handler library.
 * 
 * @return true upon success, false on failure.
 */
WHEATIS_HANDLER_API bool InitHandlerUtil (void);


/**
 * This frees the internal structures used by the Handler library.
 * 
 * @return true upon success, false on failure.
 */
WHEATIS_HANDLER_API bool DestoyHandlerUtil (void);



WHEATIS_HANDLER_LOCAL const char *GetMappedFilename (const char *protocol_s, const char *filename_s);


WHEATIS_HANDLER_LOCAL bool GetMappedFilename (const char *protocol_s, const char *filename_s, const char *mapped_filename_s);




#ifdef __cplusplus
}
#endif

#endif		/* #ifndef HANDLE_UTILS_H */
