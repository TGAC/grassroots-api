# The Grassroots System

The Grassroots information system (GIS) is a set of computing tools to allow users and developers to perform many scientific tasks as easily as possible. These tools are accessible via a web connection so are accessible for anyone with an internet connection. All of the data is sent as [JSON](http://json.org/) messages so any server or client that can understand these JSON messages can be used to access the GIS. For example, you can use a web browser or the included Qt-based desktop client to access all of the functionality of the system. The scientific data is what is important, not the user's choice of tool to access the system.


## Architecture

Using a similar approach to that used by the [Apache httpd server] (http://httpd.apache.org), the GIS consists of a core [Server](#server) and a set of external modules, called [Services](services_lib.md), that provide the scientific functionality. The access to the system is via a [Client](#clients). The GIS server can also be linked to other GIS servers allowing the functionality of both to be shared without having to perform any additional installation. Servers, Services and Clients are all interface specifications in that they declare an Application Programming Interface (API) that need to be implemented. 

All of the Server-Server and Server-Client communication is by JSON-based messages as described in the [Schema guide](schema.md "Schema Guide")

### Server

For more information, see the [Server](server.md "Server guide") documentation.

### Client

A Client is what is used to connect to a GIS Server and call Services. This can be via a web browser, a desktop client or mobile application.

### Services

[GIS Services](services_lib.md "Services guide") are the components that are accessed by the user to perform various tasks.


### Support libraries

There are a number of support libraries that any component within the GIS system can take advantage of.

#### DRMAA

[DRMAA](http://www.drmaa.org/) is an API to allow access to computing grids, clusters and cloud-based services. The GIS DRMAA library wraps up calls to specify things such as queues to submit jobs to, number of nodes to use, job monitoring, *etc.*

#### Network

#### iRODS

[iRODS](http://irods.org/) is a data storage layer that abstracts out where the data is stored as well as having the ability to add metadata to files and directories to allow for searching. The GIS iRODS library adds an API to simplify searching on various criteria.


#### Handler

Handlers are the family of components that allow data to be read or written to different storage systems. These are explained in the [Handlers] (handlers.md "Handlers guide")

#### Utility

#### Parameters Library

The Parameters library stores all of the functionality that is needed to define, change, and manipulate Service configuration options by a Client. 

#### Client

#### Server library

## Installation

### Linux 

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