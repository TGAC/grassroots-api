#ifndef SERVICE_CONFIG_H
#define SERVICE_CONFIG_H

#include "wheatis_service_library.h"
#include "typedefs.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Free any internal Server resources.
 *
 * If a new type of Server instance has been written,
 * this should be called when one of these Server instances
 * is closed.
 */
WHEATIS_SERVICE_API void FreeServerResources (void);


/**
 * Set the path to the where the WheatIS is installed.
 *
 * @param path_s The path to WheatIS. A deep copy will be made of this
 * so this value does not need to stay in scope.
 * @return <code>true></code> if the path was set successfully,
 * <code>false</code> otherwise.
 */
WHEATIS_SERVICE_API bool SetServerRootDirectory (const char * const path_s);


/**
 * Get the path to where WheatIS is installed.
 *
 * @return The path to WheatIS
 */
WHEATIS_SERVICE_API const char *GetServerRootDirectory (void);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SERVICE_CONFIG_H */

