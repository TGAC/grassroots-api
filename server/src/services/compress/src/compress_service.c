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

#include <arpa/inet.h>

#include "compress_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service_config.h"

#include "zip.h"
#include "gzip.h"


/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData bsd_base_data;
} CompressServiceData;


typedef enum
{
	CA_Z,
	CA_ZIP,
	CA_GZIP,
	CA_NUM_ALGORITHMS
} CompressionAlgorithm;


static const char *s_algorithm_names_pp [CA_NUM_ALGORITHMS] =
{
	"z",
	"zip",
	"gz"
};


typedef int (*CompressFunction) (Handler *in_p, Handler *out_p, int level);

static CompressFunction s_compress_fns [CA_NUM_ALGORITHMS] = 
{
	CompressAsZipNoHeader,
	CompressAsZip,
	CompressAsGZip
};


static NamedParameterType CS_INPUT = { "input_file", PT_FILE_TO_READ };
static NamedParameterType CS_ALGORITHM = { "compression", PT_STRING };



/*
 * STATIC PROTOTYPES
 */

static const char *GetCompressServiceName (Service *service_p);

static const char *GetCompressServiceDesciption (Service *service_p);


static ParameterSet *GetCompressServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseCompressServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunCompressService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p, ProvidersStateTable *providers_p);

static ParameterSet *IsFileForCompressService (Service *service_p, Resource *resource_p, Handler *handler_p);


static int Compress (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p);

static bool CloseCompressService (Service *service_p);


/*
 * API FUNCTIONS
 */

ServicesArray *GetServices (const json_t * UNUSED_PARAM (config_p))
{
	Service *compress_service_p = (Service *) AllocMemory (sizeof (Service));

	if (compress_service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					ServiceData *data_p = NULL;
					
					InitialiseService (compress_service_p,
						GetCompressServiceName,
						GetCompressServiceDesciption,
						NULL,
						RunCompressService,
						IsFileForCompressService,
						GetCompressServiceParameters,
						ReleaseCompressServiceParameters,
						CloseCompressService,
						NULL,
						true,
						true,
						data_p);
					
					* (services_p -> sa_services_pp) = compress_service_p;
							
					return services_p;
				}
				
			FreeService (compress_service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


static bool CloseCompressService (Service * UNUSED_PARAM (service_p))
{
	bool success_flag = true;
	
	return success_flag;
}



bool CompressData (z_stream *strm_p, Bytef **output_buffer_pp, size_t *output_buffer_size_p, const int flush)
{
	int res;
	bool success_flag = true;
	Bytef *output_buffer_p = *output_buffer_pp;
	size_t output_buffer_size = *output_buffer_size_p;
		
	res = deflate (strm_p, flush);

	while ((res == Z_BUF_ERROR) && success_flag)
		{
			FreeMemory (output_buffer_p);
			
			output_buffer_size <<= 1;
			output_buffer_p = (Bytef *) AllocMemoryArray (output_buffer_size, sizeof (Bytef));
			
			if (output_buffer_p)
				{
					*output_buffer_pp = output_buffer_p;
					*output_buffer_size_p = output_buffer_size;
					
					strm_p -> next_out = output_buffer_p;
					strm_p -> avail_out = output_buffer_size;
					
					res = deflate (strm_p, flush);					
				}
			else
				{
					success_flag = false;
				}
		}
		
	return (success_flag && (res != Z_STREAM_ERROR));
}





int CompressAsGZip (Handler *in_p, Handler *out_p, int level)
{
	bool success_flag = false;
	FileInformation file_info;

	if (CalculateFileInformationFromHandler (in_p, &file_info))
		{
			z_stream strm;
			gz_header header;

			if (InitGzipCompressor (&strm, level, &header, in_p -> ha_resource_p -> re_value_s, file_info.fi_last_modified))
				{
					#define BUFFER_SIZE (4096)
					size_t output_buffer_size = BUFFER_SIZE;
					Bytef *output_buffer_p = (Bytef *) AllocMemoryArray (output_buffer_size, sizeof (Bytef));
					
					if (output_buffer_p)
						{
							bool loop_flag = true;							
							Bytef input_buffer [BUFFER_SIZE];
														
							success_flag = true;
																							
							/*
								Loop whilst there is input data to compress and
								it is being compressed successfully
							*/
							while (loop_flag && success_flag)
								{
									size_t num_read = ReadFromHandler (in_p, input_buffer, BUFFER_SIZE - 1);
									int flush = Z_NO_FLUSH;
									HandlerStatus hs = GetHandlerStatus (in_p);
									
									switch (hs)
										{
											case HS_FINISHED:
												loop_flag = (strm.avail_out != 0);
												flush = Z_FINISH;
												break;
												
											case HS_BAD:
												success_flag = false;
												break;
											
											default:
												break;
										}									
									
									if (success_flag)
										{
											strm.next_in = input_buffer;
											strm.avail_in = num_read;

											/* Compress the data */
											do 
												{
													strm.next_out = output_buffer_p;
													strm.avail_out = output_buffer_size;

													/* Compress the data from the input_buffer into the output buffer */
													if (CompressData (&strm, &output_buffer_p, &output_buffer_size, flush))
														{
															const size_t output_size = output_buffer_size - strm.avail_out;												
															
															success_flag = (WriteToHandler (out_p, output_buffer_p, output_size) == output_size);
														}		/* if (output_size > 0) */
													else
														{
															success_flag = false;
														}
												}
											while (strm.avail_out == 0);	
											
										}		/* if (success_flag) */
									

								}		/* while (loop_flag && success_flag) */							

							
							if (output_buffer_p)
								{
									FreeMemory (output_buffer_p);
								}

							success_flag = (deflateEnd (&strm) == Z_OK);
							
						}		/* if (output_buffer_p) */
						
				}		/* if (InitGzipCompressor (&strm, level, &header, in_p -> ha_filename_s, file_info.fi_last_modified)) */
		
		}		/* if (CalculateFileInformation (in_p, &file_info))*/


	return (success_flag ? Z_OK : Z_STREAM_ERROR);
}




/*
 * STATIC FUNCTIONS
 */


static const char *GetCompressServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Compress service";
}


static const char *GetCompressServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service to compress data";
}


