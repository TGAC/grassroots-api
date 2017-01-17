/*
** Copyright 2014-2016 The Earlham Institute
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
/*!
 * \file    example.c
 * \brief   Usage example of dropbox C library.
 * \author  Adrien Python
 * \version 1.0
 * \date    06.08.2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dropbox.h>
#include <memStream.h>

void displayAccountInfo(drbAccountInfo *info);
void displayMetadata(drbMetadata *meta, char *title);
void displayMetadataList(drbMetadataList* list, char* title);


static drbClient *CreateClient (char *app_key_s, char *app_secret_s, char *token_key_s, char *token_secret_s)
{
	drbClient *client_p = NULL;
	
	/* Global initialisation */
	drbInit ();
    
	client_p = drbCreateClient (app_key_s, app_secret_s, token_key_s, token_secret_s);
	
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




int main (int argc, char **argv) {
    
    int err;
    void* output;

    // Application key and secret. To get them, create an app here:
    // https://www.dropbox.com/developers/apps
    char *c_key    = NULL;  //< consumer key
    char *c_secret = NULL;  //< consumer secret
    
    
    // User key and secret. Leave them NULL or set them with your AccessToken.
    char *t_key    = NULL; //< access token key
    char *t_secret = NULL;  //< access token secret
    
        
    if (argc < 3) 
			{
				printf ("dropbox <key> <secret>\n");
				return 0;
			}
    
    c_key    = argv [1];  //< consumer key
    c_secret = argv [2];  //< consumer secret
    
    if (argc == 5)
			{
				t_key    = argv [3]; //< access token key
				t_secret = argv [4];  //< access token secret				
			}

    
    
    // Global initialisation
    drbInit();
    
    // Create a Dropbox client
    drbClient* cli = CreateClient (c_key, c_secret, t_key, t_secret);
    
    // Read account Informations
    output = NULL;
    err = drbGetAccountInfo(cli, &output, DRBOPT_END);
    if (err != DRBERR_OK) {
        printf("Account info error (%d): %s\n", err, (char*)output);
        free(output);
    } else {
        drbAccountInfo* info = (drbAccountInfo*)output;
        displayAccountInfo(info);
        drbDestroyAccountInfo(info);
    }
    
    // List root directory files
    output = NULL;
    err = drbGetMetadata(cli, &output,
                         DRBOPT_PATH, "/",
                         DRBOPT_LIST, true,
                         //                     DRBOPT_FILE_LIMIT, 100,
                         DRBOPT_END);
    if (err != DRBERR_OK) {
        printf("Metadata error (%d): %s\n", err, (char*)output);
        free(output);
    } else {
        drbMetadata* meta = (drbMetadata*)output;
        displayMetadata(meta, "Metadata");
        drbDestroyMetadata(meta, true);
    }

    
    // Free all client allocated memory
    drbDestroyClient(cli);

    // Global Cleanup
    drbCleanup();

		cli = CreateClient(c_key, c_secret, t_key, t_secret);
    
    // Try to download a file named hello.txt
    FILE *file = fopen("/tmp/hello.txt", "w"); // Write it in this file
    output = NULL;
    err = drbGetFile(cli, &output,
                     DRBOPT_PATH, "/hello.txt",
                     DRBOPT_IO_DATA, file,
                     DRBOPT_IO_FUNC, fwrite,
                     DRBOPT_END);
    fclose(file);
    
    if (err != DRBERR_OK) {
        printf("Get File error (%d): %s\n", err, (char*)output);
        free(output);
    } else {
        displayMetadata(output, "Get File Result");
        drbDestroyMetadata(output, true);
    }
    
    
    // Upload this file if it doesnt exists
    if (err == 404) {
        memStream stream; memStreamInit(&stream);
        stream.data = "hello world!";
        stream.size = strlen(stream.data);
        
        err = drbPutFile(cli, NULL,
                         DRBOPT_PATH, "/hello.txt",
                         DRBOPT_IO_DATA, &stream,
                         DRBOPT_IO_FUNC, memStreamRead,
                         DRBOPT_END);
        printf("File upload: %s\n", err ? "Failed" : "Successful");
    }
    
    // Free all client allocated memory
    drbDestroyClient(cli);
    
    // Global Cleanup
    drbCleanup();
    
    return EXIT_SUCCESS;
}


char* strFromBool(bool b) { return b ? "true" : "false"; }

/*!
 * \brief  Display a drbAccountInfo item in stdout.
 * \param  info    account informations to display.
 * \return  void
 */
void displayAccountInfo(drbAccountInfo* info) {
    if(info) {
        printf("---------[ Account info ]---------\n");
        if(info->referralLink)         printf("referralLink: %s\n", info->referralLink);
        if(info->displayName)          printf("displayName:  %s\n", info->displayName);
        if(info->uid)                  printf("uid:          %d\n", *info->uid);
        if(info->country)              printf("country:      %s\n", info->country);
        if(info->email)                printf("email:        %s\n", info->email);
        if(info->quotaInfo.datastores) printf("datastores:   %u\n", *info->quotaInfo.datastores);
        if(info->quotaInfo.shared)     printf("shared:       %u\n", *info->quotaInfo.shared);
        if(info->quotaInfo.quota)      printf("quota:        %u\n", *info->quotaInfo.quota);
        if(info->quotaInfo.normal)     printf("normal:       %u\n", *info->quotaInfo.normal);
    }
}

/*!
 * \brief  Display a drbMetadata item in stdout.
 * \param  meta    metadata to display.
 * \param   title   display the title before the metadata.
 * \return  void
 */
void displayMetadata(drbMetadata* meta, char* title) {
    if (meta) {
        if(title) printf("---------[ %s ]---------\n", title);
        if(meta->hash)        printf("hash:        %s\n", meta->hash);
        if(meta->rev)         printf("rev:         %s\n", meta->rev);
        if(meta->thumbExists) printf("thumbExists: %s\n", strFromBool(*meta->thumbExists));
        if(meta->bytes)       printf("bytes:       %d\n", *meta->bytes);
        if(meta->modified)    printf("modified:    %s\n", meta->modified);
        if(meta->path)        printf("path:        %s\n", meta->path);
        if(meta->isDir)       printf("isDir:       %s\n", strFromBool(*meta->isDir));
        if(meta->icon)        printf("icon:        %s\n", meta->icon);
        if(meta->root)        printf("root:        %s\n", meta->root);
        if(meta->size)        printf("size:        %s\n", meta->size);
        if(meta->clientMtime) printf("clientMtime: %s\n", meta->clientMtime);
        if(meta->isDeleted)   printf("isDeleted:   %s\n", strFromBool(*meta->isDeleted));
        if(meta->mimeType)    printf("mimeType:    %s\n", meta->mimeType);
        if(meta->revision)    printf("revision:    %d\n", *meta->revision);
        if(meta->contents)    displayMetadataList(meta->contents, "Contents");
    }
}

/*!
 * \brief  Display a drbMetadataList item in stdout.
 * \param  list    list to display.
 * \param   title   display the title before the list.
 * \return  void
 */
void displayMetadataList(drbMetadataList* list, char* title) {
    if (list){
				int i;
        printf("---------[ %s ]---------\n", title);
        for (i = 0; i < list->size; i++) {
            
            displayMetadata(list->array[i], list->array[i]->path);
        }
    }
}
