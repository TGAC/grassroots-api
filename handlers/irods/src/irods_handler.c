#include <fcntl.h>

#include "dataObjOpen.h"
#include "dataObjClose.h"
#include "dataObjRead.h"
#include "dataObjWrite.h"
#include "dataObjLseek.h"
#include "fileLseek.h"

#include "irods_handler.h"
#include "connect.h"
#include "memory_allocations.h"
#include "json_tools.h"
#include "math_utils.h"



#ifdef _DEBUG
	#define IRODS_HANDLER_DEBUG	(DL_INFO)
#else
	#define IRODS_HANDLER_DEBUG	(DL_NONE)
#endif


static bool InitIRodsHandler (struct Handler *handler_p);

static bool OpenIRodsHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromIRodsHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

static size_t WriteToIRodsHandler (struct Handler *handler_p, const void *buffer_p, const size_t length);

static bool SeekIRodsHandler (struct Handler *handler_p, long offset, int whence);

static bool CloseIRodsHandler (struct Handler *handler_p);

static HandlerStatus GetIRodsHandlerStatus (struct Handler *handler_p);

static bool IsResourceForIRodsHandler (struct Handler *handler_p, const Resource * resource_p);

static const char *GetIRodsHandlerProtocol (struct Handler *handler_p);

static const char *GetIRodsHandlerName (struct Handler *handler_p);

static const char *GetIRodsHandlerDescription (struct Handler *handler_p);

static void FreeIRodsHandler (struct Handler *handler_p);

static bool IsResourceForIRodsHandler (struct Handler *handler_p, const Resource * resource_p);

static bool CalculateFileInformationFromIRodsHandler (struct Handler *handler_p, FileInformation *info_p);


/*
Handler *GetIRodsHandler (const char * const username_s, const char * const password_s)
{
	rcComm_t *connection_p = CreateConnection ((char *) username_s, (char *) password_s);

	if (connection_p)
		{
			Handler *handler_p = AllocateIRodsHandler (connection_p);

			if (handler_p)
				{
					return handler_p;
				}

			CloseConnection (connection_p);
		}

	return NULL;
}
*/

Handler *GetHandler (const json_t *credentials_p)
{
	IRodsHandler *handler_p = (IRodsHandler *) AllocMemory (sizeof (IRodsHandler));

	if (handler_p)
		{
			const char *tags_s = json_dumps (credentials_p, 0);
			const char *username_s = NULL;
			const char *password_s = NULL; 
			
			if (GetUsernameAndPassword (credentials_p, &username_s, &password_s))
				{
					rcComm_t *connection_p = CreateConnection ((char *) username_s, (char *) password_s);

					if (connection_p)
						{
							InitialiseHandler (& (handler_p -> irh_base_handler),
								InitIRodsHandler,
								IsResourceForIRodsHandler,
								GetIRodsHandlerProtocol,
								GetIRodsHandlerName,
								GetIRodsHandlerDescription,
								OpenIRodsHandler,
								ReadFromIRodsHandler,
								WriteToIRodsHandler,
								SeekIRodsHandler,
								CloseIRodsHandler,
								GetIRodsHandlerStatus,
								CalculateFileInformationFromIRodsHandler,
								FreeIRodsHandler);

								handler_p -> irh_connection_p = connection_p;
								handler_p -> irh_obj_p = NULL;
								handler_p -> irh_stat_p = NULL;
								handler_p -> irh_status = HS_GOOD;
								
								return ((Handler *) handler_p);
						}		/* if (connection_p) */
													
				}		/* if (GetUsernameAndPassword (credentials_p, &username_s, &password_s)) */
				
			FreeMemory (handler_p);
		}		/* if (handler_p) */

	return NULL;
}


void ReleaseHandler (Handler *handler_p)
{
	FreeIRodsHandler (handler_p);
}


void FreeIRodsHandler (Handler *handler_p)
{
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;

	if (irods_handler_p -> irh_obj_p)
		{
			CloseIRodsHandler (handler_p);
		}

	FreeMemory (handler_p);
}


static bool InitIRodsHandler (struct Handler *handler_p)
{
	bool success_flag = true;
	
	return success_flag;
}


static bool OpenIRodsHandler (struct Handler *handler_p, const char *filename_s, const char *mode_s)
{
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;
	openedDataObjInp_t *opened_obj_p = NULL;

	if (irods_handler_p -> irh_obj_p)
		{
			CloseIRodsHandler (handler_p);
		}

	opened_obj_p = (openedDataObjInp_t *) AllocMemory (sizeof (openedDataObjInp_t));

	if (opened_obj_p)
		{
			dataObjInp_t data_obj;
			int handler;
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
			handler = rcDataObjOpen (irods_handler_p -> irh_connection_p, &data_obj);

			if (handler >= 0)
				{
					rodsObjStat_t *stat_p = NULL;
					
					memset (opened_obj_p, 0, sizeof (openedDataObjInp_t));
					opened_obj_p -> l1descInx = handler;

					if (rcObjStat (irods_handler_p -> irh_connection_p, &data_obj, &stat_p) >= 0)
						{
							irods_handler_p -> irh_stat_p = stat_p;
						}
						
					irods_handler_p -> irh_obj_p = opened_obj_p;

				}		/* if (handler >= 0)) */

		}		/* if (opened_obj_p) */


	return (irods_handler_p -> irh_obj_p != NULL);
}



