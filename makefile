COPY	= cp


export DIR_WHEATIS_INSTALL = /opt/wheatis
export DIR_ROOT = $(realpath .)


include dependencies.properties


# BEGIN JANSSON CONFIGURATION
ifneq ($(JANSSON_HOME),)
DIR_JANSSON=$(JANSSON_HOME)
else
DIR_JANSSON=/usr/local
endif
export DIR_JANSSON_INC=$(DIR_JANSSON)/include
export DIR_JANSSON_LIB=$(DIR_JANSSON)/lib
# END JANSSON CONFIGURATION

# BEGIN DROPBOX CONFIGURATION
ifneq ($(DROPBOX_HOME),)
DIR_DROPBOX=$(DROPBOX_HOME)
else
DIR_DROPBOX=/usr/local
endif
export DIR_DROPBOX_INC=$(DIR_DROPBOX)/include
export DIR_DROPBOX_LIB=$(DIR_DROPBOX)/lib
# END DROPBOX CONFIGURATION

# BEGIN HCXSELECT CONFIGURATION
ifneq ($(HCXSELECT_HOME),)
DIR_HCXSELECT=$(HCXSELECT_HOME)
else
DIR_HCXSELECT=/usr/local
endif
export DIR_HCXSELECT_INC=$(DIR_HCXSELECT)/include
export DIR_HCXSELECT_LIB=$(DIR_HCXSELECT)/lib
# END HCXSELECT CONFIGURATION

# BEGIN HTMLCXX CONFIGURATION
ifneq ($(HTMLCXX_HOME),)
DIR_HTMLCXX=$(HTMLCXX_HOME)
else
DIR_HTMLCXX=/usr/local
endif
export DIR_HTMLCXX_INC=$(DIR_HTMLCXX)/include
export DIR_HTMLCXX_LIB=$(DIR_HTMLCXX)/lib
# END HTMLCXX CONFIGURATION

# BEGIN HTMLCXX CONFIGURATION
ifneq ($(SHARED_IRODS_HOME),)
DIR_SHARED_IRODS=$(SHARED_IRODS_HOME)
else
DIR_SHARED_IRODS=/usr/local
endif
export DIR_SHARED_IRODS_INC=$(DIR_SHARED_IRODS)
export DIR_SHARED_IRODS_LIB=$(DIR_SHARED_IRODS)/lib
# END HTMLCXX CONFIGURATION

include project.properties


all: 	
	@echo "BUILD = " $(BUILD)
	@echo "DIR_HTMLCXX= = " $(DIR_HTMLCXX)
	@echo "HTMLCXX_HOME = " $(HTMLCXX_HOME)
	$(MAKE) -C util 
	$(MAKE) -C network
	$(MAKE) -C irods/lib
	$(MAKE) -C handlers/lib
	$(MAKE) -C services/lib
	$(MAKE) -C handlers/file
	$(MAKE) -C handlers/irods
	$(MAKE) -C handlers/dropbox
	$(MAKE) -C server/lib
	$(MAKE) -C server/standalone
	$(MAKE) -C server/httpd/mod_wheatis
	$(MAKE) -C clients/standalone
#	$(MAKE) -C clients/web-server-client	
	$(MAKE) -C services/blast
	$(MAKE) -C services/compress
	$(MAKE) -C services/web
	$(MAKE) -C services/web_search
	$(MAKE) -C services/irods_search
	$(MAKE) -C services/ensembl_rest
	$(MAKE) -C services/tgac_elastic_search
					
install: install_init install_references install_images
	$(MAKE) -C util install
	$(MAKE) -C network install
	$(MAKE) -C irods/lib install
	$(MAKE) -C handlers/lib install
	$(MAKE) -C services/lib install
	$(MAKE) -C handlers/file install
	$(MAKE) -C handlers/irods install
	$(MAKE) -C handlers/dropbox install
	$(MAKE) -C server/lib install 
	$(MAKE) -C server/standalone install 
	$(MAKE) -C server/httpd/mod_wheatis install
	$(MAKE) -C clients/standalone install
#	$(MAKE) -C clients/web-server-client install	
	$(MAKE) -C services/blast install
	$(MAKE) -C services/compress install
	$(MAKE) -C services/web install
	$(MAKE) -C services/web_search install
	$(MAKE) -C services/irods_search install
	$(MAKE) -C services/ensembl_rest install
	$(MAKE) -C services/tgac_elastic_search install
	
clean: 
	$(MAKE) -C util clean
	$(MAKE) -C network clean
	$(MAKE) -C irods/lib clean
	$(MAKE) -C handlers/lib clean
	$(MAKE) -C services/lib clean
	$(MAKE) -C handlers/file clean
	$(MAKE) -C handlers/irods clean
	$(MAKE) -C handlers/dropbox clean
	$(MAKE) -C server/lib clean 
	$(MAKE) -C server/standalone clean 
	$(MAKE) -C server/httpd/mod_wheatis clean
	$(MAKE) -C clients/standalone clean
#	$(MAKE) -C clients/web-server-client clean	
	$(MAKE) -C services/blast clean
	$(MAKE) -C services/compress clean
	$(MAKE) -C services/web clean
	$(MAKE) -C services/web_search clean	
	$(MAKE) -C services/irods_search clean
	$(MAKE) -C services/ensembl_rest clean
	$(MAKE) -C services/tgac_elastic_search clean
	
install_init:
	@mkdir -p $(DIR_WHEATIS_INSTALL)
	@mkdir -p $(DIR_WHEATIS_INSTALL)/extras
	@mkdir -p $(DIR_WHEATIS_INSTALL)/lib
	@mkdir -p $(DIR_WHEATIS_INSTALL)/services
	@mkdir -p $(DIR_WHEATIS_INSTALL)/references
	@mkdir -p $(DIR_WHEATIS_INSTALL)/images
	@mkdir -p $(DIR_WHEATIS_INSTALL)/clients
	@mkdir -p $(DIR_WHEATIS_INSTALL)/handlers


install_deps:
	
install_references:
	$(COPY) references/* $(WHEATIS_INSTALL)/references/

install_images:
	$(COPY) images/* $(WHEATIS_INSTALL)/images

install_jansson:
	cd $(DIR_ROOT)/extras/jansson
	./configure --prefix=$(DIR_JANSSON)
	make
	make install	
	
