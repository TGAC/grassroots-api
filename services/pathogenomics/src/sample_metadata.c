/*
 * sample_metadata.c
 *
 *  Created on: 1 Jul 2015
 *      Author: tyrrells
 */
#include "sample_metadata.h"
#include "mongodb_tool.h"
#include "string_utils.h"
#include "country_codes.h"
#include "curl_tools.h"

bool ConvertDate (json_t *row_p)
{
	bool success_flag = false;
	const char *date_s = GetJSONString (row_p, PG_DATE_S);
	char buffer_s [9];

	if (date_s)
		{
			/* Is it DD/MM/YYYY */
			if ((strlen (date_s) == 10) && (* (date_s + 2) == '/') && (* (date_s + 5) == '/'))
				{
					memcpy (buffer_s, date_s + 6, 4 * sizeof (char));
					memcpy (buffer_s + 4, date_s + 3, 2 * sizeof (char));
					memcpy (buffer_s + 6, date_s, 2 * sizeof (char));

					* (buffer_s + 8) = '\0';
					success_flag = true;
				}
			else
				{
					#define NUM_MONTHS (12)
					const char *months_ss [NUM_MONTHS] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };

					bool loop_flag = true;
					uint month_index = 0;
					uint8 i = 0;

					while (loop_flag)
						{
							const char *month_s = * (months_ss + i);

							if (Strnicmp (month_s, date_s, strlen (month_s)) == 0)
								{
									month_index = i;
									loop_flag = false;
								}
							else
								{
									++ i;

									if (i == NUM_MONTHS)
										{
											loop_flag = false;
										}
								}

						}		/* while (loop_flag) */

					if (month_index > 0)
						{
							const char *value_p = date_s + strlen (* (months_ss + month_index));

							loop_flag = (*value_p != '\0');

							while (loop_flag)
								{
									if (isdigit (*value_p))
										{
											loop_flag = false;
										}
									else
										{
											++ value_p;

											loop_flag = (*value_p != '\0');
										}
								}

							if (*value_p != '\0')
								{
									/* Do we have a number? */
									if (isdigit (* (value_p + 1)))
										{
											bool match_flag = false;

											/* Do we have a 2 digit year... */
											if (* (value_p + 2) == '\0')
												{
													*buffer_s = '2';
													* (buffer_s + 1) = '0';

													memcpy (buffer_s + 2, value_p, 2 * sizeof (char));
													match_flag = true;
												}
											/* ... or a 4 digit one? */
											else if ((isdigit (* (value_p + 2))) && (isdigit (* (value_p + 3))))
												{
													memcpy (buffer_s, value_p, 4 * sizeof (char));
													match_flag = true;
												}

											if (match_flag)
												{
													char month_s [3];

													sprintf (month_s, "%02d", month_index + 1);
													memcpy (buffer_s + 4, month_s, 2 * sizeof (char));

													* (buffer_s + 6) = '0';
													* (buffer_s + 7) = '1';
													* (buffer_s + 8) = '\0';
													success_flag = true;
												}

										}
								}
						}		/* if (month_index > 0) */
				}

			if (success_flag)
				{
					if (json_object_set_new (row_p, PG_DATE_S, json_string (buffer_s)) != 0)
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to set date to %s", buffer_s);
							success_flag = false;
						}
				}

			if (!success_flag)
				{
					char *dump_s = json_dumps (row_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

					PrintErrors (STM_LEVEL_WARNING, "Failed to convert date from %s for %s", date_s, dump_s);

					free (dump_s);
				}
		}		/* if (date_s) */
	else
		{
			char *dump_s = json_dumps (row_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			PrintErrors (STM_LEVEL_WARNING, "No date for %s", date_s, dump_s);

			free (dump_s);
		}

	return success_flag;
}