static ParameterSet *GetCompressServiceParameters (Service *service_p, Resource *resource_p, const json_t * UNUSED_PARAM (json_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Compress service parameters", "The parameters used for the Compress service");

	if (param_set_p)
		{
			ServiceData *service_data_p = service_p -> se_data_p;
			SharedType def;
				
			def.st_resource_value_p = resource_p;		

			if (CreateAndAddParameterToParameterSet (service_data_p, param_set_p, CS_INPUT.npt_type, false, CS_INPUT.npt_name_s, "Input", "The input file to read", NULL, def, NULL, NULL, PL_ALL, NULL))
				{
					ParameterMultiOptionArray *options_p = NULL;
					const char *descriptions_pp [CA_NUM_ALGORITHMS] = { "Use Raw", "Use Zip", "Use GZip" };
					SharedType values [CA_NUM_ALGORITHMS];

					values [CA_Z].st_string_value_s = (char *) s_algorithm_names_pp [CA_Z];
					values [CA_ZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_ZIP];
					values [CA_GZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_GZIP];

					options_p = AllocateParameterMultiOptionArray (CA_NUM_ALGORITHMS, descriptions_pp, values, PT_STRING, true);

					if (options_p)
						{
							def.st_string_value_s = values [0].st_string_value_s;

							if (CreateAndAddParameterToParameterSet (service_data_p, param_set_p, CS_ALGORITHM.npt_type, false, CS_ALGORITHM.npt_name_s, "Compression algorithm", "The algorithm to use to compress the data with", options_p, def, NULL, NULL, PL_ALL, NULL))
								{
									return param_set_p;
								}

							FreeParameterMultiOptionArray (options_p);
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static void ReleaseCompressServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunCompressService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p, ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	SharedType input_resource;
	
	if (GetParameterValueFromParameterSet (param_set_p, CS_INPUT.npt_name_s, &input_resource, true))
		{
			Resource *input_resource_p = input_resource.st_resource_value_p;
			SharedType value;
			
			if (GetParameterValueFromParameterSet (param_set_p, CS_ALGORITHM.npt_name_s, &value, true))
				{
					const char * const algorithm_s = value.st_string_value_s;
					
					if (algorithm_s)
						{
							Compress (input_resource_p, algorithm_s, credentials_p);
						}		/* if (algorithm_s) */
										
				}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */			
								
		}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */
	
	return service_p -> se_jobs_p;
}


static int Compress (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p)
{
	bool success_flag = false;
	int i;
	int algo_index = -1;
	int res = 0;
	
	/* Find the algorithm to use */
	for (i = 0; i < CA_NUM_ALGORITHMS; ++ i)
		{
			if (strcmp (algorithm_s, s_algorithm_names_pp [i]) == 0)
				{
					algo_index = i;
					i = CA_NUM_ALGORITHMS;
				}
		}
		
	if (algo_index)
		{
			bool replace_flag = false;

			#ifdef WINDOWS
			replace_flag = true;
			#endif
			
			char *output_name_s = SetFileExtension (input_resource_p -> re_value_s, s_algorithm_names_pp [algo_index], replace_flag);
			
			if (output_name_s)
				{
					const char *root_path_s = GetServerRootDirectory ();
					Handler *input_handler_p = GetResourceHandler (input_resource_p, root_path_s, credentials_p);

					if (input_handler_p)
						{
							Resource output_resource;
							Handler *output_handler_p = NULL;
							
							output_resource.re_protocol_s = input_resource_p -> re_protocol_s;
							output_resource.re_value_s = output_name_s;
							
							output_handler_p = GetResourceHandler (&output_resource, root_path_s, credentials_p);

							if (output_handler_p)
								{
									if (OpenHandler (input_handler_p, input_resource_p, MF_SHADOW_USE, "rb"))
										{											
											Resource *output_p = AllocateResource (PROTOCOL_FILE_S, output_name_s, output_name_s);

											if (output_p)
												{
													if (OpenHandler (output_handler_p, output_p, MF_SHADOW_USE, "wb"))
														{
															int level = Z_DEFAULT_COMPRESSION;	/* gzip default */
															res = s_compress_fns [algo_index] (input_handler_p, output_handler_p, level);

															success_flag = (res == Z_OK);

															CloseHandler (output_handler_p);
														}		/* if (OpenHandler (output_handler_p, output_name_s "wb")) */

													FreeResource (output_p);
												}
										}		/* if (OpenHandler (input_handler_p, input_resource_p -> re_value_s, "rb")) */
								
 								}		/* if (output_handler_p) */
							
							CloseHandler (input_handler_p);
						}		/* if (input_handler_p) */
								
					FreeCopiedString (output_name_s);
				}		/* if (output_name_s) */
						
		}		/* if (algo_index) */
	
	
	return success_flag;
}


static ParameterSet *IsFileForCompressService (Service * UNUSED_PARAM (service_p), Resource *resource_p, Handler *handler_p)
{
	ParameterSet *params_p = NULL;
	
	if (strcmp (resource_p -> re_protocol_s, PROTOCOL_FILE_S) == 0)
		{
			const char *filename_s = resource_p -> re_value_s;

			/*
			 * @TODO
			 * We could check if the file is on a remote filesystem and if so
			 * make a full or partial local copy for analysis.
			 */

			if (filename_s)
				{
					if (handler_p)
						{
							/*
								Rather than use the extension, let's check the file header
							*/
							uint32 header = 0;
							uint32 i = 0;

							if (OpenHandler(handler_p, resource_p, MF_SHADOW_USE, "rb"))
								{
									size_t l = sizeof (i);
									size_t r = ReadFromHandler (handler_p, &i, l);

									if (r == l)
										{

										}

									CloseHandler (handler_p);
								}

							header = htonl (i);

							if (header == 0x504B0304)
								{
									/* it's already a zip file */
								}
							else if ((header | 0x1F8BFFFF) == 0x1F8BFFFF)
								{
									/* it's a gzip file */
								}
							else
								{

								}
						}		/* if (handler_p) */
					else
						{
							/* check file extensions */
						}

				}		/* if (filename_s) */
			
		}

	return params_p;
}
