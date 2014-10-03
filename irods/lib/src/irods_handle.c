#include <fcntl.h>

#include "dataObjOpen.h"
#include "dataObjClose.h"
#include "dataObjRead.h"
#include "dataObjWrite.h"
#include "dataObjLseek.h"
#include "fileLseek.h"

#include "irods_handle.h"
#include "connect.h"
#include "memory_allocations.h"



static bool OpenIRodsHandle (struct Handle *handle_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromIRodsHandle (struct Handle *handle_p, void *buffer_p, const size_t length);

static size_t WriteToIRodsHandle (struct Handle *handle_p, void *buffer_p, const size_t length);

static bool SeekIRodsHandle (struct Handle *handle_p, long offset, int whence);

static bool CloseIRodsHandle (struct Handle *handle_p);

static HandleStatus GetIRodsHandleStatus (struct Handle *handle_p);


Handle *GetIRodsHandle (const char * const username_s, const char * const password_s)
{
	rcComm_t *connection_p = CreateConnection ((char *) username_s, (char *) password_s);

	if (connection_p)
		{
			Handle *handle_p = AllocateIRodsHandle (connection_p);

			if (handle_p)
				{
					return handle_p;
				}

			CloseConnection (connection_p);
		}

	return NULL;
}


Handle *AllocateIRodsHandle (rcComm_t *connection_p)
{
	IRodsHandle *handle_p = (IRodsHandle *) AllocMemory (sizeof (IRodsHandle));

	if (handle_p)
		{
			handle_p -> irh_base_handle.ha_open_fn = OpenIRodsHandle;
			handle_p -> irh_base_handle.ha_close_fn = CloseIRodsHandle;
			handle_p -> irh_base_handle.ha_read_fn = ReadFromIRodsHandle;
			handle_p -> irh_base_handle.ha_write_fn = WriteToIRodsHandle;
			handle_p -> irh_base_handle.ha_seek_fn = SeekIRodsHandle;
			handle_p -> irh_base_handle.ha_status_fn = GetIRodsHandleStatus;

			handle_p -> irh_connection_p = connection_p;
			handle_p -> irh_obj_p = NULL;
			handle_p -> irh_status = HS_GOOD;
		}

	return ((Handle *) handle_p);
}


void FreeIRodsHandle (Handle *handle_p)
{
	IRodsHandle *irods_handle_p = (IRodsHandle *) handle_p;

	if (irods_handle_p -> irh_obj_p)
		{
			CloseIRodsHandle (handle_p);
		}

	FreeMemory (handle_p);
}


static bool OpenIRodsHandle (struct Handle *handle_p, const char *filename_s, const char *mode_s)
{
	IRodsHandle *irods_handle_p = (IRodsHandle *) handle_p;
	openedDataObjInp_t *opened_obj_p = NULL;

	if (irods_handle_p -> irh_obj_p)
		{
			CloseIRodsHandle (handle_p);
		}

	opened_obj_p = (openedDataObjInp_t *) AllocMemory (sizeof (openedDataObjInp_t));

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
												flags -= O_RDONLY;
											}
										else if (flags & O_WRONLY)
											{
												flags -= O_WRONLY;
											}

										flags |= O_WRONLY;
										break;
								}

							++ mode_s;
						}
				}

			data_obj.openFlags = flags;

			/* Open the object */
			handle = rcDataObjOpen (irods_handle_p -> irh_connection_p, &data_obj);

			if (handle >= 0)
				{
					memset (opened_obj_p, 0, sizeof (openedDataObjInp_t));
					opened_obj_p -> l1descInx = handle;

					irods_handle_p -> irh_obj_p = opened_obj_p;

				}		/* if (handle >= 0)) */

		}		/* if (opened_obj_p) */


	return (irods_handle_p -> irh_obj_p != NULL);
}



static bool CloseIRodsHandle (struct Handle *handle_p)
{
	bool success_flag = true;
	IRodsHandle *irods_handle_p = (IRodsHandle *) handle_p;

	if (irods_handle_p -> irh_obj_p)
		{
			success_flag = (rcDataObjClose (irods_handle_p -> irh_connection_p, irods_handle_p -> irh_obj_p) == 0);
		}

	return success_flag;
}



static size_t ReadFromIRodsHandle (struct Handle *handle_p, void *buffer_p, const size_t length)
{
	IRodsHandle *irods_handle_p = (IRodsHandle *) handle_p;
	bytesBuf_t buffer;
	int i;
	size_t res = 0;
	
	buffer.buf = buffer_p;
	buffer.len = length;
	
	irods_handle_p -> irh_obj_p -> len = length;
	
	i = rcDataObjRead (irods_handle_p -> irh_connection_p, irods_handle_p -> irh_obj_p, &buffer);

	if (i > 0)
		{
			res = (size_t) i;
			
			if (res != length)
				{
					irods_handle_p -> irh_status = HS_FINISHED;
				}
		}
	else
		{
			irods_handle_p -> irh_status = HS_BAD;
		}
		
	return res;
	
}


static size_t WriteToIRodsHandle (struct Handle *handle_p, void *buffer_p, const size_t length)
{
	IRodsHandle *irods_handle_p = (IRodsHandle *) handle_p;
	bytesBuf_t buffer;
	int i;
	size_t res = 0;
	
	buffer.buf = buffer_p;
	buffer.len = length;

	irods_handle_p -> irh_obj_p -> len = length;

	i = rcDataObjWrite (irods_handle_p -> irh_connection_p, irods_handle_p -> irh_obj_p, &buffer);
	
	if (i > 0)
		{
			res = (size_t) i;
		}
	else
		{
			irods_handle_p -> irh_status = HS_BAD;
		}

		
	return res;
}


static bool SeekIRodsHandle (struct Handle *handle_p, long offset, int whence)
{
	bool success_flag = true;
	IRodsHandle *irods_handle_p = (IRodsHandle *) handle_p;
	fileLseekOut_t *seek_p = NULL;

	irods_handle_p -> irh_obj_p -> offset = offset;
	irods_handle_p -> irh_obj_p -> whence = whence;

	success_flag = (rcDataObjLseek (irods_handle_p -> irh_connection_p, irods_handle_p -> irh_obj_p, &seek_p) == 0);

	return success_flag;
}


static HandleStatus GetIRodsHandleStatus (struct Handle *handle_p)
{
	IRodsHandle *irods_handle_p = (IRodsHandle *) handle_p;

	return (irods_handle_p -> irh_status);	
}