json_t *GetLocationData (const json_t *row_p, MongoDBServiceData *data_p)
{
	json_t *res_p = NULL;

	const char *gps_s = GetJSONString (row_p, PG_GPS_S);

	if (gps_s)
		{

		}
	else
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);

			if (buffer_p)
				{
					if (AppendStringToByteBuffer (buffer_p, data_p -> msd_geocoding_uri_s))
						{
							const char *town_s = GetJSONString (row_p, PG_TOWN_S);
							const char *county_s = NULL;
							const char *country_code_s = NULL;
							bool success_flag = true;
							bool added_query_flag = false;

							/* town */
							if (town_s)
								{
									success_flag = AppendStringsToByteBuffer (buffer_p, "&query=", town_s, NULL);
									added_query_flag = true;
								}

							/* county */
							if (success_flag)
								{
									county_s = GetJSONString (row_p, PG_COUNTY_S);

									if (county_s)
										{
											if (added_query_flag)
												{
													success_flag = AppendStringsToByteBuffer (buffer_p, ",%20", county_s, NULL);
												}
											else
												{
													success_flag = AppendStringsToByteBuffer (buffer_p, "&query=", county_s, NULL);
													added_query_flag = true;
												}
										}		/* if (county_s) */

								}		/* if (success_flag) */


							/* country */
							if (success_flag)
								{
									const char *value_s = GetJSONString (row_p, PG_COUNTRY_S);

									if (value_s)
										{
											if (IsValidCountryCode (value_s))
												{
													country_code_s = value_s;
												}
											else
												{
													country_code_s = GetCountryCodeFromName (value_s);
												}

											if (country_code_s)
												{
													success_flag = AppendStringsToByteBuffer (buffer_p, "&countrycode=", country_code_s, NULL);
												}

										}

								}		/* if (success_flag) */


							if (success_flag)
								{
									CurlTool *curl_tool_p = AllocateCurlTool ();

									if (curl_tool_p)
										{
											const char *uri_s = GetByteBufferData (buffer_p);

											if (SetUriForCurlTool (curl_tool_p, uri_s))
												{
													CURLcode c = RunCurlTool (curl_tool_p);

													if (c == CURLE_OK)
														{
															const char *response_s = GetCurlToolData (curl_tool_p);

															if (response_s)
																{
																	json_error_t error;
																	json_t *raw_res_p = NULL;

																	PrintLog (STM_LEVEL_INFO, "geo response for %s\n%s\n", uri_s, response_s);

																	raw_res_p = json_loads (response_s, 0, &error);

																	if (raw_res_p)
																		{
																			char *dump_s = json_dumps (res_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

																			PrintLog (STM_LEVEL_INFO, "json:\n%s\n", dump_s);
																			free (dump_s);


																			res_p = RefineLocationData (raw_res_p, town_s, county_s, country_code_s);

																			WipeJSON (raw_res_p);
																		}
																	else
																		{

																		}
																}
														}
													else
														{

														}
												}


											FreeCurlTool (curl_tool_p);
										}		/* if (curl_tool_p) */

								}

						}		/* if (AppendStringToByteBuffer (buffer_p, data_p -> msd_geocoding_uri_s)) */

					FreeByteBuffer (buffer_p);
				}		/* if (buffer_p) */

		}


	return res_p;
}



json_t *RefineLocationData (json_t *raw_data_p, const char * const town_s, const char * const county_s, const char * const country_code_s)
{
	json_t *res_p = NULL;
	json_t *results_array_p = json_object_get (raw_data_p, "results");

	if (results_array_p)
		{
			if (json_is_array (results_array_p))
				{
					size_t index;
					json_t *result_p;

					json_array_foreach (results_array_p, index, result_p)
						{
							json_t *address_p = json_object_get (result_p, "components");

							if (address_p)
								{
									if (county_s)
										{
											const char *result_county_s = GetJSONString (address_p, "county");

											if (result_county_s)
												{
													if (Stricmp (county_s, result_county_s) == 0)
														{

														}
												}
										}
								}		/* if (address_p) */

						}		/* json_array_foreach (raw_res_p, index, raw_result_p) */

				}		/* if (json_is_array (results_p)) */

		}		/* if (results_p) */

	return res_p;
}

