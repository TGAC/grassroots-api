#ifndef GUI_H
#define GUI_H

#include "parameter.h"
#include "parameter_library_api.h"


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_CLIENT_QT_LIBRARY_API int GetUserParameters (ParameterSet *params_set_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GUI_H */
