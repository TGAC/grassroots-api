#ifndef USER_DETAILS_H
#define USER_DETAILS_H

#include "wheatis_service_library.h"

typedef struct UserDetails
{
	char *ud_username_s;
} UserDetails;

#ifdef __cplusplus
extern "C" 
{
#endif	
	

WHEATIS_SERVICE_API UserDetails *AllocateUserDetails (const char *username_s);

WHEATIS_SERVICE_API void FreeUserDetails (UserDetails *user_details_p);


#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef USER_DETAILS_H */
