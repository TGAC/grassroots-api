#include <stdio.h>
#include "drmaa_tool.h"
#include "drmaa_util.h"

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
																			if (RunDrmaaTool (tool_p, false))
																				{
																					printf ("drmaa test ran successfully\n");
																				}

																		}		/* if (AddDrmaaToolArgument (tool_p, output_filename_s)) */

																}		/* if (AddDrmaaToolArgument (tool_p, "-out")) */

														}		/* if (AddDrmaaToolArgument (tool_p, query_filename_s)) */

												}		/* if (AddDrmaaToolArgument (tool_p, "-query")) */

										}		/* if (AddDrmaaToolArgument (tool_p, db_name_s)) */

								}		/* if (AddDrmaaToolArgument (tool_p, "-db")) */

						}		/* if (tool_p) */
					
					FreeDrmaaTool (tool_p);
				}		/* if (SetDrmaaToolQueueName ("-q webservices")) */
			ExitDrmaa ();
			}
		}		/* if (program_name_s) */

	return ret;
}
