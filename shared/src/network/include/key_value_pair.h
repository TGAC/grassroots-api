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
 * key_value_pair.h
 *
 *  Created on: 11 Feb 2015
 *      Author: tyrrells
 */

#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

#include "network_library.h"
#include "linked_list.h"

/**
 * A pair of strings.
 *
 * @ingroup network_group
 */
typedef struct KeyValuePair
{
	/** The string used as a key */
	char *kvp_key_s;

	/** The string used as a value */
	char *kvp_value_s;
} KeyValuePair;


/**
 * A datatype used for storing a KeyValuePair
 * on a LinkedList.
 *
 * @extends ListItem
 *
 * @ingroup network_group
 */
typedef struct KeyValuePairNode
{
	/** The base node. */
	ListItem kvpn_node;

	/** The KeyValuePair. */
	KeyValuePair *kvpn_pair_p;
} KeyValuePairNode;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate a KeyValuePair.
 *
 * @param key_s This string will be deep copied and be the kvp_key_s in the KeyValuePair.
 * @param value_s This string will be deep copied and be the kvp_value_s in the KeyValuePair.
 * @return A newly-allocated KeyValuePair or <code>NULL</code> upon error.
 * @memberof KeyValuePair
 */
GRASSROOTS_NETWORK_API KeyValuePair *AllocateKeyValuePair (const char *key_s, const char *value_s);


/**
 * Free a KeyValuePair.
 *
 * @param kvp_p The KeyValuePair to free.
 * @memberof KeyValuePair
 */
GRASSROOTS_NETWORK_API void FreeKeyValuePair (KeyValuePair *kvp_p);


/**
 * Allocate a KeyValuePairNode.
 *
 * @param key_s This string will be deep copied and be the kvp_key_s in the KeyValuePair.
 * @param value_s This string will be deep copied and be the kvp_value_s in the KeyValuePair.
 * @return A newly-allocated KeyValuePairNode containing the KeyValuePair or <code>NULL</code> upon error.
 * @memberof KeyValuePair
 */
GRASSROOTS_NETWORK_API KeyValuePairNode *AllocateKeyValuePairNode (const char *key_s, const char *value_s);


/**
 * Free a KeyValuePairNode.
 *
 * @param node_p The KeyValuePairNode to free.
 * @memberof KeyValuePair
 */
GRASSROOTS_NETWORK_API void FreeKeyValuePairNode (ListItem *node_p);



#ifdef __cplusplus
}
#endif

#endif /* KEY_VALUE_PAIR_H_ */
