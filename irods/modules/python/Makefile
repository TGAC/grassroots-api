#
# modules/python/Makefile
#
# Builds the iRODS python module
#

myModule=python
ifndef buildDir
buildDir = $(CURDIR)/../..
endif

include $(buildDir)/config/config.mk
include $(buildDir)/config/platform.mk
include $(buildDir)/config/directories.mk
include $(buildDir)/config/common.mk


#
# Add the python directories 
#
PYTHON_INCLUDES_DIR = /usr/include/python2.7/
PYTHON_LIBS_DIR = /usr/lib
PYTHON_LIB_NAME = python2.7
PYTHON_LDFLAGS = -lpthread -ldl -lutil -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions


#
# Directories
#
MSObjDir =	$(modulesDir)/$(myModule)/microservices/obj
MSSrcDir =	$(modulesDir)/$(myModule)/microservices/src
MSIncDir =	$(modulesDir)/$(myModule)/microservices/include


#
# Source files
#

OBJECTS =	$(MSObjDir)/python_micro_services.o $(MSObjDir)/python_util.o 
INCLUDE_FLAGS =	-I$(MSIncDir) -I$(buildDir)/server/re/include -I$(PYTHON_INCLUDES_DIR)


#
# Compile and link flags
#
INCLUDES +=	$(INCLUDE_FLAGS) $(LIB_INCLUDES) $(SVR_INCLUDES)
CFLAGS_OPTIONS := -DRODS_SERVER $(CFLAGS) $(MY_CFLAG)
CFLAGS =	$(CFLAGS_OPTIONS) $(INCLUDES) $(MODULE_CFLAGS)
LIBS = -L$(PYTHON_LIBS_DIR) -l$(PYTHON_LIB_NAME)




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
	@echo "Clean $(OBJECTS) in $(myModule) module..."
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


