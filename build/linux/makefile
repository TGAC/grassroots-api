COPY	= cp 
DELETE = rm

export DIR_BUILD_ROOT = $(realpath .)
export DIR_PKG_ROOT = $(realpath ../..)
export DIR_SRC_ROOT = $(DIR_PKG_ROOT)/src

export DIR_SHARED_SRC_ROOT = $(DIR_PKG_ROOT)/shared/src
export DIR_SERVER_SRC_ROOT = $(DIR_PKG_ROOT)/server/src
export DIR_CLIENTS_SRC_ROOT = $(DIR_PKG_ROOT)/clients/

export DIR_SHARED_BUILD_ROOT = $(DIR_BUILD_ROOT)/shared
export DIR_SERVER_BUILD_ROOT = $(DIR_BUILD_ROOT)/server
export DIR_CLIENTS_BUILD_ROOT = $(DIR_BUILD_ROOT)/clients

export DIR_HANDLER_ROOT_BUILD = $(DIR_SHARED_BUILD_ROOT)/handlers
export DIR_HANDLER_ROOT_SRC = $(DIR_SHARED_SRC_ROOT)/handlers

export DIR_SERVICES_ROOT_BUILD = $(DIR_SERVER_BUILD_ROOT)/services
export DIR_SERVICES_ROOT_SRC = $(DIR_SERVER_SRC_ROOT)/services


#export DIR_SERVER_ROOT_BUILD = $(DIR_SERVER_BUILD_ROOT)/server
#export DIR_SERVER_ROOT_SRC = $(DIR_SERVER_SRC_ROOT)/server

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
export DIR_SHARED_IRODS_INC=$(SHARED_IRODS_HOME)/include
export DIR_SHARED_IRODS_LIB=$(SHARED_IRODS_HOME)/lib
export SHARED_IRODS_LIB_NAMES= -lirods
else
export DIR_SHARED_IRODS_INC=/usr/include/irods
export DIR_SHARED_IRODS_LIB=/usr/lib
export SHARED_IRODS_LIB_NAMES= -lirods_client -lirods_client_plugins
endif

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


# BEGIN SLURM_DRMAA CONFIGURATION
ifneq ($(SLURM_DRMAA_HOME),)
DIR_SLURM_DRMAA=$(SLURM_DRMAA_HOME)
else
DIR_SLURM_DRMAA=/usr/local
endif
export DIR_SLURM_DRMAA_INC=$(DIR_SLURM_DRMAA)/include
export DIR_SLURM_DRMAA_LIB=$(DIR_SLURM_DRMAA)/lib
# END LSF_DRMAA CONFIGURATION


# BEGIN MONGODB CONFIGURATION
ifneq ($(MONGODB_HOME),)
DIR_MONGODB=$(MONGODB_HOME)
else
DIR_MONGODB=/usr/local
endif
export DIR_MONGODB_INC=$(DIR_MONGODB)/include/libmongoc-1.0
export DIR_MONGODB_LIB=$(DIR_MONGODB)/lib
# END MONGODB CONFIGURATION


# BEGIN BSON CONFIGURATION
ifneq ($(BSON_HOME),)
DIR_BSON=$(BSON_HOME)
else
DIR_BSON=/usr/local
endif
export DIR_BSON_INC=$(DIR_BSON)/include/libbson-1.0
export DIR_BSON_LIB=$(DIR_BSON)/lib
# END BSON CONFIGURATION


# BEGIN HTSLIB CONFIGURATION
ifneq ($(HTSLIB_HOME),)
DIR_HTSLIB=$(HTSLIB_HOME)
else
DIR_HTSLIB=/usr/local
endif
export DIR_HTSLIB_INC=$(DIR_HTSLIB)/include
export DIR_HTSLIB_LIB=$(DIR_HTSLIB)/lib
# END HTSLIB CONFIGURATION

# BEGIN SAMTOOLS CONFIGURATION
ifneq ($(SAMTOOLS_HOME),)
DIR_SAMTOOLS=$(SAMTOOLS_HOME)
else
DIR_SAMTOOLS=/usr/local
endif
export DIR_SAMTOOLS_INC=$(DIR_SAMTOOLS)/include
export DIR_SAMTOOLS_LIB=$(DIR_SAMTOOLS)/lib
# END SAMTOOLS CONFIGURATION



include project.properties
-include drmaa.properties

lib_util:
	$(MAKE) -C $(DIR_GRASSROOTS_UTIL_BUILD)
 
lib_network:
	$(MAKE) -C $(DIR_GRASSROOTS_NETWORK_BUILD)

lib_parameters:
	$(MAKE) -C $(DIR_GRASSROOTS_PARAMS_BUILD)


#############################
######## BEGIN IRODS ########
#############################

install_irods_stuff: install_lib_irods install_handler_irods install_service_irods_search
build_irods_stuff: lib_irods handler_irods service_irods_search

