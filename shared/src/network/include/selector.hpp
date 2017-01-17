/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
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
#include "curl_tools.h"


/**
 * @brief A datatype representing an http link.
 *
 * @ingroup network_group
 */
typedef struct HtmlLink
{
	/** A title for the link */
	char *hl_title_s;

	/** The URI of the link. */
	char *hl_uri_s;

	/** The CDATA section of the link. */
	char *hl_data_s;
} HtmlLink;


/**
 * A collection of HtmlLinks
 *
 * @ingroup network_group
 */
typedef struct HtmlLinkArray
{
	/** An array of HtmlLinks */
	HtmlLink *hla_data_p;

	/** The number of HtmlLinks in this collection */
	size_t hla_num_entries;
} HtmlLinkArray;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Run a CurlTool and select a subset of the data.
 * This will set up a CurlTool to a given URI and download the data. If successful, it
 * will then extract a set of HtmlLinks using the given selector.
 * @param tool_p The CurlTool to use.
 * @param uri_s The URI for the CurlTool to download from.
 * @param link_selector_s The CSS selector to use to extract the HtmlLinkArray with.
 * @param title_selector_s The CSS Selector for getting the link's title. If this is <code>NULL</code>,
 * then the inner text of the element found by link_selector_s will be used.
 * @return A newly-allocated HtmlLinkArray if successful or <code>NULL</code>
 * upon error.
 * @memberof HtmlLinkArray
 * @see GetMatchingLinks
 */
GRASSROOTS_NETWORK_API HtmlLinkArray *GetLinks (CurlTool *tool_p, const char * const uri_s, const char * const link_selector_s, const char * const title_selector_s);



/**
 * Get an HtmlLinkArray in JSON format. This converts the output of <code>GetMatchingLinks</code>
 * into a JSON fragment.
 *
 * @param data_s The HTML data as a string.
 * @param link_selector_s The CSS Selector for getting the uri link.
 * @param title_selector_s The CSS Selector for getting the link's title. If this is <code>NULL</code>,
 * then the inner text of the element found by link_selector_s will be used.
 * @param base_uri_s The URI to prepend to any links. This is useful when the links in data_s are local
 * to the server emitting data_s. The resulting links will be absolute full URIs.
 * @return The JSON fragment or <code>NULL</code> upon error.
 * @memberof HtmlLinkArray
 * @see GetMatchingLinks
 */
GRASSROOTS_NETWORK_API json_t *GetMatchingLinksAsJSON (const char * const data_s, const char * const link_selector_s, const char * const title_selector_s, const char * const base_uri_s);


/**
 * Get an HtmlLinkArray from an HTML fragment.
 *
 * @param data_s The HTML data as a string.
 * @param link_selector_s The CSS Selector for getting the uri link.
 * @param title_selector_s The CSS Selector for getting the link's title. If this is <code>NULL</code>,
 * then the inner text of the element found by link_selector_s will be used.
 * @param base_uri_s The URI to prepend to any links. This is useful when the links in data_s are local
 * to the server emitting data_s. The resulting links will be absolute full URIs.
 * @return The HtmlLinkArray or <code>NULL</code> upon error.
 * @memberof HtmlLinkArray
 * @see FreeHtmlLinkArray
 */
GRASSROOTS_NETWORK_API HtmlLinkArray *GetMatchingLinks (const char * const data_s, const char * const link_selector_s, const char * const title_selector_s, const char * const base_uri_s);

/**
 * Free a HtmlLinkArray
 *
 * @param links_p The HtmlLinkArray to free.
 * @memberof HtmlLinkArray
 */
GRASSROOTS_NETWORK_API void FreeHtmlLinkArray (HtmlLinkArray *links_p);


#ifdef __cplusplus
}
#endif


#endif
