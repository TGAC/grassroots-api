
#include <stdio.h>

#include "curl_tools.h"





bool IsSelectorMatch (GumboNode *node_p, GumboSelectorNode *selector_node_p)
{
	bool match_flag = false;

	if (node_p -> type == GUMBO_NODE_ELEMENT)
		{
			GumboElement *el_p = & (node_p -> v.element);
			GumboSelector *selector_p = selector_node_p -> gsn_selector_p;

			/*
			 * Check for matching id
			 */
			if (selector_p -> gs_id_s)
				{
					GumboAttribute *attr_p = gumbo_get_attribute (el_p -> attributes, "id");

					if (attr_p)
						{
							match_flag = (strcmp (attr_p -> value, selector_p -> gs_id_s) == 0);
						}
				}

			if (!match_flag)
				{
					/* check for matching element */
					if (selector_p -> gs_tag == el_p -> tag)
						{
							const char *CLASS_S [] = "class";
							GumboAttribute *attr_p = gumbo_get_attribute (el_p -> attributes, CLASS_S);

							if (attr_p)
								{
									const char *value_s = GetGumboSelectorAttributeValue (selector_p, CLASS_S);

									if (value_s)
										{
											match_flag = (strcmp (attr_p -> value, value_s) == 0);
										}

								}		/* if (attr_p) */
							else
								{
									match_flag = true;
								}

						}		/* if (selector_p -> gs_tag == el_p -> tag) */

				}		/* if (!match_flag) */

		}		/* if (node_p -> type == GUMBO_NODE_ELEMENT) */

	return match_flag;
}


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
