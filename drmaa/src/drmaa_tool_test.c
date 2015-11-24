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
#include <stdio.h>
#include <stdlib.h>

#include "drmaa_tool.h"
#include "drmaa_util.h"
#include "memory_allocations.h"

DrmaaTool *CreateBlastDrmaaJob (const char *program_name_s, const char *db_name_s, const char *query_filename_s, const char *output_filename_s);

int main (int argc, char *argv [])
{
	int ret = 10;
	int num_runs = 5;
	char *program_name_s = "/tgac/software/testing/blast/2.2.30/x86_64/bin/blastn";
	char *query_filename_s = "/tgac/services/wheatis/query.txt";
	char *output_filename_s = "/tgac/services/wheatis/drmaa_test.txt";
	char *db_name_s =  "/tgac/public/databases/blast/triticum_aestivum/IWGSC/v2/IWGSCv2.0.00";

	switch (argc)
		{
			case 6:
				db_name_s = argv [5];

			case 5:
				output_filename_s = argv [4];

			case 4:
				query_filename_s = argv [3];

			case 3:
				program_name_s = argv [2];

			case 2:
				num_runs = atoi (argv [1]);
				break;

			default:
				break;
		}

	if (program_name_s)
		{
			if (InitDrmaa ())
				{
					DrmaaTool **tools_pp = AllocMemoryArray (num_runs, sizeof (DrmaaTool *));

					if (tools_pp)
						{
							size_t i;
							bool success_flag = true;

							for (i = 0; i < num_runs; ++ i)
								{
									DrmaaTool *tool_p = CreateBlastDrmaaJob (program_name_s, db_name_s, query_filename_s, output_filename_s);

									if (tool_p)
										{
											* (tools_pp + i) = tool_p;
										}
									else
										{
											success_flag = false;
											i = num_runs;
										}
								}

							if (success_flag)
								{
									for (i = 0; i < num_runs; ++ i)
										{
											success_flag = RunDrmaaTool (* (tools_pp + i), true);

											if (!success_flag)
												{
													printf ("failed to run drmaa tool %lu\n", i);
												}
										}

									if (success_flag)
										{
											bool loop_flag = true;

											/* pause for 5 seconds */
											sleep (5);

											while (loop_flag)
												{
													bool all_finished_flag = true;

													for (i = 0; i < num_runs; ++ i)
														{
															DrmaaTool *tool_p = * (tools_pp + i);

															OperationStatus status = GetDrmaaToolStatus (tool_p);
															printf ("drmaa tool %lu status %lu\n", i, status);

															if (status == OS_STARTED || status == OS_PENDING)
																{
																	all_finished_flag = false;
																}
														}

													if (all_finished_flag)
														{
															loop_flag = false;
														}
													else
														{
															/* pause for 1 seconds */
															sleep (1);
														}
												}
										}
								}

							for (i = 0; i < num_runs; ++ i)
								{
									if (* (tools_pp + i))
										{
											FreeDrmaaTool (* (tools_pp + i));
										}
								}

							FreeMemory (tools_pp);
						}		/* if (tools_pp) */

					ExitDrmaa ();
				}		/* if (InitDrmaa ()) */

		}		/* if (program_name_s) */

	return ret;
}



DrmaaTool *CreateBlastDrmaaJob (const char *program_name_s, const char *db_name_s, const char *query_filename_s, const char *output_filename_s, const char **email_addresses_ss)
{
	DrmaaTool *tool_p = AllocateDrmaaTool (program_name_s);

	if (tool_p)
		{
			if (SetDrmaaToolQueueName (tool_p, "webservices"))
				{
					const char **email_addresses_ss = { "simon.tyrrell@tgac.ac.uk", NULL };

					if (SetDrmaaToolEmailNotifictaions (tool_p, email_addresses_ss))
						{
							if (AddDrmaaToolArgument (tool_p, "-db"))
								{
									if (AddDrmaaToolArgument (tool_p, db_name_s))
										{
											if (AddDrmaaToolArgument (tool_p, "-query"))
												{
													if (AddDrmaaToolArgument (tool_p, query_filename_s))
														{
															if (AddDrmaaToolArgument (tool_p, "-out"))
																{
																	if (AddDrmaaToolArgument (tool_p, output_filename_s))
																		{
																			return tool_p;
																		}		/* if (AddDrmaaToolArgument (tool_p, output_filename_s)) */

																}		/* if (AddDrmaaToolArgument (tool_p, "-out")) */

														}		/* if (AddDrmaaToolArgument (tool_p, query_filename_s)) */

												}		/* if (AddDrmaaToolArgument (tool_p, "-query")) */

										}		/* if (AddDrmaaToolArgument (tool_p, db_name_s)) */

								}		/* if (AddDrmaaToolArgument (tool_p, "-db")) */

						}		/* if (SetDrmaaToolEmailNotifictaions (tool_p, email_addresses_ss)) */

				}		/* if (SetDrmaaToolQueueName ("webservices")) */

			FreeDrmaaTool (tool_p);
		}		/* if (tool_p) */

	return NULL;
}
