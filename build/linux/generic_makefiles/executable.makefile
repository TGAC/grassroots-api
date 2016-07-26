TARGET_NAME = $(NAME)

include $(DIR_BUILD_ROOT)/generic_makefiles/general.makefile

.PHONY: exe

exe: $(DIR_OBJS)/$(TARGET_NAME)	
	@echo "Built $(DIR_OBJS)/$(TARGET_NAME)"

