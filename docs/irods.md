# iRods 

[iRods](www.irods.org) is a system of storing files with associated metadata within a filesystem. iRods instances that are located disparately can be shared with each other to provide access to shared resources. Part of the Grassroots system provides a wrapper around sets of iRods functions to give a higher level API. Some of these are exposed via a dedicated iRods search service.

## Library

The Grassroots iRods library currently adds functionality in two areas. The first is to get information about the data objects that a user owns, such as all data objects within a collection, any objects modified between two dates, *etc.* The second is to allow for more refined searches of both the iCAT metadata catalogue and also the other data areas within the iRods system.

## Search Service

The iRods search service gives the ability to search against the data stored within the iRods metadata catalogue. This data consists of sets of key-value pairs. This service has two search options:

1. As standard in iRods, you would need to know which keys exist to then search them them for particular values. With this Service, each keyword within the metadata catalogue is listed for the user along with all of its possible values. 

2. The ability for a value to be searched for across all possible keys. The results are then ranked by occurrence.


![iRods Service] (images/irods_service.png)

 
