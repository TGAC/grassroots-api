Developer Guide {#dev_guide}
================



Any language that can consume, parse and produce JSON-messages over a network has the potential to be used to build a component that can be used with other Grassroots components. 

For the current system, the code is written mostly in C with parts in C++, Java and Javascript. The server functionality is provided by using the supplied Apache httpd module, **mod_grassroots** although this could be replaced by another language and server of your choosing by writing the required code.


A Client is used to connect to a Grassroots Server and interact with the available Services. This can be via a web browser, a desktop client or mobile application.


Each of the existing libraries falls into either being server-specific, client-specific or usable by either.

## Server-specific libraries

For more information, see the [Server](server.md) documentation.

### DRMAA

[DRMAA](http://www.drmaa.org/) is an API to allow access to computing grids, clusters and cloud-based services. 
The GIS DRMAA library wraps up calls to specify things such as queues to submit jobs to, number of nodes to use, job monitoring, *etc.*
It can be used with both [LSF](http://www-03.ibm.com/systems/spectrum-computing/products/lsf/) and [SLURM](https://slurm.schedmd.com/) schedulers.


### iRODS

[iRODS](http://irods.org/) is a data storage layer that abstracts out where the data is stored as well as having the ability to add metadata to files and directories to allow for searching. The [Grassroots iRODS](irods.md) library adds an API to simplify searching on various criteria.


### Server library

The Server library takes care of consuming and JSON messages and processing these messages and making the appropriate in internal API calls along with any communication with other remote servers before returning the outgoing JSON message back to the initial message's sender.


### Services library


### Services

[Grassroots Services](services.md "Services guide") are the components that are accessed by the user to perform various tasks. 
Since the scientific functionality is delivered by each of the Services, most development will revolve around writing new Services. 


## Client-specific libraries


## Other libraries




### Network



### %Handler

Handlers are the family of components that allow data to be read or written to different storage systems such as local filesystems, iRods, http(s), Dropbox, *etc*.
By abstracting these different systems out, other parts of the Grassroots architecture can read and write data transparently without having to be concerned about the underlying storage mechanism.
Handlers are explained in detail in the [Handlers guide] (handlers.md "Handlers guide").


### Utility


### Parameters Library

The Parameters library stores all of the functionality that is needed to define, change, and manipulate Service configuration options by a Client. 

