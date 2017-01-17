# Linked Services

## Introduction 

Often it is desirable to run a Service based upon the results from another Service. The Grassroots API offers this facility by the idea of linking Services together. 

Although there can be multiple different linked Services for the sake of simplicity in this document we will assume there is just one, the *Output Service*, that used the results from the *Input Service*.

The configuration details of how to link the two Services together are given in the configuration file for the Input Service using the *linked_services* key which has an array of objects, one for each Output Service.

The JSON for each Linked Service uses the following keys:

 * **service_name**: The name of the Output Service to run.
 * **parameters**: This is an array of objects which pieces of data to extract from the Input Service's results to use as input values for the Output Service. Each of these objects have the following keys:
    
    * **input**: The name of the piece of data to get from the Input Service's results.
    * **output**: The name of the Output Service's Parameter whose value will be set to the *input* key's value specified above.
    * **multiple**: Setting this to true tells the Grassroots system to create the data for as many calls to the Output Service as there are of the matching *input* keys. This defaults to ```false```.

An example configuration for linking results of calls to the BLAST Service to the SamTools Service for getting full scaffolds is given below.

~~~.json
"linked_services": [{
	"service_name": "SamTools service",
	"parameters": [{
		"input": "database",
		"output": "Blast database"
	}, {
		"input": "scaffold",
		"output": "Scaffold",
		"multiple": "true"
	}]
}]
~~~
 
 
## Input Keys
 
By default, the method used by an Input Service to extract the relevant data from its results is to treat the output data as JSON-formatted and use compound keys to specify the fields to get. 

For instance, consider having a given Input Service generates results such as those given below

~~~.json
{
	"person": {
		"name": {
			"forename": "foo",
			"surname": "bar"
		}
	}
}
~~~
 
 and we want to use these in an Output Service called *Person Lookup* which has the input parameters of *first_name* and *family_name* 
 
 then the value for the forename can be retrieved by settting the *input* key within the Linked Service definition to *person.name.forename* and *person.name.surname* respectively *e.g.*
 
~~~.json
{
	"service_name": "Person Lookup",
	"parameters": [{
		"input": "person.name.forename",
		"output": "first_name"
	}, {
		"input": "person.name.surname",
		"output": "family_name"	
	}]
}
~~~


## Custom Results

For Input Services that do not produce results in JSON format that can be easily processed, developers can use custom functions to pull the data out as required.
An example of this is the BLAST Service which uses the function, ```ProcessLinkedServicesForBlastServiceJobOutput``` defined in ```blast_service_job.c```. For these types of Services, refer to their documentation to find out what keys are available for use as Input Services.   
 
 