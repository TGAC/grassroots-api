.DEFAULT_GOAL = all

DIR_OBJS = $(BUILD)

LIBNAME = lib$(NAME).so

CFLAGS 	+= -fPIC -Wall 
LDFLAGS += -shared


CPPFLAGS += -DSHARED_LIBRARY $(INCLUDES)

.SUFFIXES: .c .cpp

.PHONY:	lib clean init

# Build a list of the object files to create, based on the source we find
C_SRCS = $(filter %.c, $(SRCS))
CXX_SRCS = $(filter %.cpp, $(SRCS))

OBJS = $(patsubst %.c, $(DIR_OBJS)/%.o, $(C_SRCS))
OBJS += $(patsubst %.cpp, $(DIR_OBJS)/%.o, $(CXX_SRCS))
  
# Build a list of dependency files
DEPS = $(patsubst %.o, $(DIR_OBJS)/%.d, $(OBJS))

# Pull in dependency info for our objects
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

lib: $(DIR_OBJS)/$(LIBNAME)	
	@echo "Built $(DIR_OBJS)/$(LIBNAME)"

$(DIR_OBJS)/$(LIBNAME): init  $(OBJS) 
	$(CXX) -o $(DIR_OBJS)/$(LIBNAME) $(OBJS) $(STATIC_LIBS) $(LDFLAGS)


install: all
	@echo "Installing $(LIBNAME) to $(DIR_INSTALL)"
	cp $(DIR_OBJS)/$(LIBNAME) $(DIR_INSTALL)/  

init:
	@mkdir -p $(DIR_OBJS)
	@echo "objs: $(OBJS)"
	@echo "c: $(C_SRCS)"	
	@echo "c++: $(CXX_SRCS)"	
	
clean:
	@rm -fr $(BUILD)/

 
# Compile and generate dependency info
# 1. Compile the .c file
# 2. Generate dependency information, explicitly specifying the target name
$(DIR_OBJS)/%.o : %.c
	@echo ">>> c build for $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(MAKEDEPEND) $(basename $@).d -MT $(basename $@).o $(CPPFLAGS) $(CFLAGS) $<  
    	

# Compile and generate dependency info
# 1. Compile the .cpp file
# 2. Generate dependency information, explicitly specifying the target name
$(DIR_OBJS)/%.o : %.cpp
	@echo ">>> c++ build for $@"
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(MAKEDEPEND) $(basename $@).d -MT $(basename $@).o $(CPPFLAGS) $(CFLAGS) $<  
