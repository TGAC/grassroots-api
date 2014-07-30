.DEFAULT_GOAL = all

DIR_OBJS = $(BUILD)/$(NAME)

LIBNAME = lib$(NAME).so

CFLAGS 	+= -fPIC -Wall 
LDFLAGS += -shared


CPPFLAGS += -DSHARED_LIBRARY $(INCLUDES)

.SUFFIXES: .c

.PHONY:	all clean init

# Build a list of the object files to create, based on the .c we find
OTMP = $(patsubst %.c, %.o, $(SRCS))
 
# Build the final list of objects
OBJS = $(patsubst %, $(DIR_OBJS)/%, $(OTMP))
 
# Build a list of dependency files
DEPS = $(patsubst %.o, $(DIR_OBJS)/%.d, $(OTMP))

# Pull in dependency info for our objects
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

all: $(DIR_OBJS)/$(LIBNAME)	

$(DIR_OBJS)/$(LIBNAME): init $(OBJS) 
	$(COMP) -o $(DIR_OBJS)/$(LIBNAME) $(OBJS) $(LDFLAGS)


install: all
	@echo "Installing $(LIBNAME) to $(DIR_INSTALL)"
	cp $(DIR_OBJS)/$(LIBNAME) $(DIR_INSTALL)/  

init:
	@mkdir -p $(DIR_OBJS)

clean:
	@rm -fr $(BUILD)/$(NAME)	

 
# Compile and generate dependency info
# 1. Compile the .c file
# 2. Generate dependency information, explicitly specifying the target name
$(DIR_OBJS)/%.o : %.c
	$(COMP) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(MAKEDEPEND) $(basename $@).d -MT $(basename $@).o $(CPPFLAGS) $<  
    	
