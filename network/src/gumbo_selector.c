#include "gumbo_selector.h"
#include "memory_allocations.h"


/**
 * Allocate a Selector.
 *
 * @param selector_s The selector in the css sleector syntax
 * e.g. "p.header", "ul#results". "..
 * @return A newly-allocated Selector representing the given
 * selection string or NULL upon error.
 */
GumboSelector *AllocateGumboSelector (const char *selector_s, ...)
{
	bool success_flag = true;
	bool loop_flag = true;
	va_list args;
	va_start (args, selector_s);
	const char *arg_s = selector_s;

	while (success_flag && loop_flag)
		{
			if (arg_s)
				{


					arg_s = va_arg (args, char *);
				}
			else
				{
					loop_flag = false;
				}
		}

	va_end (args);

	return success_flag;

}



bool GetSelector (char *name_s)


void FreeGumboSelector (GumboSelector *selector_p)
{

}



const char *GetGumboSelectorAttributeValue (const GumboSelector * const selector_p, const char * const key_s)
{
	const char *value_s = NULL;

	if (selector_p -> gs_attrs_p)
		{
			KeyValuePair *kvp_p = selector_p -> gs_attrs_p;
			size_t i = selector_p -> gs_num_attrs;

			while (i > 0)
				{
					if (strcmp (kvp_p -> kvp_key_s, key_s) == 0)
						{
							value_s = kvp_p -> kvp_value_s;
							i = 0;		/* force exit from loop */
						}
					else
						{
							-- i;
							++ kvp_p;
						}
				}

		}		/* if (selector_p -> gs_attrs_p) */

	return value_s;
}


