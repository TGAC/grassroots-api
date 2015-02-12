/*
 * selector.cpp
 *
 *  Created on: 12 Feb 2015
 *      Author: tyrrells
 */

#include <string>

#include <htmlcxx/html/ParserDom.h>

#include <hcxselect.h>

#include "selector.hpp"
#include "streams.h"
#include "memory_allocations.h"


using namespace std;
using namespace htmlcxx :: HTML;
//using namespace hcxselect;


static HtmlLinkArray *AllocateHtmlLinksArrayFromSet (const hcxselect :: Selector &selector_r);


HtmlLinkArray *GetMatchingLinks (const char * const data_s, const char * const selector_s)
{
	string source (data_s);
	ParserDom parser;
	tree <Node> dom = parser.parseTree (source);
	hcxselect :: Selector s (dom);
	bool success_flag = false;
	HtmlLinkArray *links_p = NULL;

	try
		{
			s = s.select (selector_s);
			success_flag = true;
		}
	catch (hcxselect::ParseException &ex)
		{
			PrintErrors (STM_LEVEL_SEVERE, "Parse error on '%s' - %s\n", selector_s, ex.what ());
		}
	catch (...)
		{
			PrintErrors (STM_LEVEL_SEVERE, "Error parsing '%s'\n", selector_s);
		}

	if (success_flag)
		{
			links_p = AllocateHtmlLinksArrayFromSet (s);

		}

	return links_p;
}


static HtmlLinkArray *AllocateHtmlLinksArrayFromSet (const hcxselect :: Selector &selector_r)
{
	HtmlLinkArray *links_p = NULL;
	const size_t num_links = selector_r.size ();

	HtmlLink *data_p = (HtmlLink *) AllocMemory (num_links * sizeof (HtmlLink));

	if (data_p)
		{
			HtmlLink *link_p = data_p;

			for (hcxselect::Selector::const_iterator it = selector_r.begin (); it != selector_r.end (); ++ it, ++ link_p)
				{
					Node *node_p = & ((*it) -> data);

					const std string &text_r = node_p -> text ();

				}

			FreeMemory (data_p);
		}		/* if (data_p) */


	return links_p;
}

void FreeHtmlLinkArray (HtmlLinkArray *links_p)
{

}
