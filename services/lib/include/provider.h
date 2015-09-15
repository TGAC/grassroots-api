/*
 * provider.h
 *
 *  Created on: 15 Sep 2015
 *      Author: tyrrells
 */

#ifndef PROVIDER_H_
#define PROVIDER_H_

#include "wheatis_service_library.h"
#include "jansson.h"


#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_SERVICE_API const char *GetProviderName (const json_t * const data_p);

WHEATIS_SERVICE_API const char *GetProviderURI (const json_t * const data_p);

WHEATIS_SERVICE_API const char *GetProviderDescription (const json_t * const data_p);


#ifdef __cplusplus
}
#endif


#endif /* PROVIDER_H_ */
