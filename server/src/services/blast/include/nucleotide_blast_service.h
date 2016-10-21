/*
 * nucleotide_blast_service.h
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */

#ifndef SERVER_SRC_SERVICES_BLAST_INCLUDE_NUCLEOTIDE_BLAST_SERVICE_H_
#define SERVER_SRC_SERVICES_BLAST_INCLUDE_NUCLEOTIDE_BLAST_SERVICE_H_


#include "blast_service_api.h"
#include "service.h"


#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_LOCAL Service *GetNucleotideBlastService ();

#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_SERVICES_BLAST_INCLUDE_NUCLEOTIDE_BLAST_SERVICE_H_ */
