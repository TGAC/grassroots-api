COPY	= cp
WHEATIS_INSTALL = ../wheatis_demo

all: 	
	@echo "BUILD = " $(BUILD)
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
	@mkdir -p $(WHEATIS_INSTALL)
	@mkdir -p $(WHEATIS_INSTALL)/lib
	@mkdir -p $(WHEATIS_INSTALL)/services
	@mkdir -p $(WHEATIS_INSTALL)/references
	@mkdir -p $(WHEATIS_INSTALL)/images
	@mkdir -p $(WHEATIS_INSTALL)/clients
	@mkdir -p $(WHEATIS_INSTALL)/handlers


install_references:
	$(COPY) references/* $(WHEATIS_INSTALL)/references/

install_images:
	$(COPY) images/* $(WHEATIS_INSTALL)/images
