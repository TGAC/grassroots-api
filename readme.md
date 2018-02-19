# The Grassroots System

## Introduction

Broadly speaking, the Grassroots infrastructure is a lightweight architecture to share both distributed data and services across multiple servers. 

Grassroots consists of a schema of how to access, query, and interact with other Grassroots servers and a set of computing tools to allow users and developers to perform many scientific tasks as easily as possible. 
The infrastructure is accessible via a standard web connection and all data interchange is represented through [JSON](http://json.org/) messages so any server or client that can understand these JSON messages can be used to interact and integrate with Grassroots. For example, you can use a web browser or the included Qt-based desktop client to access all of the functionality of the system. The scientific data is what is important, not the user's choice of tool to access the system.

## Architecture

Using the [Apache httpd server](http://httpd.apache.org) as a fundamental part of the software stack, Grassroots consists of a core [Server](server.md) and a set of external modules, called [Services](services.md), that provide the scientific functionality. Access to the system is via a [Client](#clients). Grassroots servers can also be linked to others allowing the functionality of both to be shared to a user without having to perform any additional installation. This concept of federation and decentralisation is core to the Grassroots effort, making Grassroots instances "wrappers" or complementary infrastructure to existing data resources. Servers, Services and Clients are all interface specifications in that they declare an Application Programming Interface (API) that needs to be implemented. 

The architecture comprises:
* [build tools](https://github.com/TGAC/grassroots-build-tools)
  * Prerequisite libraries and configurations to build the grassroots core and associated modules
* [apache integration module](https://github.com/TGAC/grassroots-server-apache-httpd)
  * The Apache shared object module that exposes the underlying httpd web server functionality to the grassroots core server module
* [core server module](https://github.com/TGAC/grassroots-core)
  * The lowest level architectural implementation of the Grassroots infrastructure, including the required interactions with the apache subsystem
* [filesystem handlers](https://github.com/TGAC/?q=grassroots-handler)
  * Provide functionality to access local, remote, and data grid resources for data storage and interoperability
* [worker services](https://github.com/TGAC/?q=%22grassroots-service%22)
  * The Grassroots services that can interact with the core server module and the filesystem handlers to start building functional elements of the system, e.g. BLAST searches
* [job tracker](https://github.com/TGAC/grassroots-job-tracker)
  * Any services that carry out analytical or processing tasks can be registered with a Django pipeline monitoring tool to lete users keep track of running jobs or retrieve results from completed service processes
* [clients](https://github.com/TGAC/?q=%22grassroots-client%22)
  * Client implementations that can interact with the worker service and core server modules to provide interfaces for end users, including typical web-based libraries to build dynamic websites and Qt libraries to build desktop applications

All JSON-based Server-Server and Server-Client communication specficiations are described in the [Schema guide](schema.md "Schema Guide"). Additional documentation can be found [here](https://github.com/TGAC/grassroots-docs).
