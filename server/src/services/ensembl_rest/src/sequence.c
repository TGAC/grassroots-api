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
/*
 * sequence.c
 *
 *  Created on: 3 Mar 2015
 *      Author: tyrrells
 */

#include <string.h>

#include "sequence.h"
#include "ensembl_rest_service.h"
#include "byte_buffer.h"
#include "string_utils.h"


#ifdef _DEBUG
	#define SEQUENCE_DEBUG	(STM_LEVEL_FINE)
#else
	#define SEQUENCE_DEBUG	(STM_LEVEL_NONE)
#endif


static NamedParameterType ES_SEQUENCE_ID = { "sequence_id", PT_STRING };
static NamedParameterType ES_SEQUENCE_TYPE = { "sequence_type", PT_STRING };
static NamedParameterType ES_CONTENT_TYPE = { "content_type", PT_STRING };



static const char *S_SEQUENCE_URI_S = "sequence";

static const char *S_SEQ_FORMAT_NAMES_PP [SO_NUM_FORMATS] =
{
	"json",
	"fasta",
	"seqxml",
};


static const char *S_SEQ_FORMAT_CONTENT_TYPES_PP [SO_NUM_FORMATS] =
{
	"application/json",
	"text/x-fasta",
	"text/x-seqxml+xml"
};


static const char *S_SEQ_SEQUENCE_TYPES_PP [ST_NUM_TYPES] =
{
	"genomic",
	"cds",
	"cdna",
	"protein",
};



static json_t *GetSequencesById (const char * const id_s, const char * const content_type_s, const char * const seq_type_s, CurlTool *curl_tool_p);



json_t *RunSequenceSearch (ParameterSet *params_p, CurlTool *curl_tool_p)
{
	bool success_flag = true;
	SharedType value;
	json_t *resource_json_p = NULL;

	if (GetParameterValueFromParameterSet (params_p, ES_SEQUENCE_ID.npt_name_s, &value, true))
		{
			const char *id_s = value.st_string_value_s;

			if (!IsStringEmpty (id_s))
				{
					success_flag = false;

					if (GetParameterValueFromParameterSet (params_p, ES_SEQUENCE_TYPE.npt_name_s, &value, true))
						{
							const char *seq_type_s = value.st_string_value_s;

							if (GetParameterValueFromParameterSet (params_p, ES_CONTENT_TYPE.npt_name_s, &value, true))
								{
									const char *value_s = value.st_string_value_s;
									const char *content_type_s = NULL;
									uint32 i;

									for (i = 0; i < SO_NUM_FORMATS; ++ i)
										{
											if (strcmp (value_s, * (S_SEQ_FORMAT_NAMES_PP + i)) == 0)
												{
													content_type_s = * (S_SEQ_FORMAT_CONTENT_TYPES_PP + i);
													i = SO_NUM_FORMATS; 	/* force exit from loop */
												}
										}

									if (content_type_s)
										{
											json_t *seq_res_json_p = GetSequencesById (id_s, content_type_s, seq_type_s, curl_tool_p);

											if (seq_res_json_p)
												{
													#if SEQUENCE_DEBUG >= STM_LEVEL_FINE
													PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, seq_res_json_p, "seq_res_json_p:\n");
													#endif

													resource_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, id_s, seq_res_json_p);

													json_decref (seq_res_json_p);

													if (resource_json_p)
														{
															#if SEQUENCE_DEBUG >= STM_LEVEL_FINE
															PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, resource_json_p, "resource_json_p:\n");
															#endif
														}
													else
														{
															PrintJSONToErrors (STM_LEVEL_FINE, __FILE__, __LINE__, seq_res_json_p, "Failed to create resource json for %s:\n", id_s);
														}

												}		/* if (seq_res_json_p) */

										}		/* if (content_type_s) */

								}		/* if (GetParameterValueFromParameterSet (params_p, TAG_CONTENT_TYPE, &value, true)) */

						}		/* if (GetParameterValueFromParameterSet (params_p, TAG_OUTPUT_FORMAT, &value, true)) */

				}		/* if (!IsStringEmpty (id_s)) */

		}		/* if (GetParameterValueFromParameterSet (params_p, TAG_SEQUENCE_ID, &id_resource, true)) */

	return resource_json_p;
}



