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
