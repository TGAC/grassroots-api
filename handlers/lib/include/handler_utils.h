#ifndef HANDLE_UTILS_H
#define HANDLE_UTILS_H


#include "wheatis_handle_library.h"
#include "parameter.h"
#include "handler.h"


#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_HANDLE_API Handler *GetResourceHandler (const Resource *resource_p, const TagItem *tags_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef HANDLE_UTILS_H */
