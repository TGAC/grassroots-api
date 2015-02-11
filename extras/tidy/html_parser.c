
#include <stdio.h>

#include "curl_tools.h"





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
