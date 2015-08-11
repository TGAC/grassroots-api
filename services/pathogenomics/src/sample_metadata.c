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
#include "json_util.h"
#include "json_tools.h"


#ifdef _DEBUG
	#define SAMPLE_METADATA_DEBUG	(STM_LEVEL_FINE)
#else
	#define SAMPLE_METADATA_DEBUG	(STM_LEVEL_NONE)
#endif


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


json_t *GetLocationDataByGoogle (MongoDBServiceData *data_p, const json_t *row_p)
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
					if (AppendStringToByteBuffer (buffer_p, data_p -> msd_geocoder_uri_s))
						{
							const char *postcode_s = GetJSONString (row_p, PG_POSTCODE_S);
							const char *town_s = NULL;
							const char *county_s = NULL;
							const char *country_s = GetJSONString (row_p, PG_COUNTRY_S);
							const char *country_code_s = NULL;
							bool success_flag = true;
							bool added_query_flag = false;


							/* post code */
							if (postcode_s)
								{
									success_flag = AppendStringsToByteBuffer (buffer_p, "components=postal_code:", postcode_s, NULL);

									/* country */
									if (success_flag)
										{
											if (country_s)
												{
													if (IsValidCountryCode (country_s))
														{
															country_code_s = country_s;
														}
													else
														{
															country_code_s = GetCountryCodeFromName (country_s);
														}

													if (country_code_s)
														{
															success_flag = AppendStringsToByteBuffer (buffer_p, "|country:", country_code_s, NULL);
														}


												}

										}		/* if (success_flag) */

								}		/* if (postcode_s) */
							else
								{
									town_s = GetJSONString (row_p, PG_TOWN_S);

									/* town */
									if (town_s)
										{
											success_flag = AppendStringsToByteBuffer (buffer_p, "&address=", town_s, NULL);
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
															success_flag = AppendStringsToByteBuffer (buffer_p, "&address=", county_s, NULL);
															added_query_flag = true;
														}
												}		/* if (county_s) */

										}		/* if (success_flag) */


									/* country */
									if (success_flag)
										{
											if (country_s)
												{
													if (IsValidCountryCode (country_s))
														{
															country_code_s = country_s;
														}
													else
														{
															country_code_s = GetCountryCodeFromName (country_s);
														}

													if (country_code_s)
														{
															success_flag = AppendStringsToByteBuffer (buffer_p, "&components=country:", country_code_s, NULL);
														}
												}
										}

								}


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
																			char *dump_s = json_dumps (raw_res_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

																			PrintLog (STM_LEVEL_INFO, "json:\n%s\n", dump_s);
																			free (dump_s);


																			res_p = data_p -> msd_refine_location_fn (data_p, raw_res_p, town_s, county_s, country_code_s);

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


json_t *GetLocationDataByOpenCage (MongoDBServiceData *data_p, const json_t *row_p)
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
					if (AppendStringToByteBuffer (buffer_p, data_p -> msd_geocoder_uri_s))
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


																			res_p = data_p -> msd_refine_location_fn (data_p, raw_res_p, town_s, county_s, country_code_s);

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


/*
 {
    "results" : [
      {
         "address_components" : [
            {
               "long_name" : "Werbig",
               "short_name" : "Werbig",
               "types" : [ "sublocality_level_1", "sublocality", "political" ]
            },
            {
               "long_name" : "Brandenburg",
               "short_name" : "BB",
               "types" : [ "administrative_area_level_1", "political" ]
            },
            {
               "long_name" : "Germany",
               "short_name" : "DE",
               "types" : [ "country", "political" ]
            }
         ],
         "formatted_address" : "Werbig, Germany",
         "geometry" : {
            "bounds" : {
               "northeast" : {
                  "lat" : 52.5949616,
                  "lng" : 14.4702795
               },
               "southwest" : {
                  "lat" : 52.54852649999999,
                  "lng" : 14.3639304
               }
            },
            "location" : {
               "lat" : 52.5718829,
               "lng" : 14.3937375
            },
            "location_type" : "APPROXIMATE",
            "viewport" : {
               "northeast" : {
                  "lat" : 52.5949616,
                  "lng" : 14.4702795
               },
               "southwest" : {
                  "lat" : 52.54852649999999,
                  "lng" : 14.3639304
               }
            }
         },
         "place_id" : "ChIJQzFlvbR5B0cRQL6B1kggIQo",
         "types" : [ "sublocality_level_1", "sublocality", "political" ]
      },
      {
         "address_components" : [
            {
               "long_name" : "Werbig",
               "short_name" : "Werbig",
               "types" : [ "sublocality_level_1", "sublocality", "political" ]
            },
            {
               "long_name" : "Bad Belzig",
               "short_name" : "Bad Belzig",
               "types" : [ "locality", "political" ]
            },
            {
               "long_name" : "Brandenburg",
               "short_name" : "BB",
               "types" : [ "administrative_area_level_1", "political" ]
            },
            {
               "long_name" : "Germany",
               "short_name" : "DE",
               "types" : [ "country", "political" ]
            }
         ],
         "formatted_address" : "Werbig, Bad Belzig, Germany",
         "geometry" : {
            "bounds" : {
               "northeast" : {
                  "lat" : 52.2306637,
                  "lng" : 12.5418018
               },
               "southwest" : {
                  "lat" : 52.1621099,
                  "lng" : 12.4341461
               }
            },
            "location" : {
               "lat" : 52.2024014,
               "lng" : 12.4757793
            },
            "location_type" : "APPROXIMATE",
            "viewport" : {
               "northeast" : {
                  "lat" : 52.2306637,
                  "lng" : 12.5418018
               },
               "southwest" : {
                  "lat" : 52.1621099,
                  "lng" : 12.4341461
               }
            }
         },
         "place_id" : "ChIJGdomWMeiqEcRVKxTUIFd3gs",
         "types" : [ "sublocality_level_1", "sublocality", "political" ]
      },
      {
         "address_components" : [
            {
               "long_name" : "Werbig",
               "short_name" : "Werbig",
               "types" : [ "sublocality_level_1", "sublocality", "political" ]
            },
            {
               "long_name" : "Niederer Fläming",
               "short_name" : "Niederer Fläming",
               "types" : [ "locality", "political" ]
            },
            {
               "long_name" : "Brandenburg",
               "short_name" : "BB",
               "types" : [ "administrative_area_level_1", "political" ]
            },
            {
               "long_name" : "Germany",
               "short_name" : "DE",
               "types" : [ "country", "political" ]
            },
            {
               "long_name" : "14913",
               "short_name" : "14913",
               "types" : [ "postal_code" ]
            }
         ],
         "formatted_address" : "Werbig, 14913 Niederer Fläming, Germany",
         "geometry" : {
            "bounds" : {
               "northeast" : {
                  "lat" : 51.9568939,
                  "lng" : 13.2256467
               },
               "southwest" : {
                  "lat" : 51.9140655,
                  "lng" : 13.1596687
               }
            },
            "location" : {
               "lat" : 51.9320151,
               "lng" : 13.1920298
            },
            "location_type" : "APPROXIMATE",
            "viewport" : {
               "northeast" : {
                  "lat" : 51.9568939,
                  "lng" : 13.2256467
               },
               "southwest" : {
                  "lat" : 51.9140655,
                  "lng" : 13.1596687
               }
            }
         },
         "place_id" : "ChIJsceHQvDYp0cR8BCC1kggIQo",
         "types" : [ "sublocality_level_1", "sublocality", "political" ]
      }
   ],
   "status" : "OK"
}

 */
json_t *RefineLocationDataForGoogle (MongoDBServiceData *service_data_p, json_t *raw_data_p, const char * const town_s, const char * const county_s, const char * const country_code_s)
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
									bool match_flag = false;

									#if SAMPLE_METADATA_DEBUG >=STM_LEVEL_FINE
									PrintJSONToLog (address_p, "Address: ", STM_LEVEL_FINE);
									#endif

									if (county_s)
										{
											const char *result_county_s = GetJSONString (address_p, "county");

											if (result_county_s)
												{
													if (Stricmp (county_s, result_county_s) == 0)
														{
															match_flag = true;
														}
												}
										}		/* if (county_s) */



								}		/* if (address_p) */

						}		/* json_array_foreach (raw_res_p, index, raw_result_p) */

				}		/* if (json_is_array (results_p)) */

		}		/* if (results_p) */

	return res_p;
}


