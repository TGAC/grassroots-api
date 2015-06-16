/*
 * apr_global_storage.h
 *
 *  Created on: 16 Jun 2015
 *      Author: billy
 */

#ifndef APR_GLOBAL_STORAGE_H_
#define APR_GLOBAL_STORAGE_H_

#include "apr_hash.h"
#include "httpd.h"

bool InitAPRGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *pool_p, apr_hashfunc_t hash_fn, server_rec *server_p);

APRGlobalStorage *AllocateAPRGlobalStorage (void);

void FreeAPRGlobalStorage (APRGlobalStorage *storage_p);

void DestroyAPRGlobalStorage (APRGlobalStorage *storage_p);




#endif /* APR_GLOBAL_STORAGE_H_ */
