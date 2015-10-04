#ifndef HANDLE_UTILS_H
#define HANDLE_UTILS_H


#include "grassroots_handler_library.h"
#include "parameter.h"
#include "handler.h"




#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_HANDLER_API Handler *GetResourceHandler (const Resource *resource_p, const json_t *tags_p);


/**
 * This allocates the internal structures used by the Handler library.
 *
 * @return true upon success, false on failure.
 */
GRASSROOTS_HANDLER_API bool InitHandlerUtil (void);


/**
 * This frees the internal structures used by the Handler library.
 *
 * @return true upon success, false on failure.
 */
GRASSROOTS_HANDLER_API bool DestroyHandlerUtil (void);



GRASSROOTS_HANDLER_API const char *GetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, time_t *time_p);

GRASSROOTS_HANDLER_API bool SetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, const char *mapped_filename_s, const time_t last_mod_time);



#ifdef __cplusplus
}
#endif

#endif		/* #ifndef HANDLE_UTILS_H */
