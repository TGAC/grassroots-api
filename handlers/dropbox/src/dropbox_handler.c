#include <fcntl.h>

#define ALLOCATE_DROPBOX_TAGS
#include "dropbox_handler.h"


static bool OpenDropboxHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromDropboxHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

static size_t WriteToDropboxHandler (struct Handler *handler_p, const void *buffer_p, const size_t length);

static bool SeekDropboxHandler (struct Handler *handler_p, long offset, int whence);

static bool CloseDropboxHandler (struct Handler *handler_p);

static HandlerStatus GetDropboxHandlerStatus (struct Handler *handler_p);

static bool IsResourceForDropboxHandler (struct Handler *handler_p, const Resource * resource_p);

static const char *GetDropboxHandlerProtocol (struct Handler *handler_p);

static const char *GetDropboxHandlerName (struct Handler *handler_p);

static const char *GetDropboxHandlerDescription (struct Handler *handler_p);

static void FreeDropboxHandler (struct Handler *handler_p);

static bool IsResourceForDropboxHandler (struct Handler *handler_p, const Resource * resource_p);

static bool CalculateFileInformationFromDropboxHandler (struct Handler *handler_p, FileInformation *info_p);



Handler *GetHandler (const json_t *credentials_p)
{
	DropboxHandler *handler_p = (DropboxHandler *) AllocMemory (sizeof (DropboxHandler));

	if (handler_p)
		{
			const char *tags_s = json_dumps (credentials_p, 0);
			const char *username_s = NULL;
			const char *password_s = NULL; 
			
			if (GetUsernameAndPassword (credentials_p, &username_s, &password_s))
				{
					InitialiseHandler (& (handler_p -> irh_base_handler),
						IsResourceForDropboxHandler,
						GetDropboxHandlerProtocol,
						GetDropboxHandlerName,
						GetDropboxHandlerDescription,
						OpenDropboxHandler,
						ReadFromDropboxHandler,
						WriteToDropboxHandler,
						SeekDropboxHandler,
						CloseDropboxHandler,
						GetDropboxHandlerStatus,
						CalculateFileInformationFromDropboxHandler,
						FreeDropboxHandler);

						handler_p -> irh_status = HS_GOOD;
						
						return ((Handler *) handler_p);
													
				}		/* if (GetUsernameAndPassword (credentials_p, &username_s, &password_s)) */
				
			FreeMemory (handler_p);
		}		/* if (handler_p) */

	return NULL;
}


void ReleaseHandler (Handler *handler_p)
{
	FreeDropboxHandler (handler_p);
}


void FreeDropboxHandler (Handler *handler_p)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	if (dropbox_handler_p -> irh_obj_p)
		{
			CloseDropboxHandler (handler_p);
		}

	FreeMemory (handler_p);
}


static bool OpenDropboxHandler (struct Handler *handler_p, const char *filename_s, const char *mode_s)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	return false;
}



static bool CloseDropboxHandler (struct Handler *handler_p)
{
	bool success_flag = true;
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;


	return success_flag;
}



static size_t ReadFromDropboxHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;
	size_t res = 0;
			
	return res;
	
}


static size_t WriteToDropboxHandler (struct Handler *handler_p, const void *buffer_p, const size_t length)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	return res;
}


static bool SeekDropboxHandler (struct Handler *handler_p, long offset, int whence)
{
	bool success_flag = true;
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;


	return success_flag;
}


static HandlerStatus GetDropboxHandlerStatus (struct Handler *handler_p)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	return (dropbox_handler_p -> irh_status);	
}



static const char *GetDropboxHandlerProtocol (struct Handler *handler_p)
{
	return "dropbox";
}


static const char *GetDropboxHandlerName (struct Handler *handler_p)
{
	return "Dropbox Handler";
}


static const char *GetDropboxHandlerDescription (struct Handler *handler_p)
{
	return "A Handler for files hosted on Dropbox";
}


static bool IsResourceForDropboxHandler (struct Handler *handler_p, const Resource * resource_p)
{
	bool match_flag = false;
	
	if (resource_p -> re_protocol_s)
		{
			match_flag = (strcmp (GetDropboxHandlerProtocol (handler_p), resource_p -> re_protocol_s) == 0);
		}
		
	return match_flag;
}



static bool CalculateFileInformationFromDropboxHandler (struct Handler *handler_p, FileInformation *info_p)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;
	bool success_flag = false;


	return success_flag;
}
