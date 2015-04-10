#
# hcxselect - A CSS selector engine for htmlcxx
#

LEX = flex
CXXFLAGS += -Wall -Wno-unused-function -g -fPIC  -DSHARED_LIBRARY -DHCXSELECT_LIBRARY_EXPORTS -I../../../util/include -I$(DIR_HTMLCXX_INC)
CXXFLAGS += $(shell pkg-config --cflags htmlcxx)
LFLAGS += --reentrant 


all: lib 

.cpp.o: lexer.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

lexer.h: lexer.l
	$(LEX) $(LFLAGS) -o $@ $^

libhcxselect.a: hcxselect.o
	$(AR) rcs libhcxselect.a hcxselect.o

libhcxselect.so: hcxselect.o
	$(LD) -shared -o libhcxselect.so hcxselect.o 

lib: lexer.h libhcxselect.a libhcxselect.so


clean:
	$(RM) *.o *.a lexer.h
