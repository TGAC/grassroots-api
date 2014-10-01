#include "zip.h"
#include "file_stream.h"

/* compress or decompress from stdin to stdout */
int main(int argc, char **argv)
{
	int ret = 0;

	if (argc == 4)
		{
			Stream *in_p = AllocateFileStream ();
			
			if (in_p)
				{
					Stream *out_p = AllocateFileStream ();
					
					if (out_p)
						{
							if (OpenStream (in_p, argv [1], "rb"))
								{
									if (OpenStream (out_p, argv [2], "w"))
										{
											ret = CompressAsZip (in_p, out_p, Z_DEFAULT_COMPRESSION);
																						
											CloseStream (out_p);
										}
			
									CloseStream (in_p);
								}
								
							FreeFileStream (out_p);
						}		/* if (out_p) */
				
					FreeFileStream (in_p);
				}		/* if (in_p) */	
		}
		
	return ret;
}
