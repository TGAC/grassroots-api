
#include "curl_tools.h"

int main (int argc, char *argv [])
{
	if (argc == 2)
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);
			
			if (buffer_p)
				{
					char *header_data_s = NULL;
					char *cryptograph_engine_name_s = NULL;
					char *certificate_name_s = NULL;
					bool verify_certs = false;		
					bool success_flag = CallSecureUrl (argv [1], header_data_s, cryptograph_engine_name_s, certificate_name_s, verify_certs, buffer_p);

					printf ("%d\n%s\n", (int) success_flag, buffer_p -> bb_data_p);

					FreeByteBuffer (buffer_p);
				}		/* if (buffer_p) */
		
		}		
		
	return 0;
}
