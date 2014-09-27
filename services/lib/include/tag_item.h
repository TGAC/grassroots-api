#ifndef TAG_ITEM_H
#define TAG_ITEM_H

#include "typedefs.h"
#include "parameter.h"

#include "wheatis_util_library.h"

typedef uint32 Tag;

typedef struct TagItem
{
	Tag ti_tag;
	SharedType ti_value;
} TagItem;


WHEATIS_UTIL_API TagItem *FindMatchingTag (TagItem *tags_p, Tag tag);


#define TAG_DONE 						(0L)

#define TAG_USER 						(0xFFFF0000)

#define TAG_INPUT_FILE 			(1)
#define TAG_INPUT_DIR 			(2)
#define TAG_OUTPUT_FILE 		(3)


#endif		/* #ifndef TAG_ITEM_H */