ifeq ($(IRODS_ENABLED),1)

lib_irods:
	$(MAKE) -C $(DIR_GRASSROOTS_IRODS_BUILD)

install_lib_irods:	lib_irods
	$(MAKE) -C $(DIR_GRASSROOTS_IRODS_BUILD) install

handler_irods:
	$(MAKE) -C HANDLER/irods

install_handler_irods: handler_irods
	$(MAKE) -C HANDLER/irods install

service_irods_search: 
	$(MAKE) -C services/irods_search

install_service_irods_search: service_irods_search
	$(MAKE) -C services/irods_search install

else

lib_irods:	
	echo "**** Skip building lib_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

install_lib_irods:	
	echo "**** Skip running install_lib_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

handler_irods:
	echo "**** Skip building handler_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

install_handler_irods:
	echo "**** Skip running install_handler_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

service_irods_search:
	echo "**** Skip building service_irods_search as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

install_service_irods_search:
	echo "**** Skip running install_service_irods_search as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

endif
#############################
######### END IRODS #########
#############################


	
lib_handler:
	$(MAKE) -C $(DIR_GRASSROOTS_HANDLER_BUILD)
	
lib_services:
	$(MAKE) -C $(DIR_GRASSROOTS_SERVICES_BUILD)



#ifdef build_drmaa_flag
drmaa: lib_util lib_network lib_parameters lib_irods lib_handler lib_services
	@echo "DRMAA_IMPLEMENTATION_NAME $(DRMAA_IMPLEMENTATION_NAME)"
	@echo "DRMAA_IMPLEMENTATION_LIB_NAME $(DRMAA_IMPLEMENTATION_LIB_NAME)"
	@echo "DIR_DRMAA_IMPLEMENTATION_LIB $(DIR_DRMAA_IMPLEMENTATION_LIB)"
	@echo "DIR_DRMAA_IMPLEMENTATION_INC $(DIR_DRMAA_IMPLEMENTATION_INC)"
#endif


# SLURM DRMAA
ifeq ($(SLURM_DRMAA_ENABLED),1)	
export SLURM_DRMAA_ENABLED=1
all: drmaa_slurm

install: install_drmaa_slurm

drmaa_slurm: drmaa
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/drmaa
	
install_drmaa_slurm: drmaa_slurm
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/drmaa install

# LSF DRMAA
else ifeq ($(LSF_DRMAA_ENABLED),1)
export LSF_DRMAA_ENABLED=1

all: drmaa_lsf

install: install_drmaa_lsf

drmaa_lsf: drmaa
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/drmaa

install_drmaa_lsf: drmaa_lsf
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/drmaa install 
endif


info:
	@echo "installing grassroots to $(DIR_GRASSROOTS_INSTALL)"
	@echo "installing apache module to $(DIR_APACHE)/modules"
	@echo "APXS=$(APXS)"	
	@echo "GRASSROOTS_VERSION=$(GRASSROOTS_VERSION)"


valgrind: 
	$(MAKE) all CFLAGS=-DUSING_VALGRIND

all: lib_util lib_network lib_parameters lib_irods lib_handler lib_services handler_irods service_irods_search
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/file 
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/irods 
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/dropbox 
	$(MAKE) -C $(DIR_CLIENTS_BUILD_ROOT)/lib  
	$(MAKE) -C $(DIR_CLIENTS_BUILD_ROOT)/standalone  
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/lib
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/httpd 
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/standalone 
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/mongodb 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/blast 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/compress 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/web 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/json_search 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/web_search 	
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/ensembl_rest 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/tgac_elastic_search 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/test_long_runner 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/pathogenomics 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/samtools 



							
install: install_init install_references install_images all install_irods_stuff
	$(MAKE) -C $(DIR_GRASSROOTS_UTIL_BUILD) install
	$(MAKE) -C $(DIR_GRASSROOTS_NETWORK_BUILD) install
	$(MAKE) -C $(DIR_GRASSROOTS_PARAMS_BUILD) install
	$(MAKE) -C $(DIR_GRASSROOTS_IRODS_BUILD) install
	$(MAKE) -C $(DIR_GRASSROOTS_HANDLER_BUILD) install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/lib install
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/file install
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/irods install
#	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/dropbox install
	$(MAKE) -C $(DIR_CLIENTS_BUILD_ROOT)/lib install 
	$(MAKE) -C $(DIR_CLIENTS_BUILD_ROOT)/standalone install 
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/httpd install
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/lib install
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/standalone install
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/mongodb install
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/drmaa install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/blast install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/compress install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/web install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/json_search install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/web_search install	
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/irods_search install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/ensembl_rest install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/tgac_elastic_search install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/test_long_runner install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/pathogenomics install
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/samtools install
	git log -1 > $(DIR_GRASSROOTS_INSTALL)/grassroots.version
	
