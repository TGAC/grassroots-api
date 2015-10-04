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

/**
 * @brief A datatype representing an http link.
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
 * @param links_ The HtmlLinkArray to free.
 * @memberof HtmlLinkArray
 */
GRASSROOTS_NETWORK_API void FreeHtmlLinkArray (HtmlLinkArray *links_p);


#ifdef __cplusplus
}
#endif


#endif
