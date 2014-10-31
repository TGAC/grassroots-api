server.o: src/server.c include/server.h \
 include/wheatis_service_manager_library.h ../../util/include/library.h \
 ../../util/include/containers/linked_list.h \
 ../../util/include/typedefs.h ../../util/include/wheatis_util_library.h \
 ../../util/include/library.h ../../network/include/json_tools.h \
 ../../network/include/network_library.h ../../util/include/operation.h \
 ../../util/include/json_util.h ../../util/include/wheatis_util_library.h \
 ../../services/lib/include/service.h \
 ../../services/lib/include/wheatis_service_library.h \
 ../../services/lib/include/parameter_set.h \
 ../../services/lib/include/parameter.h ../../util/include/tags.h \
 ../../util/include/typedefs.h ../../services/lib/include/resource.h \
 ../../handlers/lib/include/handler.h \
 ../../handlers/lib/include/wheatis_handler_library.h \
 ../../util/include/plugin.h ../../util/include/memory_allocations.h \
 ../../services/lib/include/resource.h \
 ../../services/lib/include/parameter.h \
 ../../util/include/io/filesystem_utils.h \
 ../../services/lib/include/user_details.h \
 ../../services/lib/include/tag_item.h ../../util/include/time_util.h \
 ../../irods/lib/include/query.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rcConnect.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsDef.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsVersion.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsError.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsErrorTable.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsLog.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/stringOpr.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsType.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsUser.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/getRodsEnv.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/objInfo.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/api/include/dataObjInpOut.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsDef.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/irodsGuiProgressCallback.h \
 /usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsGenQuery.h \
 ../../irods/lib/include/irods_util_library.h \
 ../../irods/lib/include/connect.h ../../irods/lib/include/user.h \
 ../../irods/lib/include/query.h \
 ../../handlers/lib/include/handler_utils.h \
 ../../handlers/lib/include/handler.h \
 ../../services/lib/include/parameter_set.h
src/server.c:
include/server.h:
include/wheatis_service_manager_library.h:
../../util/include/library.h:
../../util/include/containers/linked_list.h:
../../util/include/typedefs.h:
../../util/include/wheatis_util_library.h:
../../util/include/library.h:
../../network/include/json_tools.h:
../../network/include/network_library.h:
../../util/include/operation.h:
../../util/include/json_util.h:
../../util/include/wheatis_util_library.h:
../../services/lib/include/service.h:
../../services/lib/include/wheatis_service_library.h:
../../services/lib/include/parameter_set.h:
../../services/lib/include/parameter.h:
../../util/include/tags.h:
../../util/include/typedefs.h:
../../services/lib/include/resource.h:
../../handlers/lib/include/handler.h:
../../handlers/lib/include/wheatis_handler_library.h:
../../util/include/plugin.h:
../../util/include/memory_allocations.h:
../../services/lib/include/resource.h:
../../services/lib/include/parameter.h:
../../util/include/io/filesystem_utils.h:
../../services/lib/include/user_details.h:
../../services/lib/include/tag_item.h:
../../util/include/time_util.h:
../../irods/lib/include/query.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rcConnect.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsDef.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsVersion.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsError.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsErrorTable.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsLog.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/stringOpr.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsType.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsUser.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/getRodsEnv.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/objInfo.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/api/include/dataObjInpOut.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsDef.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/irodsGuiProgressCallback.h:
/usr/users/ga002/tyrrells/Projects/iRODS-3.3.1/lib/core/include/rodsGenQuery.h:
../../irods/lib/include/irods_util_library.h:
../../irods/lib/include/connect.h:
../../irods/lib/include/user.h:
../../irods/lib/include/query.h:
../../handlers/lib/include/handler_utils.h:
../../handlers/lib/include/handler.h:
../../services/lib/include/parameter_set.h:
