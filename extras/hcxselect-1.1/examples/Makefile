#
# hcxselect - A CSS selector engine for htmlcxx
#

CXXFLAGS += -Wall -g
CXXFLAGS += $(shell pkg-config --cflags htmlcxx)
INCLUDES += -I../src -I$(DIR_HTMLCXX)/include -I$(DIR_WHEATIS_UTIL_INC)
LIBS += -L../src -lhcxselect -L$(DIR_HTMLCXX)/lib -lhtmlcxx -lstdc++
LIBS += $(shell pkg-config --libs htmlcxx)

all: select

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o $@ $<

select: select.o ../src/libhcxselect.a
	$(CXX) $(LDFLAGS) select.o $(LIBS) -o $@

clean:
	$(RM) *.o
