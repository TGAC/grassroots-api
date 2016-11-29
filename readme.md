# The Grassroots System

## Introduction

Broadly speaking, the Grassroots information system (GIS) is an architecture to share both distributed data and services across multiple servers. 
These are shared in a decentralised way and consists of a schema of how to access, query and interact with other GIS servers and a set of computing tools to allow users and developers to perform many scientific tasks as easily as possible. 
These tools are accessible via a web connection so are accessible for anyone with an internet connection.
All of the data is sent as [JSON](http://json.org/) messages so any server or client that can understand these JSON messages can be used to access the GIS. For example, you can use a web browser or the included Qt-based desktop client to access all of the functionality of the system. The scientific data is what is important, not the user's choice of tool to access the system.


## Architecture

Using a similar approach to that used by the [Apache httpd server] (http://httpd.apache.org), the GIS consists of a core [Server](server.md) and a set of external modules, called [Services](services.md), that provide the scientific functionality. The access to the system is via a [Client](#clients). The GIS server can also be linked to other GIS servers allowing the functionality of both to be shared to a user without having to perform any additional installation. Servers, Services and Clients are all interface specifications in that they declare an Application Programming Interface (API) that need to be implemented. 

![Grassroots system architecture] (architecture.svg)


All of the Server-Server and Server-Client communication is by JSON-based messages as described in the [Schema guide](schema.md "Schema Guide")
