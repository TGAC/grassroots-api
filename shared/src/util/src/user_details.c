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

#include <string.h>

#include "user_details.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "json_util.h"

/******************************************************************/

static UserAuthentication *AllocateUserAuthentication (const char *system_s, const char *username_s, const char *password_s, const char *token_s);

static void FreeUserAuthentication (UserAuthentication *auth_p);

static UserAuthenticationNode *AllocateUserAuthenticationNode (UserAuthentication *auth_p);

static void FreeUserAuthenticationNode (ListItem *item_p);

/******************************************************************/

UserDetails *AllocateUserDetails (const json_t *credentials_p)
{
	LinkedList *list_p = AllocateLinkedList (FreeUserAuthenticationNode);

	if (list_p)
		{
			UserDetails *user_p = (UserDetails *) AllocMemory (sizeof (UserDetails));

			if (user_p)
				{
					const char *key_s;
					json_t *value_p;

					user_p -> ud_auth_list_p = list_p;

					if (credentials_p)
						{
							json_object_foreach (credentials_p, key_s, value_p)
								{
									const char *username_s = GetJSONString (value_p, CREDENTIALS_USERNAME_S);

									if (username_s)
										{
											const char *password_s = GetJSONString (value_p, CREDENTIALS_PASSWORD_S);
											const char *token_s = GetJSONString (value_p, CREDENTIALS_TOKEN_KEY_S);

											if (!AddUserAuthentication (user_p, key_s, username_s, password_s, token_s))
												{

												}
										}
								}
						}

					return user_p;
				}		/* if (user_p) */

			FreeLinkedList (list_p);
		}		/* if (list_p) */

	return NULL;
}


void FreeUserDetails (UserDetails *user_details_p)
{
	FreeLinkedList (user_details_p -> ud_auth_list_p);
	FreeMemory (user_details_p);
}


const UserAuthentication *GetUserAuthenticationForSystem (const UserDetails *user_p, const char *system_s, const char **username_ss, const char **password_ss, const char **token_ss)
{
	bool success_flag = false;
	UserAuthenticationNode *node_p = (UserAuthenticationNode *) (user_p -> ud_auth_list_p -> ll_head_p);


	while (node_p && (!success_flag))
		{
			UserAuthentication *auth_p = node_p -> uan_auth_p;

			if (strcmp (auth_p -> ua_system_id_s, system_s) == 0)
				{
					return auth_p;
				}

			node_p = (UserAuthenticationNode *) (node_p -> uan_base_node.ln_next_p);
		}


	return NULL;
}



bool AddUserAuthentication (UserDetails *user_details_p, const char *system_s, const char *username_s, const char *password_s, const char *token_s)
{
	UserAuthentication *auth_p = AllocateUserAuthentication (system_s, username_s, password_s, token_s);

	if (auth_p)
		{
			UserAuthenticationNode *node_p = AllocateUserAuthenticationNode (auth_p);

			if (node_p)
				{
					LinkedListAddTail (user_details_p -> ud_auth_list_p, & (node_p -> uan_base_node));

					return true;
				}

			FreeUserAuthentication (auth_p);
		}		/* if (auth_p) */

	return false;
}


/******************************************************************/


static UserAuthenticationNode *AllocateUserAuthenticationNode (UserAuthentication *auth_p)
{
	UserAuthenticationNode *node_p = (UserAuthenticationNode *) AllocMemory (sizeof (UserAuthenticationNode));

	if (node_p)
		{
			node_p -> uan_base_node.ln_prev_p = NULL;
			node_p -> uan_base_node.ln_next_p = NULL;

			node_p -> uan_auth_p = auth_p;
		}

	return node_p;
}


static void FreeUserAuthenticationNode (ListItem *item_p)
{
	UserAuthenticationNode *node_p = (UserAuthenticationNode *) item_p;

	FreeUserAuthentication (node_p -> uan_auth_p);
	FreeMemory (node_p);
}


static UserAuthentication *AllocateUserAuthentication (const char *system_s, const char *username_s, const char *password_s, const char *token_s)
{
	char *sys_s = CopyToNewString (system_s, 0, false);

	if (sys_s)
		{
			char *user_s = CopyToNewString (username_s, 0, false);

			if (user_s)
				{
					bool continue_flag = true;
					char *pass_s = NULL;

					if (password_s)
						{
							pass_s = CopyToNewString (password_s, 0, false);

							continue_flag = (pass_s != NULL);
						}		/* if (password_s) */

					if (continue_flag)
						{
							char *tok_s = NULL;

							if (token_s)
								{
									tok_s = CopyToNewString (token_s, 0, false);

									continue_flag = (tok_s != NULL);
								}		/* if (token_s) */

							if (continue_flag)
								{
									UserAuthentication *auth_p = (UserAuthentication *) AllocMemory (sizeof (UserAuthentication));

									if (auth_p)
										{
											auth_p -> ua_system_id_s = sys_s;
											auth_p -> ua_username_s = user_s;
											auth_p -> ua_password_s = pass_s;
											auth_p -> ua_oath_totp_token_s = tok_s;

											return auth_p;
										}		/* if (auth_p) */

								}		/* if (continue_flag) */

						}		/* if (continue_flag) */

					FreeCopiedString (user_s);
				}		/* if (user_s) */

			FreeCopiedString (sys_s);
		}		/* if (sys_s) */

	return NULL;
}


static void FreeUserAuthentication (UserAuthentication *auth_p)
{
	FreeCopiedString (auth_p -> ua_system_id_s);
	FreeCopiedString (auth_p -> ua_username_s);

	if (auth_p -> ua_oath_totp_token_s)
		{
			FreeCopiedString (auth_p -> ua_oath_totp_token_s);
		}

	if (auth_p -> ua_password_s)
		{
			FreeCopiedString (auth_p -> ua_password_s);
		}

	FreeMemory (auth_p);
}
