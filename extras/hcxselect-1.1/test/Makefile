#
# hcxselect - A CSS selector engine for htmlcxx
#

CXXFLAGS += -Wall -g -I$(DIR_HTMLCXX_INC)
CXXFLAGS += $(shell pkg-config --cflags htmlcxx)
INCLUDES += -I../src
LIBS += -L../src -lhcxselect -lstdc++ -L$(DIR_HTMLCXX_LIB) -lhtmlcxx
LIBS += $(shell pkg-config --libs htmlcxx)

all: test

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c -o $@ $<

test: test.o ../src/libhcxselect.a
	$(CXX) $(LDFLAGS) test.o $(LIBS) -o $@

clean:
	$(RM) *.o
