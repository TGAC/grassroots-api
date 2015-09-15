/*
 * provider.c
 *
 *  Created on: 15 Sep 2015
 *      Author: tyrrells
 */

#include "provider.h"

#include "json_tools.h"
#include "json_util.h"


const char *GetProviderName (const json_t * const data_p)
{
	return GetJSONString (data_p, PROVIDER_NAME_S);
}


const char *GetProviderURI (const json_t * const data_p)
{
	return GetJSONString (data_p, PROVIDER_URI_S);
}


const char *GetProviderDescription (const json_t * const data_p)
{
	return GetJSONString (data_p, PROVIDER_DESCRIPTION_S);
}
