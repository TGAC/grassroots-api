#
# modules/examples/Makefile
#
# Build the iRODS examples module
#

myModule=blast
ifndef buildDir
buildDir = ../../
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
# Add our custom libraries
#
CUSTOM_LIBS_DIR = /usr/users/ga002/tyrrells/Projects/wheatis/irods/util/debug/irods_library
CUSTOM_LIBS = -lirods_library
CUSTOM_INCLUDES_DIR = /usr/users/ga002/tyrrells/Projects/wheatis/irods/util/include

#
# Directories
#
MSObjDir =	$(modulesDir)/$(myModule)/microservices/obj
MSSrcDir =	$(modulesDir)/$(myModule)/microservices/src
MSIncDir =	$(modulesDir)/$(myModule)/microservices/include


# Build the final list of objects
C_SRCS = \
	$(MSSrcDir)/blast_micro_service.c

CXX_SRCS = \
	$(MSSrcDir)/blast_tool.cpp

# Build a list of the object files to create, based on the .c we find
OC_TMP = $(patsubst %.c, %.o, $(notdir $(C_SRCS)))
 
# Build the final list of objects
C_OBJECTS = $(patsubst %, $(MSObjDir)/%, $(OC_TMP))

# Build a list of the object files to create, based on the .cpp we find
OCXX_TMP = $(patsubst %.cpp, %.o, $(notdir $(CXX_SRCS)))
 
# Build the final list of objects
CXX_OBJECTS = $(patsubst %, $(MSObjDir)/%, $(OCXX_TMP))


INCLUDE_FLAGS =	-I$(MSIncDir) -I$(buildDir)/server/re/include


#
# Compile and link flags
#
INCLUDES +=	$(INCLUDE_FLAGS) $(LIB_INCLUDES) $(SVR_INCLUDES) -I$(BLAST_INCLUDES_DIR) -I$(CUSTOM_INCLUDES_DIR)
CFLAGS_OPTIONS := $(CFLAGS) $(MY_CFLAG)
CFLAGS =	$(CFLAGS_OPTIONS) $(INCLUDES) $(MODULE_CFLAGS)
LIBS += -L$(DIR_CUSTOM_LIBS) $(CUSTOM_LIBS)


.PHONY: all rules microservices server client clean
.PHONY: server_ldflags client_ldflags server_cflags client_cflags
.PHONY: print_cflags
.PHONY: build copy_to_irods

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
	@echo $(C_OBJECTS) $(CXX_OBJECTS) $(LIBS)

# List module's includes for inclusion in the server
server_cflags:
	@echo $(INCLUDE_FLAGS)

# Build microservices
#microservices:	print_cflags $(OBJECTS)
microservices:	print_cflags $(C_OBJECTS) $(CXX_OBJECTS)
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
	@rm -f $(C_OBJECTS) $(CXX_OBJECTS)

# Copy the microservice files into the irods file structure and build the module
build:	copy_to_irods microservices
	@true

# Copy the microservice files into the irods file structure
copy_to_irods:
	@echo "Copying $(myModule) module to $(buildDir)/modules"
	@echo "cp -r -u -f ../$(myModule) $(buildDir)/modules/"
	@cp -r -u -f ../$(myModule) $(buildDir)/modules/
	@true


# Show compile flags
print_cflags:
	@echo "Compile flags:"
	@echo "    $(CFLAGS_OPTIONS)"
#	@echo "c:"
#	@echo "    $(C_SRCS)"
#	@echo "    $(C_OBJECTS)"
#	@echo "c++:"
#	@echo "    $(CXX_SRCS)"
#	@echo "    $(CXX_OBJECTS)"

#
# Compilation targets
#
$(C_OBJECTS): $(C_SRCS)
	@echo "Compile $(myModule) module `basename $@`..."
	@$(CC) -c $(CFLAGS) -o $@ $<

$(CXX_OBJECTS): $(CXX_SRCS)
	@echo "Compile $(myModule) module `basename $@`..."
	@$(CC) -c $(CFLAGS) -o $@ $<
