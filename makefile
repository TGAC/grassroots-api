COPY	= cp 
DELETE = rm

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


include project.properties
include drmaa.properties

lib_util:
	$(MAKE) -C util
 
lib_network:
	$(MAKE) -C network

lib_parameters:
	$(MAKE) -C parameters


#############################
######## BEGIN IRODS ########
#############################

install_irods_stuff: install_lib_irods install_handler_irods install_service_irods_search
build_irods_stuff: lib_irods handler_irods service_irods_search

ifeq ($(IRODS_ENABLED),1)

lib_irods:
	$(MAKE) -C irods/lib

install_lib_irods:	lib_irods
	$(MAKE) -C irods/lib install

handler_irods:
	$(MAKE) -C handlers/irods

install_handler_irods: handler_irods
	$(MAKE) -C handlers/irods install

service_irods_search: 
	$(MAKE) -C services/irods_search

install_service_irods_search: service_irods_search
	$(MAKE) -C services/irods_search install

else

lib_irods:	
	echo "**** Not building lib_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

install_lib_irods:	
	echo "**** Not running install_lib_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

handler_irods:
	echo "**** Not building handler_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

install_handler_irods:
	echo "**** Not running install_handler_irods as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

service_irods_search:
	echo "**** Not building service_irods_search as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

install_service_irods_search:
	echo "**** Not running install_service_irods_search as IRODS_ENABLED != 1, $(IRODS_ENABLED)" 

endif
#############################
######### END IRODS #########
#############################


	
lib_handlers:
	$(MAKE) -C handlers/lib
	
lib_services:
	$(MAKE) -C services/lib



ifdef build_drmaa_flag
drmaa: lib_util lib_network lib_parameters lib_irods lib_handlers lib_services
endif


# SLURM DRMAA
ifeq ($(SLURM_DRMAA_ENABLED),1)	

all: drmaa_slurm

install: install_drmaa_slurm

drmaa_slurm: drmaa
	$(MAKE) -C drmaa/base
	
install_drmaa_slurm: drmaa_slurm
	$(MAKE) -C drmaa/base install

# LSF DRMAA
else ifeq ($(LSF_DRMAA_ENABLED),1)

all: drmaa_lsf

install: install_drmaa_lsf

drmaa_lsf: drmaa
	$(MAKE) -C drmaa/base

install_drmaa_lsf: drmaa_lsf
	$(MAKE) -C drmaa/base install 
endif


info:
	@echo "installing grassroots to $(DIR_GRASSROOTS_INSTALL)"
	@echo "installing apache module to $(DIR_APACHE)/modules"
	@echo "APXS=$(APXS)"	
	@echo "GRASSROOTS_VERSION=$(GRASSROOTS_VERSION)"


valgrind: 
	$(MAKE) all CFLAGS=-DUSING_VALGRIND

all: lib_util lib_network lib_parameters lib_irods lib_handlers lib_services handler_irods service_irods_search
	@echo "DIR_HTMLCXX= = " $(DIR_HTMLCXX)
	@echo "HTMLCXX_HOME = " $(HTMLCXX_HOME)
	$(MAKE) -C handlers/file
#	$(MAKE) -C handlers/dropbox
	$(MAKE) -C server/lib
	$(MAKE) -C server/standalone
	$(MAKE) -C server/httpd/mod_grassroots
	$(MAKE) -C mongodb 
	$(MAKE) -C clients/lib
	$(MAKE) -C clients/standalone
#	$(MAKE) -C clients/web-server-client	
	$(MAKE) -C services/blast
	$(MAKE) -C services/compress
	$(MAKE) -C services/web
	$(MAKE) -C services/json_search
	$(MAKE) -C services/web_search
	$(MAKE) -C services/ensembl_rest
	$(MAKE) -C services/tgac_elastic_search
	$(MAKE) -C services/test_long_runner
	$(MAKE) -C services/pathogenomics 
	$(MAKE) -C services/samtools
							
