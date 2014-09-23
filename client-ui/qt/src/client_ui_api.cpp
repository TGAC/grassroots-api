#include "client_ui_api.h"

#include "memory_allocations.h"


static const char *GetQTClientName (void);
static const char *GetQTClientDescription (void);
static int RunQTClient (ClientData *client_data_p, const char * const filename_s, ParameterSet *param_set_p);


Client *AllocateQTClient (void)
{
	Client *client_p = (Client *) AllocMemory (sizeof (Client));

	if (client_p)
		{
			InitialiseClient (client_p, GetQTClientName, GetQTClientDescription, RunQTClient, NULL);
		}

	return client_p;
}


void *FreeQTClient (Client *client_p)
{
	FreeMemory (client_p);
}


static const char *GetQTClientName (void)
{
	return "Qt-based WheatIS client";
}


static const char *GetQTClientDescription (void)
{
	return "Qt-based WheatIS client";
}


static int RunQTClient (ClientData *client_data_p, const char * const filename_s, ParameterSet *param_set_p)
{
	int res = -1;

	return res;
}

