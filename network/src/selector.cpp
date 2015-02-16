/*
 * selector.cpp
 *
 *  Created on: 12 Feb 2015
 *      Author: tyrrells
 */

#include <string>
#include <iostream>

#include <htmlcxx/html/ParserDom.h>

#include <hcxselect.h>

#include "selector.hpp"
#include "streams.h"
#include "memory_allocations.h"
#include "string_utils.h"

using namespace std;
using namespace htmlcxx :: HTML;
//using namespace hcxselect;


static HtmlLinkArray *AllocateHtmlLinksArray (const size_t num_links);

static HtmlLinkArray *AllocateHtmlLinksArrayFromSet (const hcxselect :: Selector &selector_r, const char * const data_s);

static void ClearHtmlLink (HtmlLink *link_p);
static char *GetInnerText (const Node *node_p, const char *data_s);

static bool InitHtmlLink (HtmlLink *link_p, const char *title_s, const char *uri_s, const char *data_s);


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
			links_p = AllocateHtmlLinksArrayFromSet (s, data_s);

		}

	return links_p;
}


static HtmlLinkArray *AllocateHtmlLinksArray (const size_t num_links)
{
	HtmlLink *data_p = (HtmlLink *) AllocMemoryArray (num_links, sizeof (HtmlLink));

	if (data_p)
		{
			HtmlLinkArray *links_p = (HtmlLinkArray *) AllocMemory (sizeof (HtmlLinkArray));

			if (links_p)
				{
					links_p -> hla_data_p = data_p;
					links_p -> hla_num_entries = num_links;

					return links_p;
				}

			FreeMemory (data_p);
		}

	return NULL;
}

static HtmlLinkArray *AllocateHtmlLinksArrayFromSet (const hcxselect :: Selector &selector_r, const char * const data_s)
{
	const size_t num_links = selector_r.size ();
	HtmlLinkArray *links_p = AllocateHtmlLinksArray (num_links);

	if (links_p)
		{
			HtmlLink *link_p = links_p -> hla_data_p;

			for (hcxselect::Selector::const_iterator it = selector_r.begin (); it != selector_r.end (); ++ it, ++ link_p)
				{
					Node *node_p = & ((*it) -> data);
					const string &tag_name_r = node_p -> tagName ();

					const map <string, string> &attrs_r = node_p -> attributes ();

					size_t num_attrs = attrs_r.size();

					node_p -> parseAttributes ();

					num_attrs = attrs_r.size();

					cout << "node: " << *node_p << endl;

					if ((tag_name_r.compare ("a") == 0) || ((tag_name_r.compare ("A") == 0)))
						{
							pair <bool, string> p = node_p -> attribute ("href");

							if (p.first == false)
								{
									p = node_p -> attribute ("HREF");
								}

							if (p.first == true)
								{
									const string &uri_r = p.second;
									const char *title_s = NULL;
									char *inner_text_s = GetInnerText (node_p, data_s);

									if (inner_text_s)
										{
											pair <bool, string> title_attr = node_p -> attribute ("title");

											if (title_attr.first == true)
												{
													title_s = title_attr.second.c_str ();
												}

											if (!InitHtmlLink (link_p, title_s, uri_r.c_str (), inner_text_s))
												{

												}

											FreeCopiedString (inner_text_s);
										}

								}
						}
				}
		}		/* if (links_p) */


	return links_p;
}



static char *GetInnerText (const Node *node_p, const char *data_s)
{
	char *inner_text_s = NULL;
	const char *start_p = data_s + (node_p -> offset ()) + 1;

	while (*start_p && (*start_p != '>'))
		{
			++ start_p;
		}

	if (start_p)
		{
			const char *end_p = data_s + (node_p -> offset ()) + (node_p -> length ()) - 1;

			while (*end_p && (*end_p != '<') && (start_p < end_p))
				{
					-- end_p;
				}

			if (end_p)
				{
					/* scroll past the < and > */
					-- end_p;
					++ start_p;

					inner_text_s = CopyToNewString (start_p, end_p - start_p, true);
				}
		}

	return inner_text_s;
}


void FreeHtmlLinkArray (HtmlLinkArray *links_p)
{
	size_t i = links_p -> hla_num_entries;
	HtmlLink *link_p = links_p -> hla_data_p;

	for ( ; i > 0; -- i, ++ link_p)
		{
			ClearHtmlLink (link_p);
		}

	FreeMemory (links_p);
}


static void ClearHtmlLink (HtmlLink *link_p)
{
	if (link_p -> hl_uri_s)
		{
			FreeCopiedString (link_p -> hl_uri_s);
		}

	if (link_p -> hl_title_s)
		{
			FreeCopiedString (link_p -> hl_title_s);
		}

	if (link_p -> hl_data_s)
		{
			FreeCopiedString (link_p -> hl_data_s);
		}

}


static bool InitHtmlLink (HtmlLink *link_p, const char *title_s, const char *uri_s, const char *data_s)
{
	char *value_s = CopyToNewString (uri_s, 0, false);

	link_p -> hl_uri_s = NULL;
	link_p -> hl_data_s = NULL;
	link_p -> hl_title_s = NULL;

	if (value_s)
		{
			link_p -> hl_uri_s = value_s;

			value_s = CopyToNewString (data_s, 0, false);

			if (value_s)
				{
					link_p -> hl_data_s = value_s;

					link_p -> hl_title_s = NULL;

					if (title_s)
						{
							value_s = CopyToNewString (title_s, 0, false);

							if (value_s)
								{
									link_p -> hl_title_s = value_s;
								}
						}

					return true;
				}

			FreeCopiedString (link_p -> hl_uri_s);
			link_p -> hl_uri_s = NULL;
		}

	return false;
}
