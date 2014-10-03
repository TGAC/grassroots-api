
all: 	
	@echo "BUILD = " $(BUILD)
	$(MAKE) -C util 
	$(MAKE) -C irods/lib
	$(MAKE) -C network
	$(MAKE) -C services/lib
	$(MAKE) -C handle
	$(MAKE) -C server
	$(MAKE) -C clients
	$(MAKE) -C services/blast
	$(MAKE) -C services/compress
			
install: install_init
	$(MAKE) -C util install
	$(MAKE) -C irods/lib install
	$(MAKE) -C network install
	$(MAKE) -C services/lib install
	$(MAKE) -C handle install
	$(MAKE) -C server install install_exe
	$(MAKE) -C clients install
	$(MAKE) -C services/blast install
	$(MAKE) -C services/compress install


clean: 
	$(MAKE) -C util clean
	$(MAKE) -C irods/lib clean
	$(MAKE) -C network clean
	$(MAKE) -C services/lib clean
	$(MAKE) -C handle clean
	$(MAKE) -C server clean 
	$(MAKE) -C clients clean
	$(MAKE) -C services/blast clean
	$(MAKE) -C services/compress clean

install_init:
	@mkdir -p ../wheatis_demo
	@mkdir -p ../wheatis_demo/lib
	@mkdir -p ../wheatis_demo/services
	@mkdir -p ../wheatis_demo/clients

