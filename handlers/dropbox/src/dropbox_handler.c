#include <fcntl.h>
#include <string.h>

#define ALLOCATE_DROPBOX_TAGS
#include "dropbox_handler.h"
#include "handler_utils.h"

#include "json_util.h"
#include "time_util.h"
#include "filesystem_utils.h"


static bool InitDropboxHandler (struct Handler *handler_p, json_t *credentials_p);

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

static bool FlushCachedFile (DropboxHandler *dropbox_handler_p);

static drbClient *CreateClient (char *token_key_s, char *token_secret_s);

static bool GetLastModifiedTime (struct DropboxHandler *handler_p, const char * const filename_s, time_t *time_p);



/**********************************/


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
			const char *token_key_s = NULL;
			const char *token_secret_s = NULL;
			
			json_t *dropbox_p = json_object_get (credentials_p, GetDropboxHandlerName (NULL));
			
			if (dropbox_p)
				{
					token_key_s = GetJSONString (dropbox_p, DROPBOX_TOKEN_KEY_S);
					token_secret_s = GetJSONString (dropbox_p, DROPBOX_TOKEN_SECRET_S);
				}			
		
			handler_p -> dh_client_p = CreateClient ((char *) token_key_s, (char *) token_secret_s);
			
			if (handler_p -> dh_client_p)
				{
					InitialiseHandler (& (handler_p -> dh_base_handler),
						InitDropboxHandler,
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
			
			if (dropbox_handler_p -> dh_local_copy_f)
				{
					fclose (dropbox_handler_p -> dh_local_copy_f);
					dropbox_handler_p -> dh_local_copy_f = NULL;
				}
				
			if (handler_p -> ha_filename_s)
				{
					RemoveFile (handler_p -> ha_filename_s);
				}
				
			if (dropbox_handler_p -> dh_dropbox_filename_s)
				{
					FreeMemory (dropbox_handler_p -> dh_dropbox_filename_s);
					dropbox_handler_p -> dh_dropbox_filename_s = NULL;
				}

			drbDestroyClient (dropbox_handler_p -> dh_client_p);
			dropbox_handler_p -> dh_client_p = NULL;
		}
		
	/* Global Cleanup */
	drbCleanup ();


	FreeMemory (handler_p);
}


static bool InitDropboxHandler (struct Handler *handler_p, json_t *credentials_p)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;	
	bool success_flag = (dropbox_handler_p -> dh_client_p != NULL);
	
	if (!success_flag)
		{
			dropbox_handler_p -> dh_client_p = NULL; //CreateClient ((char *) token_key_s, (char *) token_secret_s);
			
			if (dropbox_handler_p -> dh_client_p)
				{
				}
		}
			
	return success_flag;
}


static bool FlushCachedFile (DropboxHandler *dropbox_handler_p)
{
	bool success_flag = true;
	
	/* If there are any changes, upload them */
	if (dropbox_handler_p -> dh_updated_flag)
		{
			int res;
			
			rewind (dropbox_handler_p -> dh_local_copy_f);
			
			res = drbPutFile (dropbox_handler_p -> dh_client_p, 
				NULL,
				DRBOPT_PATH, dropbox_handler_p -> dh_dropbox_filename_s,
				DRBOPT_IO_DATA, dropbox_handler_p -> dh_local_copy_f,
				DRBOPT_IO_FUNC, fread,
				DRBOPT_END);

			success_flag = (res == DRBERR_OK);
		}

		
	return success_flag;
}


