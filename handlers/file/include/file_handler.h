#ifndef WHEATIS_FILE_HANDLER_H
#define WHEATIS_FILE_HANDLER_H

#include <stdio.h>

#include "wheatis_file_handler_library.h"
#include "handler.h"


typedef struct FileHandler
{
	Handler fh_base_handler;
	FILE *fh_handler_f;
} FileHandler;


#ifdef __cplusplus
extern "C" 
{
#endif


FILE_HANDLER_API Handler *GetHandler (TagItem *tags_p);

FILE_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_FILE_HANDLER_H */
