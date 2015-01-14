
#include "curl_tools.h"

int main (int argc, char *argv [])
{
	if (argc == 3)
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);
			
			if (buffer_p)
				{
					char *header_data_s = "";
					char *cryptograph_engine_name_s = "";
					char *certificate_name_s = "";
					bool verify_certs = false;		
					bool success_flag = CallSecureUrl (argv [1], header_data_s, cryptograph_engine_name_s, argv [2], verify_certs, buffer_p);

					printf ("%d\n%s\n", (int) success_flag, buffer_p -> bb_data_p);

					FreeByteBuffer (buffer_p);
				}		/* if (buffer_p) */
		
		}		
		
	return 0;
}
