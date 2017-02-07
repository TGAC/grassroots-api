/*
 * polymarker_service_job.c
 *
 *  Created on: 7 Feb 2017
 *      Author: billy
 */


#include "polymarker_service_job.h"
#include "polymarker_tool.hpp"


PolymarkerServiceJob *AllocatePolymarkerServiceJob (Service *service_p, const char *job_name_s, const char *job_description_s, const char *tool_name_s, PolymarkerServiceData *data_p)
{
	PolymarkerServiceJob *poly_job_p = (PolymarkerServiceJob *) AllocMemory (sizeof (PolymarkerServiceJob));

	if (poly_job_p)
		{
			PolymarkerTool *tool_p = NULL;
			ServiceJob * const base_service_job_p = & (poly_job_p -> psj_base_job);

			InitServiceJob (base_service_job_p, service_p, job_name_s, job_description_s, NULL, FreePolymarkerServiceJob);

			tool_p = CreatePolymarkerTool (data_p, base_service_job_p);

			if (tool_p)
				{
					poly_job_p -> psj_tool_p = tool_p;

					return poly_job_p;
				}		/* if (tool_p) */

			ClearServiceJob (base_service_job_p);
			FreeMemory (poly_job_p);
		}		/* if (blast_job_p) */

	return NULL;
}


void FreePolymarkerServiceJob (ServiceJob *job_p)
{
	PolymarkerServiceJob *poly_job_p = (PolymarkerServiceJob *) job_p;

	if (poly_job_p -> psj_tool_p)
		{
			FreePolymarkerTool (poly_job_p -> psj_tool_p);
		}

	ClearServiceJob (job_p);

	FreeMemory (poly_job_p);
}