install: install_init install_references install_images all install_irods_stuff
	$(MAKE) -C util install
	$(MAKE) -C network install
	$(MAKE) -C parameters install
	$(MAKE) -C handlers/lib install
	$(MAKE) -C services/lib install
	$(MAKE) -C handlers/file install
#	$(MAKE) -C handlers/dropbox install
	$(MAKE) -C server/lib install 
	$(MAKE) -C server/standalone install 
	$(MAKE) -C server/httpd/mod_grassroots install
	$(MAKE) -C mongodb install
	$(MAKE) -C clients/lib install
	$(MAKE) -C clients/standalone install
#	$(MAKE) -C clients/web-server-client install	
	$(MAKE) -C services/blast install
	$(MAKE) -C services/compress install
	$(MAKE) -C services/web install
	$(MAKE) -C services/json_search install
	$(MAKE) -C services/web_search install
	$(MAKE) -C services/ensembl_rest install
	$(MAKE) -C services/tgac_elastic_search install
	$(MAKE) -C services/test_long_runner install
	$(MAKE) -C services/pathogenomics install	
	$(MAKE) -C services/samtools install
	git log -1 > $(DIR_GRASSROOTS_INSTALL)/grassroots.version
	
clean: 
	$(MAKE) -C util clean
	$(MAKE) -C network clean
	$(MAKE) -C parameters clean
	$(MAKE) -C irods/lib clean
	$(MAKE) -C handlers/lib clean
	$(MAKE) -C services/lib clean
	$(MAKE) -C handlers/file clean
	$(MAKE) -C handlers/irods clean
	$(MAKE) -C handlers/dropbox clean
	$(MAKE) -C server/lib clean 
	$(MAKE) -C server/standalone clean 
	$(MAKE) -C mongodb clean
	$(MAKE) -C drmaa/base clean
	$(MAKE) -C server/httpd/mod_grassroots clean
	$(MAKE) -C clients/lib clean
	$(MAKE) -C clients/standalone clean
#	$(MAKE) -C clients/web-server-client clean	
	$(MAKE) -C services/blast clean
	$(MAKE) -C services/compress clean
	$(MAKE) -C services/web clean
	$(MAKE) -C services/json_search clean
	$(MAKE) -C services/web_search clean	
#	$(MAKE) -C services/irods_search clean
	$(MAKE) -C services/ensembl_rest clean
	$(MAKE) -C services/tgac_elastic_search clean
	$(MAKE) -C services/test_long_runner clean
	$(MAKE) -C services/pathogenomics clean
	$(MAKE) -C services/samtools clean

build_docs:
	cd docs; \
	doxygen

release: install build_docs
	$(DELETE) grassroots_release.zip; \
	cd docs/api;
	zip -r $(DIR_ROOT)/grassroots_release.zip html; \
	cd $(DIR_GRASSROOTS_INSTALL); \
	zip -r $(DIR_ROOT)/grassroots_release.zip handlers images lib references services grassroots.version client; \
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
	@mkdir -p $(DIR_GRASSROOTS_INSTALL)/handlers


install_deps: install_jansson install_htmlcxx install_hcxselect install_oauth install_dropbox_c install_irods_dev install_uuid install_blast
	
install_references: 
	$(COPY) references/* $(DIR_GRASSROOTS_INSTALL)/references/

install_images:
	$(COPY) images/* $(DIR_GRASSROOTS_INSTALL)/images

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

install_mongodb_c:
	cd $(DIR_ROOT)/extras/mongo-c-driver-1.2.1; \
	./configure --prefix=$(DIR_MONGODB); \
	make; \
	make install		
	

install_samlib:
	cd $(DIR_ROOT)/extras/htslib; \
	./configure --prefix=$(DIR_HTSLIB); \
	make; \
	make install	
	
install_pooltest:
	$(MAKE) -C server/httpd/mod_pooltest install


install_slurm_drmaa:
	cd $(DIR_ROOT)/extras/slurm-drmaa-1.0.7; \
	./configure --prefix=$(DIR_SLURM_DRMAA); \
	make; \
	make install


