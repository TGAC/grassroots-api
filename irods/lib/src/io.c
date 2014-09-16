#include "rcConnect.h"

#include "irods_stream.h"
#include "io.h"


Stream *GetIRodsStream (char * const username_s, char * const password_s)
{
	rcComm_t *connection_p = CreateConnection (username_s, password_s);
	
	if (connection_p)
		{
			Stream *stream_p = AllocateIRodsStream (connection_p);
			
			if (stream_p)
				{
					return stream_p;
				}
			
			CloseConnection (connection_p);
		}
		
	return NULL;
}

