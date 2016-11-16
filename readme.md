﻿# The Grassroots System


Broadly speaking, the Grassroots information system (GIS) is an architecture to share both distributed data and services across multiple servers. These are shared in a decentralised way and consists of a schema of how to access, query and interact with other GIS servers and a set of computing tools to allow users and developers to perform many scientific tasks as easily as possible. These tools are accessible via a web connection so are accessible for anyone with an internet connection. All of the data is sent as [JSON](http://json.org/) messages so any server or client that can understand these JSON messages can be used to access the GIS. For example, you can use a web browser or the included Qt-based desktop client to access all of the functionality of the system. The scientific data is what is important, not the user's choice of tool to access the system.


## Architecture

Using a similar approach to that used by the [Apache httpd server] (http://httpd.apache.org), the GIS consists of a core [Server](#server) and a set of external modules, called [Services](docs/services_lib.md), that provide the scientific functionality. The access to the system is via a [Client](#clients). The GIS server can also be linked to other GIS servers allowing the functionality of both to be shared to a user without having to perform any additional installation. Servers, Services and Clients are all interface specifications in that they declare an Application Programming Interface (API) that need to be implemented. 

![Grassroots system architecture] (architecture.svg)


All of the Server-Server and Server-Client communication is by JSON-based messages as described in the [Schema guide](docs/schema.md "Schema Guide")

### Server

For more information, see the [Server](docs/server.md "Server guide") documentation.

### Client

A Client is what is used to connect to a GIS Server and call Services. This can be via a web browser, a desktop client or mobile application.

### Services

[GrassrootsIS Services](docs/services.md "Services guide") are the components that are accessed by the user to perform various tasks.


### Support libraries

There are a number of support libraries that any component within the GIS system can take advantage of.

#### DRMAA

[DRMAA](http://www.drmaa.org/) is an API to allow access to computing grids, clusters and cloud-based services. The GIS DRMAA library wraps up calls to specify things such as queues to submit jobs to, number of nodes to use, job monitoring, *etc.*

#### Network

#### iRODS

[iRODS](http://irods.org/) is a data storage layer that abstracts out where the data is stored as well as having the ability to add metadata to files and directories to allow for searching. The GIS iRODS library adds an API to simplify searching on various criteria.


#### Handler

Handlers are the family of components that allow data to be read or written to different storage systems. These are explained in the [Handlers] (docs/handlers.md "Handlers guide")

#### Utility

#### Parameters Library

The Parameters library stores all of the functionality that is needed to define, change, and manipulate Service configuration options by a Client. 

#### Client

#### Server library