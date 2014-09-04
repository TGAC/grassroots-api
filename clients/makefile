NAME 		= client
DIR_SRC = src
DIR_INCLUDE = include
COMP		= g++
LINK		= ld
MAKEDEPEND 	= g++ -MM -MF


DIR_WHEATIS_UTIL = ../util

DIR_WHEATIS_UTIL_INC = $(DIR_WHEATIS_UTIL)/include
DIR_WHEATIS_UTIL_LIB = $(DIR_WHEATIS_UTIL)/$(BUILD)
WHEATIS_UTIL_LIB_NAME = wheatis_util

DIR_CTHREAD_POOL_DEP = deps/c_thread_pool

BUILD		= debug

VPATH	= \
	$(DIR_SRC) \
	$(DIR_CTHREAD_POOL_DEP)

INCLUDES = \
	-I$(DIR_INCLUDE) \
	-I$(DIR_CTHREAD_POOL_DEP) \
	-I$(DIR_WHEATIS_UTIL_INC) \
	-I$(DIR_WHEATIS_UTIL_INC)/containers
	
SRCS 	= \
	server.c \
	thpool.c

CPPFLAGS += -DUNIX

LDFLAGS += -lpthread -L$(DIR_WHEATIS_UTIL_LIB) -l$(WHEATIS_UTIL_LIB_NAME)

OBJS = $(patsubst %.c, $(BUILD)/%.o, $(SRCS))

.PHONY:	all init clean

all: init $(BUILD)/$(NAME)
	@echo "building $(OBJS)"


init:
	@mkdir -p $(BUILD)

clean:
	rm -f $(BUILD)/*


$(BUILD)/$(NAME) : $(OBJS)
	@echo "building exe $(BUILD)/$(NAME) from $(OBJS)"
	$(COMP) -o $@ $(OBJS) $(STATIC_LIBS) $(LDFLAGS)

$(BUILD)/%.o : %.c
	@echo "building objs  $(OBJS)"
	$(COMP) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -o $@ -c $<



