# Installation

Practically all of the code used to build a GIS is platform-independent with less than 5 files having to be implemented in platform-specific code. 


## Linux

## Binary package

It's recommended that Grassroots be installed from a binary package where possible though you are welcome to build from source should you wish. The grassroots directory is self-contained and can be unpacked anywhere that you would like. 

To connect the Grassroots software available for users to access, it needs to run on a server. Currently, the installer comes with a version to run with Apache httpd 2.4.x. It does this by adding a module and configuration to httpd. The installer comes with a script to install the module and set the appropriate configuration details. Currently this exists for the system-default httpd installation on Ubuntu 14.04.

## Manual installation to the Apache web server

For a self-contained Apache server, there are a number of steps to perform:

1. Copy the mod_grassroots.so file into the modules folder. 

2. Copy the httpd-grassroots.conf into the conf/extras

 

## Building from source

The current system of installation requires building from source. 

1. Copy example_dependencies.properties to dependencies.properties. This file is read in by the makefile to allow the installation to be configured. An example file is shown below.

        export JANSSON_HOME=$(DIR_GIS_INSTALL)/extras/jansson
        export DROPBOX_HOME=$(DIR_GIS_INSTALL)/extras/dropbox
        export HCXSELECT_HOME=$(DIR_GIS_INSTALL)/extras/hcxselect
        export HTMLCXX_HOME=$(DIR_GIS_INSTALL)/extras/htmlcxx
        export SHARED_IRODS_HOME=$(DIR_GIS_INSTALL)/extras/irods
        export OAUTH_HOME=$(DIR_GIS_INSTALL)/extras/liboauth
        export UUID_HOME=$(DIR_GIS_INSTALL)/extras/libuuid
        export BLAST_HOME=$(DIR_GIS_INSTALL)/extras/blast
        
        export DIR_GIS_INSTALL=/opt/GIS
        
        export DIR_APACHE=/usr/local/apache2
        export APXS=$(DIR_APACHE)/bin/apxs
        
        export SUDO=sudo
        export DRMAA_ENABLED=1
        
    For a default installation, the dependencies of GIS, such as [jansson] (http://www.digip.org/jansson/), [htmlcxx] (http://htmlcxx.sourceforge.net/), *etc.* get installed automatically. The three lines at the bottom of the file specify where the Apache httpd server is installed so that the GIS module can get installed inside it correctly. 

    `DIR_GIS_INSTALL` is where you would like GIS to be installed.
    
    `DIR_APACHE` is where the Apache httpd server that you wish to run GIS with is installed.
    
    `APXS` is the path to the apxs tool that is used to make httpd modules.
    
    `SUDO` is used is you need elevated privileges to install to any of the folders specified. If you do not, you can leave this value blank *e.g.* `export SUDO=`
    
    `DRMAA_ENABLED` if this is set to 1 then the GIS drmaa support library will be built.

2. If you now run `make` it will compile the code and then run `make install` to install GIS.