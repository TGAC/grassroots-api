
#include <stdio.h>

#include "curl_tools.h"
#include "gumbo.h"



typedef struct KeyValuePair
{
	char *kvp_key_s;
	char *kvp_value_s;
} KeyValuePair;


typedef struct GumboSelector
{
	GumboTag gs_tag;
	const char *gs_id_s;
	KeyValuePair *gs_attrs_p;
	size_t gs_num_attrs;
} GumboSelector;


typedef struct GumboSelectorNode
{
	ListNode gsn_node;
	GumboSelector gsn_selector;
} GumboSelectorNode;


/**
 * Allocate a Selector.
 *
 * @param selector_s The selector in the css sleector syntax
 * e.g. "p.header", "ul#results".
 * @return A newly-allocated Selector representing the given
 * selection string or NULL upon error.
 */
GumboSelector *AllocateGumboSelector (const char *selector_s)
{

}


void FreeGumboSelector (GumboSelector *selector_p)
{

}


KeyValuePair *AllocateKeyValuePair (const char *key_s, const char *value_s)
{
	char *copied_key_s = CopyToNewString (key_s, 0, false);

	if (copied_key_s)
		{
			char *copied_value_s = CopyToNewString (value_s, 0, false);

			if (copied_value_s)
				{
					KeyValuePair *kvp_p = (KeyValuePair *) AllocMemory (sizeof (KeyValuePair));

					if (kvp_p)
						{
							kvp_p -> kvp_key_s = copied_key_s;
							kvp_p -> kvp_value_s = copied_value_s;

							return kvp_p;
						}

					FreeCopiedString (copied_value_s);
				}		/* if (copied_value_s) */

			FreeCopiedString (copied_key_s);
		}		/* if (copied_key_s) */

	return NULL;
}


void FreeKeyValuePair (KeyValuePair *kvp_p)
{
	FreeCopiedString (kvp_p -> kvp_key_s);
	FreeCopiedString (kvp_p -> kvp_value_s);
	FreeMemory (kvp_p);
}


bool IsSelectorMatch (GumboNode *node_p, GumboSelector *selector_p)

void DoGumboSelector (GumboNode *node_p, GumboSelector *selector_p)
{
	GumboVector *children_p;
	GumboElement *el_p = & (node_p -> v.element);

	if (node_p -> type == GUMBO_NODE_ELEMENT)
		{
			GumboElement *el_p = & (node_p -> v.element);
			GumboVector *children_p  = & (el_p -> children);
		}
}


int main (int argc, char **argv )
{
	CurlTool *tool_p = AllocateCurlTool ();

	if (tool_p)
		{
			if (SetUriForCurlTool (tool_p, argv [1]))
				{
					CURLcode res = RunCurlTool (tool_p);

					if (res == CURLE_OK)
						{
							const char *data_p = GetCurlToolData (tool_p);
							GumboOutput *gumbo_p = gumbo_parse (data_p);

							if (gumbo_p)
								{
									DoGumboSelector (gumbo_p -> root);
									gumbo_destroy_output (&kGumboDefaultOptions, gumbo_p);
								}		/* if (gumbo_p) */

						}		/* if (res == CURLE_OK) */

				}		/* if (SetUriForCurlTool (tool_p, argv [1])) */

			FreeCurlTool (tool_p);
		}		/* if (tool_p) */
;

  return(0);
}
