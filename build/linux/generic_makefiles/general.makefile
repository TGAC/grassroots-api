.DEFAULT_GOAL = all

DIR_OBJS = $(BUILD)


CFLAGS 	+=  -Wall -Wshadow -Wextra 

CPPFLAGS += $(INCLUDES) -DUNIX

.SUFFIXES: .c .cpp

.PHONY: clean init

# Build a list of the object files to create, based on the source we find
C_SRCS = $(filter %.c, $(SRCS))
CXX_SRCS = $(filter %.cpp, $(SRCS))

OBJS = $(patsubst %.c, $(DIR_OBJS)/%.o, $(C_SRCS))
OBJS += $(patsubst %.cpp, $(DIR_OBJS)/%.o, $(CXX_SRCS))
  
# Pull in dependency info for our objects
ifneq ($(MAKECMDGOALS), clean)
-include $(OBJS:.o=.d)
endif


ifeq ($(BUILD),release)
	CFLAGS 	+= -O3 -s
 	LDFLAGS += -s
else
#	CFLAGS 	+= -g -O0
	CFLAGS 	+= -g -pg
	LDFLAGS += -pg -g
	CPPFLAGS += -D_DEBUG
endif



$(DIR_OBJS)/$(TARGET_NAME): init  $(OBJS) 
	$(CXX) -o $(DIR_OBJS)/$(TARGET_NAME) $(OBJS) $(STATIC_LIBS) $(LDFLAGS)


install: all
	@echo "ROOT DIR $(DIR_ROOT)"
	@echo "THIS DIR $(THIS_DIR)"
	@echo "Installing $(TARGET_NAME) to $(DIR_INSTALL)"
	cp $(DIR_OBJS)/$(TARGET_NAME) $(DIR_INSTALL)/  

init:
	@mkdir -p $(DIR_OBJS)
	@echo "-----------------------------------------------" 
	@echo "objs: $(OBJS)"
	@echo "c: $(C_SRCS)"	
	@echo "c++: $(CXX_SRCS)"	
	@echo "VPATH: $(VPATH)" 
	@echo "IRODS: =$(DIR_IRODS)=" 
	@echo "-----------------------------------------------" 
	
clean:
	@rm -fr $(BUILD)/

 
# Compile and generate dependency info
# 1. Compile the .c file
# 2. Generate dependency information, explicitly specifying the target name
$(DIR_OBJS)/%.o : %.c
	@echo ">>> c build for $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(MAKEDEPEND) $(basename $@).d -MT $(basename $@).o $(CPPFLAGS) $(CFLAGS) $<  
	@mv -f $(basename $@).d $(basename $@).d.tmp
	@sed -e 's|.*:|$*.o:|' < $(basename $@).d.tmp > $(basename $@).d
	@sed -e 's/.*://' -e 's/\\$$//' < $(basename $@).d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $(basename $@).d
	@rm -f $(basename $@).d.tmp   	

# Compile and generate dependency info
# 1. Compile the .cpp file
# 2. Generate dependency information, explicitly specifying the target name
$(DIR_OBJS)/%.o : %.cpp 
	@echo ">>> c++ build for $@"
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(MAKEDEPEND) $(basename $@).d -MT $(basename $@).o $(CPPFLAGS) $(CFLAGS) $<  
	@mv -f $(basename $@).d $(basename $@).d.tmp
	@sed -e 's|.*:|$*.o:|' < $(basename $@).d.tmp > $(basename $@).d
	@sed -e 's/.*://' -e 's/\\$$//' < $(basename $@).d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $(basename $@).d
	@rm -f $(basename $@).d.tmp  
