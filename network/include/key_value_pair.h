/*
 * key_value_pair.h
 *
 *  Created on: 11 Feb 2015
 *      Author: tyrrells
 */

#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

#include "network_library.h"

typedef struct KeyValuePair
{
	char *kvp_key_s;
	char *kvp_value_s;
} KeyValuePair;


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_NETWORK_API KeyValuePair *AllocateKeyValuePair (const char *key_s, const char *value_s);

WHEATIS_NETWORK_API void FreeKeyValuePair (KeyValuePair *kvp_p);

#ifdef __cplusplus
}
#endif

#endif /* KEY_VALUE_PAIR_H_ */
