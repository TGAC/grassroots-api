/*
 * key_value_pair.h
 *
 *  Created on: 11 Feb 2015
 *      Author: tyrrells
 */

#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

#include "network_library.h"

/**
 * A pair of strings.
 */
typedef struct KeyValuePair
{
	/** The string used as a key */
	char *kvp_key_s;

	/** The string used as a value */
	char *kvp_value_s;
} KeyValuePair;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Allocate a KeyValuePair.
 *
 * @param key_s This string will be deep copied and be the kvp_key_s in the KeyValuePair.
 * @param key_s This string will be deep copied and be the kvp_value_s in the KeyValuePair.
 * @return A newly-allocated KeyValuePair or <code>NULL</code> upon error.
 * @memberof KeyValuePair
 */
WHEATIS_NETWORK_API KeyValuePair *AllocateKeyValuePair (const char *key_s, const char *value_s);


/**
 * Free a KeyValuePair.
 *
 * @param kvp_pThe KeyValuePair to free.
 * @memberof KeyValuePair
 */
WHEATIS_NETWORK_API void FreeKeyValuePair (KeyValuePair *kvp_p);


#ifdef __cplusplus
}
#endif

#endif /* KEY_VALUE_PAIR_H_ */
