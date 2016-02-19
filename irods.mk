

ifeq ($(IRODS_ENABLED),1)

ifeq ($(IRODS_VERSION), 3)

INCLUDES += 	-I$(DIR_SHARED_IRODS_INC)/lib/api/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/core/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/md5/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/sha1/include \
	-I$(DIR_SHARED_IRODS_INC)/server/core/include \
	-I$(DIR_SHARED_IRODS_INC)/server/drivers/include \
	-I$(DIR_SHARED_IRODS_INC)/server/icat/include \
	-I$(DIR_SHARED_IRODS_INC)/server/re/include

LDFLAGS += -L$(DIR_SHARED_IRODS_LIB) $(SHARED_IRODS_LIB_NAMES)

else ifeq ($(IRODS_VERSION), 4)
INCLUDES += 	-I$(DIR_SHARED_IRODS_INC)
IRODS_DEP_LIBS +=  -lboost_thread -lboost_system -lboost_chrono -lboost_timer -lboost_filesystem -lboost_program_options -lboost_regex
IRODS_EXTRA_LIBS = -lirods_client_api_table
LDFLAGS += $(SHARED_IRODS_LIB_NAMES) -lrt
endif

CFLAGS += -DIRODS_ENABLED=1 

endif