#ifndef TAG_ITEM_H
#define TAG_ITEM_H

#include "typedefs.h"
#include "parameter.h"
#include "tags.h"

#include "wheatis_service_library.h"



#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_SERVICE_API TagItem *FindMatchingTag (TagItem *tags_p, Tag tag);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef TAG_ITEM_H */
