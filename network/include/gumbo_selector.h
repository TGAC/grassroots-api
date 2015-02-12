/*
 * gumbo_selector.h
 *
 *  Created on: 11 Feb 2015
 *      Author: tyrrells
 */

#ifndef GUMBO_SELECTOR_H
#define GUMBO_SELECTOR_H

#include "gumbo.h"
#include "linked_list.h"
#include "network_library.h"




typedef struct GumboSelector
{
	GumboTag gs_tag;
	const char *gs_id_s;
	const char *gs_class_s;
	KeyValuePair *gs_attrs_p;
	size_t gs_num_attrs;
} GumboSelector;


typedef struct GumboSelectorNode
{
	ListItem gsn_node;
	GumboSelector *gsn_selector_p;
} GumboSelectorNode;


#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_NETWORK_API GumboSelector *AllocateGumboSelector (const char *selector_s);


WHEATIS_NETWORK_API void FreeGumboSelector (GumboSelector *selector_p);


WHEATIS_NETWORK_API GumboSelectorNode *AllocateGumboSelectorNode (GumboSelector *selector_p);


WHEATIS_NETWORK_API void FreeGumboSelectorNode (ListItem *node_p);


WHEATIS_NETWORK_API const char *GetGumboSelectorAttributeValue (const GumboSelector * const selector_p, const char * const key_s);


#ifdef __cplusplus
}
#endif

#endif /* GUMBO_SELECTOR_H_ */
