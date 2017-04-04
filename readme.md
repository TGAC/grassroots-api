# The Grassroots System

## Introduction

Broadly speaking, the Grassroots infrastructure is a lightweight architecture to share both distributed data and services across multiple servers. 

Grassroots consists of a schema of how to access, query, and interact with other Grassroots servers and a set of computing tools to allow users and developers to perform many scientific tasks as easily as possible. 
The infrastructure is accessible via a standard web connection and all data interchange is represented through [JSON](http://json.org/) messages so any server or client that can understand these JSON messages can be used to interact and integrate with Grassroots. For example, you can use a web browser or the included Qt-based desktop client to access all of the functionality of the system. The scientific data is what is important, not the user's choice of tool to access the system.

## Architecture

Using the [Apache httpd server] (http://httpd.apache.org) as a fundamental part of the software stack, Grassroots consists of a core [Server](server.md) and a set of external modules, called [Services](services.md), that provide the scientific functionality. Access to the system is via a [Client](#clients). Grassroots servers can also be linked to others allowing the functionality of both to be shared to a user without having to perform any additional installation. This concept of federation and decentralisation is core to the Grassroots effort, making Grassroots instances "wrappers" or complementary infrastructure to existing data resources. Servers, Services and Clients are all interface specifications in that they declare an Application Programming Interface (API) that needs to be implemented. 

![Grassroots system architecture] (architecture.svg)

All JSON-based Server-Server and Server-Client communication specficiations are described in the [Schema guide](schema.md "Schema Guide").
