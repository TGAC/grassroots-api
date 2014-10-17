#include <string.h>

#include "zip.h"
#include "file_handler.h"

/* compress or decompress from stdin to stdout */
int main(int argc, char **argv)
{
	int ret = 0;

	if (argc == 4)
		{
			Handler *in_p = AllocateFileHandler ();
			
			if (in_p)
				{
					Handler *out_p = AllocateFileHandler ();
					
					if (out_p)
						{
							if (OpenHandler (in_p, argv [1], "rb"))
								{
									if (OpenHandler (out_p, argv [2], "w"))
										{
											if (strcmp (argv [3], "gzip") == 0)
												{
													puts ("running gzip");
													ret = CompressAsGZip (in_p, out_p, Z_DEFAULT_COMPRESSION);
												}
																					
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
