/*
 * blast_service_job_markup.h
 *
 *  Created on: 17 Jan 2017
 *      Author: billy
 *
 * @file
 */

#ifndef SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_SERVICE_JOB_MARKUP_H_
#define SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_SERVICE_JOB_MARKUP_H_

#include "typedefs.h"
#include "linked_list.h"

#include "blast_service_api.h"
#include "blast_service.h"

#include "jansson.h"


#ifdef __cplusplus
extern "C"
{
#endif

BLAST_SERVICE_LOCAL bool GetAndAddHitLocation (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_from_key_s, const char *hsp_to_key_s, const char *strand_key_s, const char *child_key_s);


BLAST_SERVICE_LOCAL LinkedList *GetScaffoldsFromHit (const json_t *hit_p);

BLAST_SERVICE_LOCAL bool MarkUpHit (const json_t *hit_p, json_t *mark_up_p, const char *database_s, const DatabaseType db_type);

BLAST_SERVICE_LOCAL json_t *GetInitialisedProcessedRequest (void);


BLAST_SERVICE_LOCAL bool AddTerm (json_t *root_p, const char *key_s, const char *term_s, const bool add_id_flag);



BLAST_SERVICE_LOCAL bool AddSequence (json_t *root_p, const char *key_s, const char *query_sequence_s);

BLAST_SERVICE_LOCAL bool AddFaldoTerminus (json_t *parent_json_p, const char *child_key_s, const int32 position, const bool forward_strand_flag);


BLAST_SERVICE_LOCAL bool AddIntScoreValue (json_t *parent_p, const char *key_s, int score_value);

BLAST_SERVICE_LOCAL bool AddDoubleScoreValue (json_t *parent_p, const char *key_s, double64 score_value);


BLAST_SERVICE_LOCAL bool GetAndAddDoubleScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s);

BLAST_SERVICE_LOCAL bool GetAndAddIntScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s);

BLAST_SERVICE_LOCAL bool GetAndAddSequenceValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *sequence_key_s);

BLAST_SERVICE_LOCAL bool GetAndAddDatabaseDetails (json_t *marked_up_result_p, const char *database_s);

BLAST_SERVICE_LOCAL bool GetAndAddQueryMetadata (const json_t *blast_search_p, json_t *mark_up_p);

BLAST_SERVICE_LOCAL json_t *GetAndAddMarkedUpHit (json_t *marked_up_hits_array_p, const json_t *blast_hit_p, const DatabaseType db_type);


BLAST_SERVICE_LOCAL bool AddHitDetails (json_t *marked_up_result_p, const json_t *blast_hit_p, const DatabaseType db_type);


BLAST_SERVICE_LOCAL bool AddSubsequenceMarkup (json_t *parent_p, const char *key_s, const char *subsequence_start_s, const uint32 length);

BLAST_SERVICE_LOCAL bool GetAndAddNucleotidePolymorphisms (json_t *marked_up_hsp_p, const char *reference_sequence_s, const char *hit_sequence_s, const char *midline_s, uint32 hit_index, const int32 inc_value);

BLAST_SERVICE_LOCAL bool AddPolymorphism (json_t *marked_up_hsp_p, const char *hit_gap_start_p, const char *reference_gap_start_p, const uint32 start_of_region, const uint32 end_of_region);

BLAST_SERVICE_LOCAL bool AddHsp (json_t *marked_up_hit_p, const json_t *hsp_p);



/**
 * Get the Grassroots marked-up data from a BlastServiceJob.
 *
 * @param job_p The BlastServiceJob to get the marked-up result for.
 * @return The JSON fragment containing the marked-up data or <code>
 * NULL</code> upon error.
 * @memberof BlastServiceJob
 */
BLAST_SERVICE_LOCAL json_t *MarkUpBlastResult (BlastServiceJob *job_p);


#ifdef __cplusplus
}
#endif

#endif /* SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_SERVICE_JOB_MARKUP_H_ */
