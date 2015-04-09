#
# hcxselect - A CSS selector engine for htmlcxx
#

SUBDIRS = src examples test
.PHONY: all $(SUBDIRS)

all: examples test install

install:
	cp src/libhcxselect.so $(DIR_HTMLCXX_LIB)
	cp src/*.h $(DIR_HTMLCXX_INC) 

src:
	@$(MAKE) -wC src $(MFLAGS)

examples: src
	@$(MAKE) -wC examples $(MFLAGS)

test: src
	@$(MAKE) -wC test $(MFLAGS)

clean:
	for d in $(SUBDIRS); do ($(MAKE) -wC $$d clean ); done