/*
 {
    "licenses" : [
       {
          "name" : "CC-BY-SA",
          "url" : "http://creativecommons.org/licenses/by-sa/3.0/"
       },
       {
          "name" : "ODbL",
          "url" : "http://opendatacommons.org/licenses/odbl/summary/"
       }
    ],
    "rate" : {
       "limit" : 2500,
       "remaining" : 2489,
       "reset" : 1439337600
    },
    "results" : [
       {
          "annotations" : {
             "DMS" : {
                "lat" : "52\u00b0 37' 42.98160'' N",
                "lng" : "1\u00b0 17' 32.17200'' E"
             },
             "MGRS" : "31UCU8441732326",
             "Maidenhead" : "JO02pp50bu",
             "Mercator" : {
                "x" : 143854.838,
                "y" : 6880612.848
             },
             "OSGB" : {
                "easting" : 622783.039,
                "gridref" : "TG 227 085",
                "northing" : 308555.893
             },
             "OSM" : {
                "url" : "http://www.openstreetmap.org/?mlat=52.62861&mlon=1.29227#map=17/52.62861/1.29227"
             },
             "callingcode" : 44,
             "geohash" : "u12gmktpb54r76b7ngce",
             "sun" : {
                "rise" : {
                   "astronomical" : 1439257860,
                   "civil" : 1439265060,
                   "nautical" : 1439261940
                },
                "set" : {
                   "astronomical" : 1439330700,
                   "civil" : 1439323680,
                   "nautical" : 1439326800
                }
             },
             "timezone" : {
                "name" : "Europe/London",
                "now_in_dst" : 1,
                "offset_sec" : 3600,
                "offset_string" : 100,
                "short_name" : "BST"
             },
             "what3words" : {
                "words" : "dragon.crunch.coins"
             }
          },
          "bounds" : {
             "northeast" : {
                "lat" : 52.6849096,
                "lng" : 1.5407848
             },
             "southwest" : {
                "lat" : 52.555435,
                "lng" : 1.2038691
             }
          },
          "components" : {
             "city" : "Norwich",
             "country" : "United Kingdom",
             "country_code" : "gb",
             "county" : "Norfolk",
             "state" : "England",
             "state_district" : "East of England"
          },
          "confidence" : 5,
          "formatted" : "Norwich, Norfolk, United Kingdom",
          "geometry" : {
             "lat" : 52.628606,
             "lng" : 1.29227
          }
       },
       {
          "annotations" : {
             "DMS" : {
                "lat" : "52\u00b0 37' 36.40656'' N",
                "lng" : "1\u00b0 18' 24.44434'' E"
             },
             "MGRS" : "31UCU8539432100",
             "Maidenhead" : "JO02pp60tk",
             "Mercator" : {
                "x" : 145471.208,
                "y" : 6880278.724
             },
             "OSGB" : {
                "easting" : 623774.629,
                "gridref" : "TG 237 083",
                "northing" : 308397.845
             },
             "OSM" : {
                "url" : "http://www.openstreetmap.org/?mlat=52.62678&mlon=1.30679#map=17/52.62678/1.30679"
             },
             "callingcode" : 44,
             "geohash" : "u12gmsrt4qv26np1jsqv",
             "sun" : {
                "rise" : {
                   "astronomical" : 1439257860,
                   "civil" : 1439265060,
                   "nautical" : 1439261940
                },
                "set" : {
                   "astronomical" : 1439330700,
                   "civil" : 1439323680,
                   "nautical" : 1439326740
                }
             },
             "timezone" : {
                "name" : "Europe/London",
                "now_in_dst" : 1,
                "offset_sec" : 3600,
                "offset_string" : 100,
                "short_name" : "BST"
             },
             "what3words" : {
                "words" : "ample.sketch.cares"
             }
          },
          "bounds" : {
             "northeast" : {
                "lat" : 52.6271858,
                "lng" : 1.3078487
             },
             "southwest" : {
                "lat" : 52.6263854,
                "lng" : 1.306142
             }
          },
          "components" : {
             "city" : "Norwich",
             "country" : "United Kingdom",
             "country_code" : "gb",
             "county" : "Norfolk",
             "postcode" : "NR1 1EF",
             "road" : "Station Approach",
             "state" : "England",
             "state_district" : "East of England",
             "station" : "Norwich",
             "suburb" : "Thorpe Hamlet"
          },
          "confidence" : 10,
          "formatted" : "Norwich, Station Approach, Norwich NR1 1EF, United Kingdom",
          "geometry" : {
             "lat" : 52.6267796,
             "lng" : 1.3067900949115
          }
       }
    ],
    "status" : {
       "code" : 200,
       "message" : "OK"
    },
    "stay_informed" : {
       "blog" : "http://blog.opencagedata.com",
       "twitter" : "https://twitter.com/opencagedata"
    },
    "thanks" : "For using an OpenCage Data API",
    "timestamp" : {
       "created_http" : "Tue, 11 Aug 2015 16:18:05 GMT",
       "created_unix" : 1439309885
    },
    "total_results" : 2
 }
*/
json_t *RefineLocationDataForOpenCage (MongoDBServiceData *service_data_p, json_t *raw_data_p, const char * const town_s, const char * const county_s, const char * const country_code_s)
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
									bool match_flag = false;

									#if SAMPLE_METADATA_DEBUG >=STM_LEVEL_FINE
									PrintJSONToLog (address_p, "Address: ", STM_LEVEL_FINE);
									#endif

									if (county_s)
										{
											const char *result_county_s = GetJSONString (address_p, "county");

											if (result_county_s)
												{
													if (Stricmp (county_s, result_county_s) == 0)
														{
															match_flag = true;
														}
												}
										}		/* if (county_s) */



								}		/* if (address_p) */

						}		/* json_array_foreach (raw_res_p, index, raw_result_p) */

				}		/* if (json_is_array (results_p)) */

		}		/* if (results_p) */

	return res_p;
}

