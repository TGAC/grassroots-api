#
# hcxselect - A CSS selector engine for htmlcxx
#

CXXFLAGS += -Wall -g
CXXFLAGS += $(shell pkg-config --cflags htmlcxx)
INCLUDES += -I../src -I$(DIR_HTMLCXX)/include -I$(DIR_WHEATIS_UTIL_INC)
LIBS += -L../src -L$(DIR_HTMLCXX)/lib -lhcxselect -lhtmlcxx -lstdc++
LIBS += $(shell pkg-config --libs htmlcxx)

all: test

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o $@ $<

test: test.o ../src/libhcxselect.a
	$(CXX) $(LDFLAGS) test.o $(LIBS) -o $@

clean:
	$(RM) *.o
