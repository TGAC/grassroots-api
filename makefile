COPY	= cp


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
					
install: install_init install_references
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
	
install_init:
	@mkdir -p ../wheatis_demo
	@mkdir -p ../wheatis_demo/lib
	@mkdir -p ../wheatis_demo/services
	@mkdir -p ../wheatis_demo/references
	@mkdir -p ../wheatis_demo/clients
	@mkdir -p ../wheatis_demo/handlers


install_references:
	$(COPY) references/* ../wheatis_demo/references/

