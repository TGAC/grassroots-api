#include <stdio.h>
#include "drmaa_tool.h"
#include "drmaa_util.h"


DrmaaTool *CreateBlastDrmaaJob (const char *program_name_s, const char *db_name_s, const char *query_filename_s, const char *output_filename_s);

int main (int argc, char *argv [])
{
	int ret = 10;
	char *program_name_s = "/tgac/software/testing/blast/2.2.30/x86_64/bin/blastn";
	char *query_filename_s = "/tgac/services/wheatis/query.txt";
	char *output_filename_s = "/tgac/services/wheatis/drmaa_test.txt";
	char *db_name_s =  "/tgac/public/databases/blast/triticum_aestivum/IWGSC/v2/IWGSCv2.0.00";

	switch (argc)
		{
			case 5:
				db_name_s = argv [4];

			case 4:
				output_filename_s = argv [3];

			case 3:
				query_filename_s = argv [2];

			case 2:
				program_name_s = argv [1];
				break;

			default:
				break;
		}

	if (program_name_s)
		{
			if (InitDrmaa ())
				{
					#define NUM_TOOLS (30)

					DrmaaTool *tools_pp [NUM_TOOLS] = { 0 };
					size_t i;
					bool success_flag = true;

					for (i = 0; i < NUM_TOOLS; ++ i)
						{
							DrmaaTool *tool_p = CreateBlastDrmaaJob (program_name_s, db_name_s, query_filename_s, output_filename_s);

							if (tool_p)
								{
									* (tools_pp + i) = tool_p;
								}
							else
								{
									success_flag = false;
									i = NUM_TOOLS;
								}
						}

					if (success_flag)
						{
							for (i = 0; i < NUM_TOOLS; ++ i)
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

											for (i = 0; i < NUM_TOOLS; ++ i)
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

					for (i = 0; i < NUM_TOOLS; ++ i)
						{
							if (* (tools_pp + i))
								{
									FreeDrmaaTool (* (tools_pp + i));
								}
						}

					ExitDrmaa ();
				}		/* if (InitDrmaa ()) */

		}		/* if (program_name_s) */

	return ret;
}



DrmaaTool *CreateBlastDrmaaJob (const char *program_name_s, const char *db_name_s, const char *query_filename_s, const char *output_filename_s)
{
	DrmaaTool *tool_p = AllocateDrmaaTool (program_name_s);

	if (tool_p)
		{
			if (SetDrmaaToolQueueName (tool_p, "-q webservices"))
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

				}		/* if (SetDrmaaToolQueueName ("-q webservices")) */

			FreeDrmaaTool (tool_p);
		}		/* if (tool_p) */

	return NULL;
}
