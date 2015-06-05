/**
 * @file
 * @addtogroup Parameters
 * @{
 */
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

/**
 * Allocate a UserDetails with the given username.
 *
 * @param username_s The username to set. This will be copied by the
 * UserDetails so this value does not need to stay in scope after the
 * call.
 * @return A newly-allocated UserDetails or <code>NULL</code> upon error.
 * @memberof UserDetails
 */
WHEATIS_PARAMS_API UserDetails *AllocateUserDetails (const char *username_s);


/**
 * Free a UserDetails.
 *
 * @param user_details_p The UserDetails to free.
 * @memberof UserDetails
 */
WHEATIS_PARAMS_API void FreeUserDetails (UserDetails *user_details_p);

/** @} */

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef USER_DETAILS_H */
