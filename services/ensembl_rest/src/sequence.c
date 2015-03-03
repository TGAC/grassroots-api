/*
 * sequence.c
 *
 *  Created on: 3 Mar 2015
 *      Author: tyrrells
 */

#include "sequence.h"
#include "ensembl_rest_service.h"
#include "byte_buffer.h"


#define TAG_SEQUENCE_ID MAKE_TAG ('S', 'Q', 'I', 'D')
#define TAG_OUTPUT_FORMAT MAKE_TAG ('S', 'F', 'M', 'T')
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



static json_t *GetSequencesById (const char * const id_s, const char * const output_format_s, const char * const seq_type_s, CurlTool *curl_tool_p);



bool RunSequenceSearch (ParameterSet *params_p, json_t *res_p, CurlTool *curl_tool_p)
{
	bool success_flag = true;
	SharedType value;

	if (GetParameterValueFromParameterSet (params_p, TAG_SEQUENCE_ID, &value, true))
		{
			const char *id_s = value.st_string_value_s;

			if (!IsStringEmpty (id_s))
				{
					success_flag = false;

					if (GetParameterValueFromParameterSet (params_p, TAG_OUTPUT_FORMAT, &value, true))
						{
							const char *output_format_s = value.st_string_value_s;

							if (GetParameterValueFromParameterSet (params_p, TAG_CONTENT_TYPE, &value, true))
								{
									const char *seq_type_s = value.st_string_value_s;
									json_t *seq_res_json_p = GetSequencesById (id_s, output_format_s, seq_type_s, curl_tool_p);

									if (seq_res_json_p)
										{
											success_flag = (json_object_set_new (res_p, S_SEQUENCE_URI_S, seq_res_json_p) == 0);
										}

								}		/* if (GetParameterValueFromParameterSet (params_p, TAG_CONTENT_TYPE, &value, true)) */

						}		/* if (GetParameterValueFromParameterSet (params_p, TAG_OUTPUT_FORMAT, &value, true)) */

				}		/* if (!IsStringEmpty (id_s)) */

		}		/* if (GetParameterValueFromParameterSet (params_p, TAG_SEQUENCE_ID, &id_resource, true)) */

	return success_flag;
}



static json_t *GetSequencesById (const char * const id_s, const char * const output_format_s, const char * const seq_type_s, CurlTool *curl_tool_p)
{
	json_t *res_p = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (256);

	if (buffer_p)
		{
			if (AppendStringsToByteBuffer (buffer_p, GetRootRestURI (), S_SEQUENCE_URI_S, "/", id_s,
			  "?content-type=", output_format_s,
			  ";type=", seq_type_s,
				NULL))
				{
					if (SetUriForCurlTool (curl_tool_p, GetByteBufferData (buffer_p)))
						{
							CURLcode res = RunCurlTool (curl_tool_p);

							if (res == CURLE_OK)
								{
									const char *sequence_data_s = GetCurlToolData (curl_tool_p);

									res_p = json_string (sequence_data_s);

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
	const Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_params, sizeof (Parameter *));
	const Parameter **grouped_param_pp = grouped_params_pp;
	bool success_flag = false;
	SharedType def;

	def.st_string_value_s = "";

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Sequence ID", NULL, "An Ensembl stable ID", TAG_SEQUENCE_ID, NULL, def, NULL, NULL, PL_BASIC, NULL)) != NULL)
		{
			ParameterMultiOptionArray *output_type_options_p = NULL;
			SharedType output_types_p [ST_NUM_TYPES];
			uint32 i;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			for (i = ST_NUM_TYPES; i >= 0; -- i)
				{
					(* (output_types_p + i)).st_string_value_s = (char *) (* (S_SEQ_SEQUENCE_TYPES_PP + i));
				}

			output_type_options_p = AllocateParameterMultiOptionArray (ST_NUM_TYPES, NULL, output_types_p, PT_STRING);

			if (output_type_options_p)
				{
					def.st_string_value_s = output_types_p [0].st_string_value_s;

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Type", NULL,
					  "Type of sequence. Defaults to genomic where applicable, i.e. not translations. cdna refers to the spliced transcript sequence with UTR; cds refers to the spliced transcript sequence without UTR.",
					  TAG_CONTENT_TYPE, output_type_options_p, def, NULL, NULL, PL_BASIC, NULL)) != NULL)
						{
							ParameterMultiOptionArray *output_format_options_p = NULL;
							SharedType output_formats_p [SO_NUM_FORMATS];

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}


							for (i = SO_NUM_FORMATS; i >= 0; -- i)
								{
									(* (output_formats_p + i)).st_string_value_s = (char *) (* (S_SEQ_FORMAT_NAMES_PP + i));
								}

							output_format_options_p = AllocateParameterMultiOptionArray (SO_NUM_FORMATS, NULL, output_formats_p, PT_STRING);

							if (output_format_options_p)
								{
									def.st_string_value_s = output_formats_p [0].st_string_value_s;

									if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Output Format", NULL,
									  "The filetype that any results will be in",
									  TAG_OUTPUT_FORMAT, output_format_options_p, def, NULL, NULL, PL_BASIC, NULL)) != NULL)
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