static json_t *GetSequencesById (const char * const id_s, const char * const content_type_s, const char * const seq_type_s, CurlTool *curl_tool_p)
{
	json_t *res_p = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (256);

	if (buffer_p)
		{
			if (AppendStringsToByteBuffer (buffer_p, GetRootRestURI (), S_SEQUENCE_URI_S, "/id/", id_s, "?type=", seq_type_s, NULL))
				{
					if (SetUriForCurlTool (curl_tool_p, GetByteBufferData (buffer_p)))
						{
							if (SetCurlToolHeader (curl_tool_p, "Content-type", content_type_s))
								{
									CURLcode res = RunCurlTool (curl_tool_p);

									if (res == CURLE_OK)
										{
											const char *sequence_data_s = GetCurlToolData (curl_tool_p);

											/* is the reponse a json object? */
											if (strcmp (content_type_s,  * (S_SEQ_FORMAT_CONTENT_TYPES_PP + SO_JSON)) == 0)
												{
													json_error_t error;

													res_p = json_loads (sequence_data_s, 0, &error);

													if (!res_p)
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create sequence json_result object for:\n%s\n", sequence_data_s);
														}
												}

											if (!res_p)
												{
													res_p = json_string (sequence_data_s);
												}

											if (!res_p)
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create sequence json_result string for:\n%s\n", sequence_data_s);
												}

										}		/* if (res == CURLE_OK) */

								}		/* if (SetCurlToolHeader (curl_tool_p , content_type_s)) */


						}		/* if (SetUriForCurlTool (curl_tool_p, GetByteBufferData (buffer_p))) */

				}

			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */

	return res_p;
}


bool AddSequenceParameters (ServiceData *data_p, ParameterSet *param_set_p)
{
	Parameter *param_p = NULL;
	bool success_flag = false;
	SharedType def;
	ParameterGroup *group_p = CreateAndAddParameterGroupToParameterSet ("Sequence Parameters", NULL, data_p, param_set_p);

	def.st_string_value_s = NULL;

	if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, group_p, ES_SEQUENCE_ID.npt_type, ES_SEQUENCE_ID.npt_name_s, "Sequence ID", "An Ensembl stable ID", def, PL_ALL)) != NULL)
		{
			LinkedList *options_p = CreateProgramOptionsList ();

			if (options_p)
				{
					uint32 i;

					success_flag = true;

					for (i = 0; i < ST_NUM_TYPES; ++ i)
						{
							def.st_string_value_s = (char *) (* (S_SEQ_SEQUENCE_TYPES_PP + i));

							if (!CreateAndAddParameterOption (options_p, def, NULL, PT_STRING))
								{
									i = ST_NUM_TYPES;
									success_flag = false;
								}
						}

					if (success_flag)
						{
							def.st_string_value_s = (char *) (*S_SEQ_SEQUENCE_TYPES_PP);

							if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, group_p, ES_SEQUENCE_TYPE.npt_type, ES_SEQUENCE_TYPE.npt_name_s, "Type",
							  "Type of sequence. Defaults to genomic where applicable, i.e. not translations. cdna refers to the spliced transcript sequence with UTR; cds refers to the spliced transcript sequence without UTR.",
								def, PL_ALL)) != NULL)
								{

									def.st_string_value_s = (char *) (*S_SEQ_FORMAT_NAMES_PP);

									if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, group_p, ES_CONTENT_TYPE.npt_type, ES_CONTENT_TYPE.npt_name_s, "Output Format",
									  "The filetype that any results will be in",
									 def, PL_ALL)) != NULL)

									for (i = 0; i < SO_NUM_FORMATS; ++ i)
										{
											def.st_string_value_s = (char *) (* (S_SEQ_FORMAT_NAMES_PP + i));

											if (!CreateAndAddParameterOptionToParameter (param_p, def, NULL))
												{
													i = SO_NUM_FORMATS;
													success_flag = false;
												}
										}

								}
							else
								{
									FreeLinkedList (options_p);
								}
						}
					else
						{
							FreeLinkedList (options_p);
						}

					FreeLinkedList (options_p);
				}
		}

	return success_flag;
}



const char **GetSequenceFormatNames (void)
{
	return S_SEQ_FORMAT_NAMES_PP;
}


const char **GetSequenceContentTypes (void)
{
	return S_SEQ_FORMAT_CONTENT_TYPES_PP;
}


const char **GetSequenceSequenceTypes (void)
{
	return S_SEQ_SEQUENCE_TYPES_PP;
}