static bool CloseIRodsHandler (struct Handler *handler_p)
{
	bool success_flag = true;
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;

	if (irods_handler_p -> irh_obj_p)
		{
			success_flag = (rcDataObjClose (irods_handler_p -> irh_connection_p, irods_handler_p -> irh_obj_p) == 0);
			irods_handler_p -> irh_obj_p = NULL;
		}

	if (irods_handler_p -> irh_stat_p)
		{
			free (irods_handler_p -> irh_stat_p);
			irods_handler_p -> irh_stat_p = NULL;
		}

	CloseConnection (irods_handler_p -> irh_connection_p);
	irods_handler_p -> irh_connection_p = NULL;
	
	irods_handler_p -> irh_status = HS_NONE;

	return success_flag;
}



static size_t ReadFromIRodsHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;
	bytesBuf_t buffer;
	int i;
	size_t res = 0;
	
	buffer.buf = buffer_p;
	buffer.len = length;
	
	irods_handler_p -> irh_obj_p -> len = length;
	
	i = rcDataObjRead (irods_handler_p -> irh_connection_p, irods_handler_p -> irh_obj_p, &buffer);

	if (i > 0)
		{
			res = (size_t) i;
			
			if (res != length)
				{
					irods_handler_p -> irh_status = HS_FINISHED;
				}
		}
	else
		{
			irods_handler_p -> irh_status = HS_BAD;
		}
		
	return res;
	
}


static size_t WriteToIRodsHandler (struct Handler *handler_p, const void *buffer_p, const size_t length)
{
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;
	bytesBuf_t buffer;
	int i;
	size_t res = 0;
	
	buffer.buf = (void *) buffer_p;
	buffer.len = length;

	irods_handler_p -> irh_obj_p -> len = length;

	#if IRODS_HANDLER_DEBUG >= DL_FINER
		{
			size_t j = 0;
			const char *ptr = (const char *) buffer_p;

			fprintf (stdout, " WRITING --------\n");
			for (j = 0; j < length; ++ j, ++ ptr)
				{
					int k = *ptr;
					fprintf (stdout, "j %lu = \'%x\'\n", j, k);
				}
			fprintf (stdout, "WRITING --------\n\n");
			fflush (stdout);
		}
	#endif
	
	i = rcDataObjWrite (irods_handler_p -> irh_connection_p, irods_handler_p -> irh_obj_p, &buffer);
	
	if (i > 0)
		{
			res = (size_t) i;
		}
	else
		{
			irods_handler_p -> irh_status = HS_BAD;
		}

		
	return res;
}


static bool SeekIRodsHandler (struct Handler *handler_p, long offset, int whence)
{
	bool success_flag = true;
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;
	fileLseekOut_t *seek_p = NULL;

	irods_handler_p -> irh_obj_p -> offset = offset;
	irods_handler_p -> irh_obj_p -> whence = whence;

	success_flag = (rcDataObjLseek (irods_handler_p -> irh_connection_p, irods_handler_p -> irh_obj_p, &seek_p) == 0);

	return success_flag;
}


static HandlerStatus GetIRodsHandlerStatus (struct Handler *handler_p)
{
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;

	return (irods_handler_p -> irh_status);	
}



static const char *GetIRodsHandlerProtocol (struct Handler *handler_p)
{
	return "irods";
}


static const char *GetIRodsHandlerName (struct Handler *handler_p)
{
	return "iRODS Handler";
}


static const char *GetIRodsHandlerDescription (struct Handler *handler_p)
{
	return "A Handler for files using iRODS";
}


static bool IsResourceForIRodsHandler (struct Handler *handler_p, const Resource * resource_p)
{
	bool match_flag = false;
	
	if (resource_p -> re_protocol_s)
		{
			match_flag = (strcmp (GetIRodsHandlerProtocol (handler_p), resource_p -> re_protocol_s) == 0);
		}
		
	return match_flag;
}



static bool CalculateFileInformationFromIRodsHandler (struct Handler *handler_p, FileInformation *info_p)
{
	IRodsHandler *irods_handler_p = (IRodsHandler *) handler_p;
	bool success_flag = false;

	if (irods_handler_p -> irh_stat_p)
		{
			time_t t;
			const char *time_s = irods_handler_p -> irh_stat_p -> modifyTime;
			
			info_p -> fi_size = irods_handler_p -> irh_stat_p -> objSize;

			if (GetValidLong (&time_s, &t))
				{
					info_p -> fi_last_modified = t;
				}
			else
				{
					info_p -> fi_last_modified = 0;
				}

			success_flag = true;
		}

	return success_flag;
}
