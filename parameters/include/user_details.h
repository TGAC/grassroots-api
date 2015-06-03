#ifndef USER_DETAILS_H
#define USER_DETAILS_H

#include "wheatis_params_library.h"

typedef struct UserDetails
{
	char *ud_username_s;
	char *ud_password_s;
	char *ud_oath_totp_token_s;
} UserDetails;

#ifdef __cplusplus
extern "C" 
{
#endif	
	

WHEATIS_PARAMS_API UserDetails *AllocateUserDetails (const char *username_s);

WHEATIS_PARAMS_API void FreeUserDetails (UserDetails *user_details_p);


#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef USER_DETAILS_H */
