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



#define TAG_SEQUENCE_ID MAKE_TAG ('S', 'Q', 'I', 'D')
#define TAG_SEQUENCE_TYPE MAKE_TAG ('S', 'T', 'Y', 'P')
#define TAG_CONTENT_TYPE MAKE_TAG ('S', 'C', 'O', 'T')


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



bool RunSequenceSearch (ParameterSet *params_p, json_t *results_array_json_p, CurlTool *curl_tool_p)
{
	bool success_flag = true;
	SharedType value;

	if (GetParameterValueFromParameterSet (params_p, TAG_SEQUENCE_ID, &value, true))
		{
			const char *id_s = value.st_string_value_s;

			if (!IsStringEmpty (id_s))
				{
					success_flag = false;

					if (GetParameterValueFromParameterSet (params_p, TAG_SEQUENCE_TYPE, &value, true))
						{
							const char *seq_type_s = value.st_string_value_s;

							if (GetParameterValueFromParameterSet (params_p, TAG_CONTENT_TYPE, &value, true))
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

									json_t *seq_res_json_p = GetSequencesById (id_s, content_type_s, seq_type_s, curl_tool_p);

									if (seq_res_json_p)
										{
											#if SEQUENCE_DEBUG >= STM_LEVEL_FINE
											char *dump_s = json_dumps (seq_res_json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);
											printf ("seq_res_json_p:\n%s\n", dump_s);
											free (dump_s);
											#endif

											json_t *resource_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, id_s, seq_res_json_p);

											#if SEQUENCE_DEBUG >= STM_LEVEL_FINE
											dump_s = json_dumps (resource_json_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);
											printf ("resource_json_p:\n%s\n", dump_s);
											free (dump_s);
											#endif

											success_flag = (json_array_append_new (results_array_json_p, resource_json_p) == 0);
										}

								}		/* if (GetParameterValueFromParameterSet (params_p, TAG_CONTENT_TYPE, &value, true)) */

						}		/* if (GetParameterValueFromParameterSet (params_p, TAG_OUTPUT_FORMAT, &value, true)) */

				}		/* if (!IsStringEmpty (id_s)) */

		}		/* if (GetParameterValueFromParameterSet (params_p, TAG_SEQUENCE_ID, &id_resource, true)) */

	return success_flag;
}



static json_t *GetSequencesById (const char * const id_s, const char * const content_type_s, const char * const seq_type_s, CurlTool *curl_tool_p)
{
	json_t *res_p = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (256);

	if (buffer_p)
		{
			if (AppendStringsToByteBuffer (buffer_p, GetRootRestURI (), S_SEQUENCE_URI_S, "/id/", id_s,
			  "?content-type=", content_type_s,
			  ";type=", seq_type_s,
				NULL))
				{
					if (SetUriForCurlTool (curl_tool_p, GetByteBufferData (buffer_p)))
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
													PrintErrors (STM_LEVEL_SEVERE, "Failed to create sequence json_result object for:\n%s\n", sequence_data_s);
												}
										}

									if (!res_p)
										{
											res_p = json_string (sequence_data_s);
										}

									if (!res_p)
										{
											PrintErrors (STM_LEVEL_SEVERE, "Failed to create sequence json_result string for:\n%s\n", sequence_data_s);
										}

								}		/* if (res == CURLE_OK) */

						}		/* if (SetUriForCurlTool (curl_tool_p, GetByteBufferData (buffer_p))) */

				}

			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */

	return res_p;
}


bool AddSequenceParameters (ParameterSet *param_set_p)
{
	const uint32 num_params = 3;
	Parameter *param_p = NULL;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;
	bool success_flag = false;
	SharedType def;

	def.st_string_value_s = NULL;

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "Sequence ID", NULL, "An Ensembl stable ID", TAG_SEQUENCE_ID, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
		{
			ParameterMultiOptionArray *output_type_options_p = NULL;
			SharedType output_types_p [ST_NUM_TYPES];
			uint32 i;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			for (i = 0; i < ST_NUM_TYPES; ++ i)
				{
					(* (output_types_p + i)).st_string_value_s = (char *) (* (S_SEQ_SEQUENCE_TYPES_PP + i));
				}

			output_type_options_p = AllocateParameterMultiOptionArray (ST_NUM_TYPES, NULL, output_types_p, PT_STRING);

			if (output_type_options_p)
				{
					def.st_string_value_s = output_types_p [0].st_string_value_s;

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "Type", NULL,
					  "Type of sequence. Defaults to genomic where applicable, i.e. not translations. cdna refers to the spliced transcript sequence with UTR; cds refers to the spliced transcript sequence without UTR.",
					  TAG_SEQUENCE_TYPE, output_type_options_p, def, NULL, NULL, PL_ALL, NULL)) != NULL)
						{
							ParameterMultiOptionArray *output_format_options_p = NULL;
							SharedType output_formats_p [SO_NUM_FORMATS];

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}

							for (i = 0; i < SO_NUM_FORMATS; ++ i)
								{
									(* (output_formats_p + i)).st_string_value_s = (char *) (* (S_SEQ_FORMAT_NAMES_PP + i));
								}

							output_format_options_p = AllocateParameterMultiOptionArray (SO_NUM_FORMATS, NULL, output_formats_p, PT_STRING);

							if (output_format_options_p)
								{
									def.st_string_value_s = output_formats_p [0].st_string_value_s;

									if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "Output Format", NULL,
									  "The filetype that any results will be in",
									  TAG_CONTENT_TYPE, output_format_options_p, def, NULL, NULL, PL_ALL, NULL)) != NULL)
										{
											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;

													if (!AddParameterGroupToParameterSet (param_set_p, "Sequence", grouped_params_pp, num_params))
														{
															PrintErrors (STM_LEVEL_WARNING, "Failed to add sequence grouping");
															FreeMemory (grouped_params_pp);
														}
												}

											success_flag = true;
										}
									else
										{
											FreeParameterMultiOptionArray (output_format_options_p);
										}
								}		/* if (output_format_options_p) */

						}

					if (!success_flag)
						{
							FreeParameterMultiOptionArray (output_type_options_p);
						}

				}		/* if (output_type_options_p) */
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
