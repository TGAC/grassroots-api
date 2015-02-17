/*
 * selector.h
 *
 *  Created on: 11 Feb 2015
 *      Author: tyrrells
 */

#ifndef CSS_SELECTOR_HPP
#define CSS_SELECTOR_HPP

#include "network_library.h"
#include "jansson.h"

typedef struct HtmlLink
{
	char *hl_title_s;
	char *hl_uri_s;
	char *hl_data_s;
} HtmlLink;


typedef struct HtmlLinkArray
{
	HtmlLink *hla_data_p;
	size_t hla_num_entries;
} HtmlLinkArray;


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_NETWORK_API json_t *GetMatchingLinksAsJSON (const char * const data_s, const char * const selector_s);

WHEATIS_NETWORK_API HtmlLinkArray *GetMatchingLinks (const char * const data_s, const char * const selector_s);

WHEATIS_NETWORK_API void FreeHtmlLinkArray (HtmlLinkArray *links_p);


#ifdef __cplusplus
}
#endif


#endif
