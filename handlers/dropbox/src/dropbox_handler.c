#include <fcntl.h>

#define ALLOCATE_DROPBOX_TAGS
#include "dropbox_handler.h"

#include "json_util.h"



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


static drbClient *CreateClient (char *token_key_s, char *token_secret_s);


static drbClient *CreateClient (char *token_key_s, char *token_secret_s)
{
	drbClient *client_p = NULL;
	
	/* Global initialisation */
	drbInit ();
    
	client_p = drbCreateClient ((char *) DROPBOX_APP_KEY_S, (char *) DROPBOX_APP_SECRET_S, token_key_s, token_secret_s);
	
	if (client_p)
		{
			// Request a AccessToken if undefined (NULL)
			if (! (token_key_s && token_secret_s))
				{
					bool success_flag = false;
					drbOAuthToken *req_token_p = drbObtainRequestToken (client_p);
					
					if (req_token_p) 
						{
							drbOAuthToken *acc_token_p = NULL;
							char *url_s = drbBuildAuthorizeUrl (req_token_p);
							printf("Please visit %s\nThen press Enter...\n", url_s);
							free (url_s);
							fgetc (stdin);
							
							acc_token_p = drbObtainAccessToken (client_p);
							
							if (acc_token_p) 
								{
									// This key and secret can replace the NULL value in t_key and
									// t_secret for the next time.
									printf("key:    %s\nsecret: %s\n", acc_token_p -> key, acc_token_p -> secret);
									success_flag = true;
								} 
							else
								{
									fprintf(stderr, "Failed to obtain an AccessToken...\n");
								}
						} 
					else 
						{
							fprintf(stderr, "Failed to obtain a RequestToken...\n");
						}

					if (!success_flag)
						{
							drbDestroyClient (client_p);
							client_p = NULL;
						}		

				}		/* if (! (token_key_s && token_secret_s) */

    
			/* Set default arguments to not repeat them on each API call */
			if (client_p)
				{
					drbSetDefault (client_p, DRBOPT_ROOT, DRBVAL_ROOT_AUTO, DRBOPT_END);
				}
				
		}		/* if (client_p) */
	
	return client_p;
}



Handler *GetHandler (const json_t *credentials_p)
{
	DropboxHandler *handler_p = (DropboxHandler *) AllocMemory (sizeof (DropboxHandler));

	if (handler_p)
		{
			const char *tags_s = json_dumps (credentials_p, 0);
			const char *token_key_s = GetJSONString (credentials_p, DROPBOX_TOKEN_KEY_S);
			const char *token_secret_s = GetJSONString (credentials_p, DROPBOX_TOKEN_SECRET_S);
		
			handler_p -> dh_client_p = CreateClient ((char *) token_key_s, (char *) token_secret_s);
			
			if (handler_p -> dh_client_p)
				{
					InitialiseHandler (& (handler_p -> dh_base_handler),
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

						handler_p -> dh_status = HS_GOOD;
						handler_p -> dh_local_copy_f = NULL;
						handler_p -> dh_dropbox_filename_s = NULL;
						handler_p -> dh_updated_flag = false;
						
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

	if (dropbox_handler_p -> dh_client_p)
		{
			CloseDropboxHandler (handler_p);
		}

	FreeMemory (handler_p);
}


static void FlushCachedFile (void)
{
	if (dropbox_handler_p -> dh_local_copy_f)
		{
			if (fclose (dropbox_handler_p -> dh_local_copy_f) != EOF)
				{
					if (handler_p -> dh_dropbox_filename_s)
						{
							
						}
						
				}		/* if (fclose (dropbox_handler_p -> dh_local_copy_f) != EOF) */
								
		}		/* if (dropbox_handler_p -> dh_local_copy_f) */
}


static bool OpenDropboxHandler (struct Handler *handler_p, const char *filename_s, const char *mode_s)
{
	bool success_flag = false;
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	if (mode_s)
		{
			/* 
			 * If we need to access the file, cache a local copy
			 */
			const char c = *mode_s;
			char *buffer_s = NULL;
			
			FlushCachedFile ();

			buffer_s = (char *) AllocMemory (L_tmpnam);

			if (buffer_s)
				{
					FILE *temp_f = NULL;
`
					tmpnam (buffer_s);
					
					temp_f = fopen (buffer_s, "wb+");
					
					if (temp_f)
						{
							void *output_p = NULL;
							
							int res = drbGetFile (dropbox_handler_p -> dh_client_p, 
								&output_p,
								DRBOPT_PATH, buffer_s,
								DRBOPT_IO_DATA, temp_f,
								DRBOPT_IO_FUNC, fwrite,
								DRBOPT_END);
							
							if (res == DBERR_OK)
								{
									dropbox_handler_p -> dh_dropbox_filename_s = buffer_s;
									dropbox_handler_p -> dh_local_copy_f = temp_f;
									success_flag = true;
								}		/* if (res != DBERR_OK) */
																
						}		/* if (temp_f) */							
					
				}		/* if (buffer_s) */

		}

	return success_flag;
}



static bool CloseDropboxHandler (struct Handler *handler_p)
{
	bool success_flag = true;
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	/* If there are any changes, upload them */
	if (dropbox_handler_p -> dh_updated_flag)
		{
			memStream stream; 
			memStreamInit (&stream);
			stream.data = "hello world!";
			stream.size = strlen(stream.data);
			
			err = drbPutFile (dropbox_handler_p -> dh_client_p, 
				NULL,
				DRBOPT_PATH, "/hello.txt",
				DRBOPT_IO_DATA, &stream,
				DRBOPT_IO_FUNC, memStreamRead,
				DRBOPT_END);

			printf("File upload: %s\n", err ? "Failed" : "Successful");
		}


	/* Free all client allocated memory */
	if (dropbox_handler_p -> dh_client_p)
		{
			drbDestroyClient (dropbox_handler_p -> dh_client_p);
			dropbox_handler_p -> dh_client_p = NULL;
		}
		
	/* Global Cleanup */
	drbCleanup ();

	return success_flag;
}



static size_t ReadFromDropboxHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;
	size_t res = 0;
			
	if (dropbox_handler_p -> dh_local_copy_f)
		{
			res = fread (buffer_p, length, 1, dropbox_handler_p -> dh_local_copy_f);
		}
					
	return res;
}


static size_t WriteToDropboxHandler (struct Handler *handler_p, const void *buffer_p, const size_t length)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;
	size_t res = 0;

	if (dropbox_handler_p -> dh_local_copy_f)
		{
			res = fwrite (buffer_p, length, 1, dropbox_handler_p -> dh_local_copy_f);
			
			if (res == length)
				{
					if (! (dropbox_handler_p -> dh_updated_flag))
						{
							dropbox_handler_p -> dh_updated_flag = true;
						}
				}
			else
				{
					dropbox_handler_p -> dh_status = HS_BAD;
				}
		}

	return res;
}


static bool SeekDropboxHandler (struct Handler *handler_p, long offset, int whence)
{
	bool success_flag = false;
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	if (dropbox_handler_p -> dh_local_copy_f)
		{
			success_flag = (fseek (dropbox_handler_p -> dh_local_copy_f, offset, whence) == 0);
		}


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
