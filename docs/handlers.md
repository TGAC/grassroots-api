# Handlers

## Introduction

Data that a user wishes to access can be potentially be stored on a variety of different systems *e.g.* local filesystems, http(s) locations, cloud-based storage systems such as Dropbox, Google Drive, *etc.* 
Regardless of the storage mechanism, the actual operations that may be needed are the same *e.g.* read from a file, write to a file, create a file, *etc.* 
So the Grassroots system abstracts out these actions to an API, the **Handlers** API and has separate components for each of the storage mechanisms.  
This allows developers to code to the *Handlers* API for doing file access and not need to worry where the underlying files are actually stored.

The currently available Handlers are:

 * Any mounted filesystem
 * iRODS
 * Dropbox (partial support)
 * HTTP(S) locations 

## Usage

Handlers work upon data files represented by the *Resource* datatype. For accesing via a Handler, the fields of interest in this datatype are *re_protocol_s* and *re_value_s*.

~~~{.c}
typedef struct Resource
{
	/**
	 * The protocol of this Resource.
	 */
	char *re_protocol_s;

	/**
	 * The protocol-specific path to the data object.
	 */
	char *re_value_s;
	
	...
} Resource;
~~~ 

*re_protocol_s* is the value that declares the underlying storage mechanism *e.g.* http, file, *etc.* and *re_value_s* is the path to the file/directory of interest. 
For example, the uri http://earlham.ac.uk/data/wheat_assembly.fasta would have 

~~~{.c}
re_protcol_s = "http"
re_value_s = "earlham.ac.uk/data/wheat_assembly.fasta"
~~~

and the file /opt/data/research.pdf would have

~~~{.c}
re_protcol_s = "file"
re_value_s = "/opt/data/research.pdf"
~~~

Within the `handler_utils.h` file, there is a function `GetResourceHandler` that determines the correct Handler for as given Resource

~~~{.c}
Handler *GetResourceHandler (const Resource *resource_p, const UserDetails *user_p);
~~~ 

Once you have this Handler you can then perform common I/O tasks with routines analagous to those provided by the standard C library such as ```fread```, ```fwrite```, ```fseek```, *etc.*
Further details are provided by the Handler API documentation.

## Examples

We will now give some examples that show some standard C file I/O code and the equivalent using Handlers.
The first is to open a file, seek 20 bytes in and read 16 characters into a buffer and return it. The first code section uses standard C library calls, and the code below use the Grassroots API equivalents.

~~~{.c}
char *ReadFileChunk (const char * const filename_s)
{
	FILE *in_f = fopen (filename_s, "rb");

	if (in_f)
		{
			/* Scroll 20 bytes into the file */
			if (fseek (in_f, 20, SEEK_SET) == 0)
				{
					/*
					 * We are going to try and read in 16 bytes so allocate
					 * the memory needed including the extra byte for the
					 * terminating \0
					 */
					const size_t buffer_size = 16;
					char *buffer_s = (char *) malloc ((buffer_size + 1) * sizeof (char));

					if (buffer_s)
						{
							/* Read in the data */
							if (fread (buffer_s, 1, buffer_size, in_f) == buffer_size)
								{
									/* Add the terminating \0 */
									* (buffer_s + buffer_size) = '\0';

									return buffer_s;
								}		/* if (fread (buffer_s, 1, buffer_size, in_f) == buffer_size) */
							else
								{
									fprintf (stderr, "Failed to read value from %s", filename_s);
								}

							free (buffer_s);
						}		/* if (buffer_s) */
					else
						{
							fprintf (stderr, "Failed to allocate memory for buffer when reading %s", filename_s);
						}

				}		/* if (fseek (in_f, 20, SEEK_SET) == 0) */
			else
				{
					fprintf (stderr, "Failed to seek in file %s", filename_s);
				}

			if (fclose (in_f) != 0)
				{
					fprintf (stderr, "Failed to close  file %s", filename_s);
				}		/* if (fclose (in_f) != 0) */

		}		/* if (in_f) */
	else
		{
			fprintf (stderr, "Failed to open %s", filename_s);
		}

	return NULL;
}
~~~

Below is the equivalent code using the Handlers API

~~~{.c}
char *ReadFileChunk (const char * const filename_s)
{
	/* The value that we will return */
	char *result_s = NULL;

	/*
	 * Allocate the Resource.
	 * We know that it's a file so we use the protocol for a file,
	 * PROTOCOL_FILE_S, and we do not need a title for the Resource
	 * so we send it NULL.
	 */
	Resource res_p = AllocateResource (PROTOCOL_FILE_S, filename_s, NULL);

	if (res_p)
		{
			/*
			 * Now that we have the Resource, let the Grassroots system find
			 * the appropriate Handler.
			 */
			Handler *handler_p = GetResourceHandler (res_p, NULL);

			if (handler_p)
				{
					/* Scroll 20 bytes into the file */
					if (SeekHandler (handler_p, 20, SEEK_SET))
						{
							/*
							 * We are going to try and read in 16 bytes so allocate
							 * the memory needed including the extra byte for the
							 * terminating \0
							 */
							const size_t buffer_size = 16;
							char *buffer_s = (char *) AllocMemory ((buffer_size + 1) * sizeof (char));

							if (buffer_s)
								{
									/* Read in the data */
									if (ReadFromHandler (handler_p, buffer_s, buffer_size) == buffer_size)
										{
											/* Add the terminating \0 */
											* (buffer_s + buffer_size) = '\0';

											/* Store the value for returning */
											result_s = buffer_s;
										}		/* if (fread (buffer_s, 1, buffer_size, in_f) == buffer_size) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to read value from %s", filename_s);

											/* We failed to read in the value so we can release the buffer memory */
											FreeMemory (buffer_s);
										}

								}		/* if (buffer_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for buffer when reading %s", filename_s);
								}

						}		/* if (SeekHandler (handler_p, 20, SEEK_SET)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to seek in %s", filename_s);
						}

					CloseHandler (handler_p);
				}		/* if (handler_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to find a Handler for %s", filename_s);
				}

			FreeResource (res_p);
		}		/* if (res_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate Resource for %s", filename_s);
		}

	return result_s;
}
~~~

~~~{.c}
size_t 	ReadFromHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
 
size_t 	WriteToHandler (struct Handler *handler_p, const void *buffer_p, const size_t length)
 
bool 	SeekHandler (struct Handler *handler_p, size_t offset, int whence)
  
HandlerStatus 	GetHandlerStatus (struct Handler *handler_p)
 
bool 	CalculateFileInformationFromHandler (struct Handler *handler_p, FileInformation *info_p)
~~~