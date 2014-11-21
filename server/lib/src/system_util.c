#include "system_util.h"

#include "handler_utils.h"
#include "streams.h"



bool InitInformationSystem ()
{
	if (InitHandlerUtil ())
		{
			if (InitDefaultOutputStream ())
				{
					return true;
				}
		}

	return false;
}


bool DestroyInformationSystem ()
{
	bool res_flag = true;
	
	FreeDefaultOutputStream ();
	DestroyHandlerUtil ();

	return res_flag;
}
