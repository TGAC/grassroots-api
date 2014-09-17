#include <fcntl.h>

#include "dataObjOpen.h"
#include "dataObjClose.h"
#include "dataObjRead.h"
#include "dataObjLseek.h"
#include "fileLseek.h"

#include "irods_stream.h"
#include "memory_allocations.h"


static bool OpenIRodsStream (struct Stream *stream_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromIRodsStream (struct Stream *stream_p, void *buffer_p, const size_t length);

static bool SeekIRodsStream (struct Stream *stream_p, long offset, int whence);

static bool CloseIRodsStream (struct Stream *stream_p);


Stream *AllocateIRodsStream (rcComm_t *connection_p)
{
	IRodsStream *stream_p = (IRodsStream *) malloc (sizeof (IRodsStream));
	
	if (stream_p)
		{
			stream_p -> irs_base_stream.st_open_fn = OpenIRodsStream;
			stream_p -> irs_base_stream.st_close_fn = CloseIRodsStream;
			stream_p -> irs_base_stream.st_read_fn = ReadFromIRodsStream;
			stream_p -> irs_base_stream.st_seek_fn = SeekIRodsStream;
			
			stream_p -> irs_connection_p = connection_p;
			stream_p -> irs_obj_p = NULL;
		}
		
	return ((Stream *) stream_p);
}


void FreeIRodsStream (Stream *stream_p)
{
	IRodsStream *irods_stream_p = (IRodsStream *) stream_p;
	
	if (irods_stream_p -> irs_obj_p)
		{
			CloseIRodsStream (stream_p);
		}
		
	FreeMemory (stream_p);
}




static bool OpenIRodsStream (struct Stream *stream_p, const char *filename_s, const char *mode_s)
{
	IRodsStream *irods_stream_p = (IRodsStream *) stream_p;
	openedDataObjInp_t *opened_obj_p = NULL;
	
	if (irods_stream_p -> irs_obj_p)
		{
			CloseIRodsStream (stream_p);
		}
	
	opened_obj_p = (openedDataObjInp_t *) malloc (sizeof (openedDataObjInp_t));
			
	if (opened_obj_p)
		{
			dataObjInp_t data_obj;
			int handle;
			int flags = 0;
			
			/* Set up the input data */
			memset (&data_obj, 0, sizeof (dataObjInp_t));			
			rstrcpy (data_obj.objPath, (char *) filename_s, MAX_NAME_LEN); 
			
			
			if (mode_s)
				{
					while (*mode_s != '\0')
						{
							switch (*mode_s)
								{
									case 'r':
										flags |= O_RDONLY;
										break;

									case 'w':
										flags |= O_WRONLY;
										flags |= O_TRUNC;
										flags |= O_CREAT;
										break;

									case 'a':
										flags |= O_WRONLY;
										flags |= O_CREAT;
										break;										

									case '+':
										if (flags & O_RDONLY)
											{
												flags = flags & ~O_RDONLY;	
											}
										else if (flags & O_WRONLY)
											{
												flags = flags & ~O_WRONLY;	
											}
	
										flags |= O_WRONLY;										
										break;
								}
															
							++ mode_s;
						}
				}
			
			data_obj.openFlags = flags; 
				
			/* Open the object */					
			handle = rcDataObjOpen (irods_stream_p -> irs_connection_p, &data_obj);

			if (handle >= 0)
				{					
					memset (opened_obj_p, 0, sizeof (openedDataObjInp_t));
					opened_obj_p -> l1descInx = handle;			
					
					irods_stream_p -> irs_obj_p = opened_obj_p;
						
				}		/* if (handle >= 0)) */
			
		}		/* if (opened_obj_p) */
	
	
	return (irods_stream_p -> irs_obj_p != NULL);
}



static bool CloseIRodsStream (struct Stream *stream_p)
{
	bool success_flag = true;
	IRodsStream *irods_stream_p = (IRodsStream *) stream_p;
	
	if (irods_stream_p -> irs_obj_p)
		{
			success_flag = (rcDataObjClose (irods_stream_p -> irs_connection_p, irods_stream_p -> irs_obj_p) == 0); 
		}
		
	return success_flag;
}



static size_t ReadFromIRodsStream (struct Stream *stream_p, void *buffer_p, const size_t length)
{
	IRodsStream *irods_stream_p = (IRodsStream *) stream_p;
	bytesBuf_t buffer;
	
	buffer.buf = buffer_p;
	buffer.len = length;

	irods_stream_p -> irs_obj_p -> len = length;
	
	return rcDataObjRead (irods_stream_p -> irs_connection_p, irods_stream_p -> irs_obj_p, &buffer);	
}




static bool SeekIRodsStream (struct Stream *stream_p, long offset, int whence)
{
	bool success_flag = true;
	IRodsStream *irods_stream_p = (IRodsStream *) stream_p;
	fileLseekOut_t *seek_p = NULL;
	
	irods_stream_p -> irs_obj_p -> offset = offset; 
	irods_stream_p -> irs_obj_p -> whence = whence; 
	
	success_flag = (rcDataObjLseek (irods_stream_p -> irs_connection_p, irods_stream_p -> irs_obj_p, &seek_p) == 0); 

	return success_flag;
}