static bool OpenDropboxHandler (struct Handler *handler_p, const char *filename_s, const char *mode_s)
{
	bool success_flag = false;
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	if (mode_s)
		{
			const char *cached_filename_s = NULL;
			char *user_id_s = dropbox_handler_p -> dh_client_p -> t.secret;
			time_t last_modified = 0;
			const char * const protocol_s = GetDropboxHandlerProtocol (handler_p);
			
			if (*mode_s != 'w')
				{
					/* Get the timestamp of the file (if it exists) on the server */
					if (!GetLastModifiedTime (dropbox_handler_p, filename_s, &last_modified))
						{
							printf ("Couldn't get last modified time for %s\n", filename_s);
						}
				}

			cached_filename_s = GetMappedFilename (protocol_s, user_id_s, filename_s, &last_modified);
			
			if (cached_filename_s)
				{
					success_flag = true;
				}
			else			
				{
					/* 
					 * If we need to access the file, cache a local copy
					 */
					char *buffer_s = NULL;
					
					FlushCachedFile (dropbox_handler_p);

					buffer_s = (char *) AllocMemory (L_tmpnam);

					if (buffer_s)
						{
							FILE *temp_f = NULL;

							tmpnam (buffer_s);
							
							temp_f = fopen (buffer_s, "wb+");
							
							if (temp_f)
								{
									void *output_p = NULL;
									int res;
									
									/* 
									 * If we're not creating a file from scratch, then download 
									 * any existing file from the server
									 */
									if (*mode_s != 'w')
										{
											res = drbGetFile (dropbox_handler_p -> dh_client_p, 
												&output_p,
												DRBOPT_PATH, filename_s,
												DRBOPT_IO_DATA, temp_f,
												DRBOPT_IO_FUNC, fwrite,
												DRBOPT_END);
											
													
											if (res == DRBERR_OK)
												{
													dropbox_handler_p -> dh_dropbox_filename_s = buffer_s;

													/* If we're not appending, then rewind to the start */
													if (*mode_s != 'a')
														{
															rewind (dropbox_handler_p -> dh_local_copy_f);
														}
														
													success_flag = true;
												}		/* if (res != DBERR_OK) */
											else if ((res == 404) && (*mode_s == 'a'))
												{
													/* file doesn't exist, but it's ok as we're writing to it */
													success_flag = true;
												}
											else
												{
													
												}
											
											
										}		/* if (*mode_s != 'w') */
									else
										{
											success_flag = true;
										}
										
									if (success_flag)
										{
											if (!SetMappedFilename (protocol_s, user_id_s, filename_s, buffer_s, last_modified))
												{
													printf ("failed to set filename for handler cache\n");
													success_flag = false;
												}

										}		/* if (success_flag) */
										
								}		/* if (temp_f) */							
							
						}		/* if (buffer_s) */

					
				}		/* if (!cached_filename_s) */

		}

	return success_flag;
}



static bool CloseDropboxHandler (struct Handler *handler_p)
{
	bool success_flag = true;
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;

	FlushCachedFile (dropbox_handler_p);

	return success_flag;
}



static size_t ReadFromDropboxHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;
	size_t res = 0;
			
	if (dropbox_handler_p -> dh_local_copy_f)
		{
			res = fread (buffer_p, 1, length, dropbox_handler_p -> dh_local_copy_f);
		}
					
	return res;
}


static size_t WriteToDropboxHandler (struct Handler *handler_p, const void *buffer_p, const size_t length)
{
	DropboxHandler *dropbox_handler_p = (DropboxHandler *) handler_p;
	size_t res = 0;

	if (dropbox_handler_p -> dh_local_copy_f)
		{
			res = fwrite (buffer_p, 1, length, dropbox_handler_p -> dh_local_copy_f);
			
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

	return (dropbox_handler_p -> dh_status);	
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
	bool success_flag = false;

	if (handler_p -> ha_filename_s)
		{
			success_flag = CalculateFileInformation (handler_p -> ha_filename_s, info_p);
		}

	return success_flag;
}


static bool GetLastModifiedTime (struct DropboxHandler *handler_p, const char * const filename_s, time_t *time_p)
{
	bool success_flag = false;
	void *output_p = NULL;
	
	/* Get the last modified time of the file on the server */
	int res = drbGetMetadata (handler_p -> dh_client_p, &output_p, DRBOPT_PATH, filename_s, DRBOPT_END);

	if (res == DRBERR_OK) 
		{
			drbMetadata *meta_p = (drbMetadata*) output_p;
			
			/*
			 * Tue, 17 Jun 2014 14:26:52 +0000
			 * "%a, %d %b %Y %H:%M:%S %z"
			 */
			if (meta_p -> modified)
				{
					success_flag = ConvertDropboxStringToEpochTime (meta_p -> modified, time_p);
				}
			
			drbDestroyMetadata (meta_p, true);
		} 
	else 
		{
			printf ("Metadata error (%d): %s\n", res, (char *) output_p);
			free (output_p);
		}
	
	return success_flag;
}
