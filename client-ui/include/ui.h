/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#ifndef CLIENT_UI_H
#define CLIENT_UI_H


#include "plugin.h"


/* forward declaration */
struct ClientUI;
 
typedef struct ClientData
{
	/** The client that owns this data. */
	struct ClientUI *cd_client_p;
} ClientData;
 

typedef struct ClientUI
{
	struct Plugin *se_plugin_p;
	
	int (*se_run_fn) (const char * const json_s);
	
	ClientData *se_data_p;
} ClientUI;



#endif		/* #ifndef CLIENT_UI_H */
