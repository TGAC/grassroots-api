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

# BEGIN IRODS CONFIGURATION
ifneq ($(SHARED_IRODS_HOME),)
DIR_SHARED_IRODS=$(SHARED_IRODS_HOME)
else
DIR_SHARED_IRODS=/usr/local
endif
export DIR_SHARED_IRODS_INC=$(DIR_SHARED_IRODS)/include
export DIR_SHARED_IRODS_LIB=$(DIR_SHARED_IRODS)/lib
# END IRODS CONFIGURATION

# BEGIN OAUTH CONFIGURATION
ifneq ($(OAUTH_HOME),)
DIR_OAUTH=$(OAUTH_HOME)
else
DIR_OAUTH=/usr/local
endif
export DIR_OAUTH_INC=$(DIR_OAUTH)/include
export DIR_OAUTH_LIB=$(DIR_OAUTH)/lib
# END OAUTH CONFIGURATION


# BEGIN UUID CONFIGURATION
ifneq ($(UUID_HOME),)
DIR_UUID=$(UUID_HOME)
else
DIR_UUID=/usr/local
endif
export DIR_UUID_INC=$(DIR_UUID)/include
export DIR_UUID_LIB=$(DIR_UUID)/lib
# END UUID CONFIGURATION


# BEGIN BLAST CONFIGURATION
ifneq ($(BLAST_HOME),)
DIR_BLAST=$(BLAST_HOME)
else
DIR_BLAST=/usr/local
endif
export DIR_BLAST_INC=$(DIR_BLAST)/include/ncbi-tools++
export DIR_BLAST_LIB=$(DIR_BLAST)/lib
# END BLAST CONFIGURATION

# BEGIN LSF_DRMAA CONFIGURATION
ifneq ($(LSF_DRMAA_HOME),)
DIR_LSF_DRMAA=$(LSF_DRMAA_HOME)
else
DIR_LSF_DRMAA=/usr/local
endif
export DIR_LSF_DRMAA_INC=$(DIR_LSF_DRMAA)/include
export DIR_LSF_DRMAA_LIB=$(DIR_LSF_DRMAA)/lib
# END LSF_DRMAA CONFIGURATION


include project.properties


all: 	
	@echo "BUILD = " $(BUILD)
	@echo "DIR_HTMLCXX= = " $(DIR_HTMLCXX)
	@echo "HTMLCXX_HOME = " $(HTMLCXX_HOME)
	$(MAKE) -C util 
	$(MAKE) -C network
	$(MAKE) -C drmaa
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
	$(MAKE) -C services/test_long_runner
					
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
	$(MAKE) -C drmaa install
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
	$(MAKE) -C services/test_long_runner install
	
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
	$(MAKE) -C drmaa clean
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
	$(MAKE) -C services/test_long_runner clean
	
install_init:
	@mkdir -p $(DIR_WHEATIS_INSTALL)
	@mkdir -p $(DIR_WHEATIS_INSTALL)/extras
	@mkdir -p $(DIR_WHEATIS_INSTALL)/lib
	@mkdir -p $(DIR_WHEATIS_INSTALL)/services
	@mkdir -p $(DIR_WHEATIS_INSTALL)/references
	@mkdir -p $(DIR_WHEATIS_INSTALL)/images
	@mkdir -p $(DIR_WHEATIS_INSTALL)/clients
	@mkdir -p $(DIR_WHEATIS_INSTALL)/handlers


install_deps: install_jansson install_htmlcxx install_hcxselect install_oauth install_dropbox_c install_irods_dev install_uuid install_blast
	
install_references: 
	$(COPY) references/* $(DIR_WHEATIS_INSTALL)/references/

install_images:
	$(COPY) images/* $(DIR_WHEATIS_INSTALL)/images

install_jansson:
	cd $(DIR_ROOT)/extras/jansson-2.6; \
	./configure --prefix=$(DIR_JANSSON); \
	make; \
	make install	
	
install_htmlcxx:
	cd $(DIR_ROOT)/extras/htmlcxx-0.84; \
	./configure --prefix=$(DIR_HTMLCXX); \
	make; \
	make install		
	
install_hcxselect:
	@mkdir -p $(DIR_HCXSELECT)
	@mkdir -p $(DIR_HCXSELECT_INC)
	@mkdir -p $(DIR_HCXSELECT_LIB)
	cd $(DIR_ROOT)/extras/hcxselect-1.1; \
	make; \
	make install		

install_dropbox_c:
	cd $(DIR_ROOT)/extras/Dropbox-C; \
	make all LIBRARY_INSTALL_PATH=$(DIR_DROPBOX_LIB) INCLUDE_INSTALL_PATH=$(DIR_DROPBOX_INC) JANSSON_PATH=$(DIR_JANSSON) LIBOAUTH_PATH=$(DIR_OAUTH)

install_oauth:
	cd $(DIR_ROOT)/extras/liboauth-1.0.3; \
	./configure --prefix=$(DIR_OAUTH); \
	make; \
	make install			
	
install_uuid:
	cd $(DIR_ROOT)/extras/libuuid-1.0.3; \
	./configure --prefix=$(DIR_UUID); \
	make; \
	make install			

install_blast:
	cd $(DIR_ROOT)/extras/ncbi-blast-2.2.30+-src/c++; \
	./configure --without-gui --without-internal --with-dll --prefix=$(DIR_BLAST) --with-flat-makefile; \
	make -j 4; \
	make install	
	
install_irods_dev:
	@mkdir -p $(DIR_SHARED_IRODS_INC)
	@mkdir -p $(DIR_SHARED_IRODS_LIB)
	@mkdir -p $(DIR_SHARED_IRODS_INC)/lib/api/include
	$(COPY) $$IRODS_HOME/lib/api/include/* $(DIR_SHARED_IRODS_INC)/lib/api/include
	@mkdir -p $(DIR_SHARED_IRODS_INC)/lib/core/include
	$(COPY) $$IRODS_HOME/lib/core/include/* $(DIR_SHARED_IRODS_INC)/lib/core/include
	@mkdir -p $(DIR_SHARED_IRODS_INC)/lib/md5/include
	$(COPY) $$IRODS_HOME/lib/md5/include/* $(DIR_SHARED_IRODS_INC)/lib/md5/include
	@mkdir -p $(DIR_SHARED_IRODS_INC)/lib/sha1/include
	$(COPY) $$IRODS_HOME/lib/sha1/include/* $(DIR_SHARED_IRODS_INC)/lib/sha1/include
	@mkdir -p $(DIR_SHARED_IRODS_INC)/server/core/include
	$(COPY) $$IRODS_HOME/server/core/include/* $(DIR_SHARED_IRODS_INC)/server/core/include
	@mkdir -p $(DIR_SHARED_IRODS_INC)/server/drivers/include
	$(COPY) $$IRODS_HOME/server/drivers/include/* $(DIR_SHARED_IRODS_INC)/server/drivers/include
	@mkdir -p $(DIR_SHARED_IRODS_INC)/server/icat/include
	$(COPY) $$IRODS_HOME/server/icat/include/* $(DIR_SHARED_IRODS_INC)/server/icat/include
	@mkdir -p $(DIR_SHARED_IRODS_INC)/server/re/include
	$(COPY) $$IRODS_HOME/server/re/include/* $(DIR_SHARED_IRODS_INC)/server/re/include