clean: 
	$(MAKE) -C $(DIR_GRASSROOTS_UTIL_BUILD) clean
	$(MAKE) -C $(DIR_GRASSROOTS_PARAMS_BUILD) clean
	$(MAKE) -C $(DIR_GRASSROOTS_NETWORK_BUILD) clean
	$(MAKE) -C $(DIR_GRASSROOTS_IRODS_BUILD) clean
	$(MAKE) -C $(DIR_GRASSROOTS_HANDLER_BUILD) clean
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/file clean
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/irods clean
	$(MAKE) -C $(DIR_HANDLER_ROOT_BUILD)/dropbox clean
	$(MAKE) -C $(DIR_CLIENTS_BUILD_ROOT)/lib clean 
	$(MAKE) -C $(DIR_CLIENTS_BUILD_ROOT)/standalone clean 
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/lib clean
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/httpd clean
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/lib clean
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/standalone clean
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/mongodb clean
	$(MAKE) -C $(DIR_SERVER_BUILD_ROOT)/drmaa clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/blast clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/compress clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/web clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/json_search clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/web_search clean	
#	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/irods_search clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/ensembl_rest clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/tgac_elastic_search clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/test_long_runner clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/pathogenomics clean
	$(MAKE) -C $(DIR_SERVICES_ROOT_BUILD)/samtools clean

build_docs:
	cd ../../docs; \
	doxygen

release: install build_docs
	$(DELETE) grassroots_release.zip; \
	cd docs/api;
	zip -r $(DIR_ROOT)/grassroots_release.zip html; \
	cd $(DIR_GRASSROOTS_INSTALL); \
	zip -r $(DIR_ROOT)/grassroots_release.zip HANDLER images lib references services grassroots.version client; \
	cd $(DIR_APACHE)/modules; \
	zip $(DIR_ROOT)/grassroots_release.zip mod_grassroots.so; \
	
install_init:
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/extras
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/lib
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/services
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/references
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/images
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/clients
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/HANDLER


install_deps: install_init install_jansson install_htmlcxx install_hcxselect install_oauth install_dropbox_c install_uuid install_mongodb_c install_samlib
	
install_references: 
	$(COPY) $(DIR_PKG_ROOT)/server/references/* $(DIR_GRASSROOTS_INSTALL)/references/

install_images:
	$(COPY) $(DIR_PKG_ROOT)/images/* $(DIR_GRASSROOTS_INSTALL)/images

install_jansson: 
	cd $(DIR_PKG_ROOT)/extras/jansson-2.6; \
	./configure --prefix=$(DIR_JANSSON); \
	make; \
	make install	
	
install_htmlcxx:
	cd $(DIR_PKG_ROOT)/extras/htmlcxx-0.84; \
	./configure --prefix=$(DIR_HTMLCXX); \
	make; \
	make install		
	
install_hcxselect:
	@mkdir -p $(DIR_HCXSELECT)
	@mkdir -p $(DIR_HCXSELECT_INC)
	@mkdir -p $(DIR_HCXSELECT_LIB)
	cd $(DIR_PKG_ROOT)/extras/hcxselect-1.1; \
	make; \
	make install		

install_dropbox_c:
	cd $(DIR_PKG_ROOT)/extras/Dropbox-C; \
	make all LIBRARY_INSTALL_PATH=$(DIR_DROPBOX_LIB) INCLUDE_INSTALL_PATH=$(DIR_DROPBOX_INC) JANSSON_PATH=$(DIR_JANSSON) LIBOAUTH_PATH=$(DIR_OAUTH)

install_oauth:
	cd $(DIR_PKG_ROOT)/extras/liboauth-1.0.3; \
	./configure --prefix=$(DIR_OAUTH); \
	make; \
	make install			
	
install_uuid: 
	cd $(DIR_PKG_ROOT)/extras/libuuid-1.0.3; \
	./configure --prefix=$(DIR_UUID); \
	make; \
	make install			

install_blast:
	cd $(DIR_PKG_ROOT)/extras/ncbi-blast-2.2.30+-src/c++; \
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

install_mongodb_c:
	cd $(DIR_PKG_ROOT)/extras/mongo-c-driver-1.3.5; \
	./configure --prefix=$(DIR_MONGODB); \
	make; \
	make install		
	

install_samlib:
	cd $(DIR_PKG_ROOT)/extras/samtools-1.3; \
	./configure --prefix=$(DIR_SAMTOOLS) --without-curses; \
	make all; \
	make install
		
install_pooltest:
	$(MAKE) -C server/httpd/mod_pooltest install


install_slurm_drmaa:
	cd $(DIR_PKG_ROOT)/extras/slurm-drmaa-1.0.7; \
	./configure --prefix=$(DIR_SLURM_DRMAA); \
	make; \
	make install


