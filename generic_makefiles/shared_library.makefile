
TARGET_NAME = lib$(NAME).so

CFLAGS 	+= -fPIC
LDFLAGS += -shared -rdynamic

CPPFLAGS += -DSHARED_LIBRARY 

include $(DIR_ROOT)/generic_makefiles/general.makefile


.PHONY:	all 

all: $(DIR_OBJS)/$(TARGET_NAME)	
	@echo "current dur $(CURRENT_DIR)"
	@echo "Built $(DIR_OBJS)/$(TARGET_NAME)"

