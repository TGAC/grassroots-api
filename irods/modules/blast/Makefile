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
DIR_CUSTOM_LIBS = /usr/users/ga002/tyrrells/Projects/wheatis/irods/util/debug/irods_library
CUSTOM_LIBS = -lirods_library

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




INCLUDE_FLAGS =	-I$(MSIncDir) -I$(buildDir)/server/re/include





#
# Compile and link flags
#
INCLUDES +=	$(INCLUDE_FLAGS) $(LIB_INCLUDES) $(SVR_INCLUDES) -I$(BLAST_INCLUDES_DIR)
CFLAGS_OPTIONS := $(CFLAGS) $(MY_CFLAG)
CFLAGS =	$(CFLAGS_OPTIONS) $(INCLUDES) $(MODULE_CFLAGS)
LIBS += -L$(DIR_CUSTOM_LIBS) $(CUSTOM_LIBS)




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
	@echo $(OBJECTS) $(LIBS)

# List module's includes for inclusion in the server
server_cflags:
	@echo $(INCLUDE_FLAGS)

# Build microservices
microservices:	print_cflags $(OBJECTS)
#microservices:	print_cflags $(C_OBJECTS) $(CXX_OBJECTS)
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

#$(OBJECTS): $(MSObjDir)/%.o: $(MSSrcDir)/%.c $(DEPEND)
#	@echo "Compile $(myModule) module `basename $@`..."
#	@$(CC) -c $(CFLAGS) -o $@ $<

$(OBJECTS): $(MSObjDir)/%.o: $(C_SRCS)
	@echo "Compile c code $(myModule) module `basename $@`..."
	@$(CC) -c $(CFLAGS) -o $@ $<
	
$(DIR_OBJS)/%.o : %.c
	@echo "Compile c code $(myModule) module `basename $@`..."
	$(CC) $(CFLAGS)  -o $@ $<


$(DIR_OBJS)/%.o : %.cpp
	@echo "Compile cpp code $(myModule) module `basename $@`..."
	$(CC) $(CFLAGS)  -o $@ $<
