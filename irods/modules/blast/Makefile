#
# modules/examples/Makefile
#
# Build the iRODS examples module
#

# get the current path
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))

IRODS_INCLUDES_DIR = /usr/include/irods

MODULE_NAME = blast
ifndef buildDir
#buildDir = /var/lib/irods/iRODS
buildDir = $(CURDIR)/../..
endif


include $(buildDir)/config/config.mk
include $(buildDir)/config/platform.mk
include $(buildDir)/config/directories.mk
include $(buildDir)/config/common.mk

#
# Add the blast directories 
#
BLAST_INCLUDES_DIR = /usr/local/include/ncbi-tools++
BLAST_LIBS_DIR = /usr/local/lib


#
# Directories
#
#MSObjDir =	microservices/obj
#MSSrcDir =	microservices/src
#MSIncDir =	microservices/include

MSObjDir =     $(modulesDir)/$(MODULE_NAME)/microservices/obj
MSSrcDir =     $(modulesDir)/$(MODULE_NAME)/microservices/src
MSIncDir =     $(modulesDir)/$(MODULE_NAME)/microservices/include

#
# Source files
#
OBJECTS =	$(MSObjDir)/blast_micro_service.o
INCLUDE_FLAGS =	-I$(MSIncDir) -I$(IRODS_INCLUDES_DIR)





#
# Compile and link flags
#
INCLUDES +=	$(INCLUDE_FLAGS) $(LIB_INCLUDES) $(SVR_INCLUDES) $(BLAST_INCLUDES_DIR)
CFLAGS_OPTIONS := $(CFLAGS) $(MY_CFLAG)
CFLAGS =	$(CFLAGS_OPTIONS) $(INCLUDES) $(MODULE_CFLAGS)





.PHONY: all rules microservices server client clean
.PHONY: server_ldflags client_ldflags server_cflags client_cflags
.PHONY: print_cflags

# Build everything
all:	microservices
	@true

# List module's objects for inclusion in the clients
client_ldflags:
	@true

# List module's includes for inclusion in the clients
client_cflags:
	@true

# List module's objects for inclusion in the server
server_ldflags:
	@echo $(OBJECTS)

# List module's includes for inclusion in the server
server_cflags:
	@echo $(INCLUDE_FLAGS)

# Build microservices
microservices:	print_cflags $(OBJECTS)
	@true

# Build client additions
client:
	@true

# Build server additions
server:
	@true

# Build rules
rules:
	@true

# Clean
clean:
	@echo "Clean $(myModule) module..."
	@rm -f $(OBJECTS)





# Show compile flags
print_cflags:
	@echo "Compile flags:"
	@echo "    $(CFLAGS_OPTIONS)"





#
# Compilation targets
#
$(OBJECTS): $(MSObjDir)/%.o: $(MSSrcDir)/%.c $(DEPEND)
	@echo "Compile $(myModule) module `basename $@`..."
	@$(CC) -c $(CFLAGS) -o $@ $<

