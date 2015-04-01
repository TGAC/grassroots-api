#
# hcxselect - A CSS selector engine for htmlcxx
#

SUBDIRS = src examples test
.PHONY: all $(SUBDIRS)

all: examples test install

install:
	mkdir $(PREFIX)
	mkdir $(PREFIX)/include
	mkdir $(PREFIX)/lib
	cp src/libhcxselect.so $(PREFIX)/lib
	cp src/*.h $(PREFIX)/include 

src:
	@$(MAKE) -wC src $(MFLAGS)

examples: src
	@$(MAKE) -wC examples $(MFLAGS)

test: src
	@$(MAKE) -wC test $(MFLAGS)

clean:
	for d in $(SUBDIRS); do ($(MAKE) -wC $$d clean ); done
