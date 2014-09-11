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
