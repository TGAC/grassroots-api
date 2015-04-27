NAME 		= blast_service
DIR_SRC = src
DIR_INCLUDE = include
CC		= g++
CXX		= g++
MAKEDEPEND 	= gcc -MM -MF



ifeq ($(DIR_ROOT),)
export DIR_ROOT = $(realpath $(../..))
endif

ifneq ($(PROPERTIES_SET),1)
include $(DIR_ROOT)/project.properties
endif

export DIR_INSTALL=$(DIR_WHEATIS_INSTALL)/services

BUILD		= debug


VPATH	= \
	$(DIR_SRC) \
	

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers \
	-I$(DIR_WHEATIS_UTIL_INC)/io \
	-I$(DIR_WHEATIS_HANDLER_INC) \
	-I$(DIR_WHEATIS_NETWORK_INC) \
	-I$(DIR_WHEATIS_SERVICES_INC) \
	-I$(DIR_SHARED_IRODS_INC)/lib/api/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/core/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/md5/include \
	-I$(DIR_SHARED_IRODS_INC)/lib/sha1/include \
	-I$(DIR_SHARED_IRODS_INC)/server/core/include \
	-I$(DIR_SHARED_IRODS_INC)/server/drivers/include \
	-I$(DIR_SHARED_IRODS_INC)/server/icat/include \
	-I$(DIR_SHARED_IRODS_INC)/server/re/include \
	-I$(DIR_JANSSON_INC) \
	-I$(DIR_UUID_INC) \
	-I$(DIR_BLAST_INC)
	
SRCS 	= \
	blast_service.c \
	blast_tool.cpp \
	blast_tool_set.cpp \
	external_blast_tool.cpp \
	system_blast_tool.cpp \
	temp_file.cpp
#	blast_search.cpp 
	


	

CPPFLAGS += -DBLAST_LIBRARY_EXPORTS 

LDFLAGS += -L$(DIR_JANSSON_LIB) -ljansson -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME) \
	-L$(DIR_WHEATIS_SERVICE_LIB) -l$(WHEATIS_SERVICE_LIB_NAME) \
	-L$(DIR_WHEATIS_NETWORK_LIB) -l$(WHEATIS_NETWORK_LIB_NAME)


ifneq ($(DRMAA_ENABLED),)
SRCS += drma_blast_tool.cpp
LDFLAGS += -L$(DIR_WHEATIS_DRMAA_LIB) -l$(WHEATIS_DRMAA_LIB_NAME)
endif


BLAST_SEARCH_LDFLAGS = \
	-L$(DIR_BLAST_LIB) \
	-lblast_app_util  \
	-lblastinput-static  \
	-lncbi_xloader_blastdb-static  \
	-lncbi_xloader_blastdb_rmt-static  \
	-lxblastformat-static  \
	-lalign_format-static  \
	-ltaxon1-static  \
	-lblastdb_format-static  \
	-lgene_info-static  \
	-lxalnmgr-static  \
	-lblastxml-static  \
	-lxcgi-static  \
	-lxhtml-static  \
	-lxblast-static  \
	-lxalgoblastdbindex-static  \
	-lcomposition_adjustment-static  \
	-lxalgodustmask-static  \
	-lxalgowinmask-static  \
	-lseqmasks_io-static  \
	-lseqdb-static  \
	-lblast_services-static  \
	-lxobjutil-static  \
	-lxobjread-static  \
	-lvariation-static  \
	-lcreaders-static  \
	-lsubmit-static  \
	-lxnetblastcli-static  \
	-lxnetblast-static  \
	-lblastdb-static  \
	-lscoremat-static  \
	-ltables-static  \
	-lxalnmgr-static  \
	-lncbi_xloader_genbank-static  \
	-lncbi_xreader_id1-static  \
	-lncbi_xreader_id2-static  \
	-lncbi_xreader_cache-static  \
	-lxconnext-static  \
	-lxconnect-static  \
	-ldbapi_driver-static  \
	-lncbi_xreader-static  \
	-lxconnext-static  \
	-lxconnect-static  \
	-lid1-static  \
	-lid2-static  \
	-lseqsplit-static  \
	-lxcompress-static  \
	-lxobjmgr-static  \
	-lgenome_collection-static  \
	-lseqedit-static  \
	-lseqset-static  \
	-lseq-static  \
	-lseqcode-static  \
	-lsequtil-static  \
	-lpub-static  \
	-lmedline-static  \
	-lbiblio-static  \
	-lgeneral-static  \
	-lxser-static  \
	-lxutil-static  \
	-lxncbi-static  \
	-lz   \
	-lbz2    \
	-ldl  \
	-lnsl  \
	-lrt  \
	-lm   \
	-lpthread 	



include $(DIR_ROOT)/generic_makefiles/shared_library.makefile

