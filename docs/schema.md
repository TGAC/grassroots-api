The Grassroots Schema
=====================

# [Schema Introduction](#schema-introduction)

All of the messages between Servers and Clients use a JSON-based schema. A full example of this is shown below and then each part will get described in turn. There are [Examples](examples.md)

Any message can have a header, much like the \<head\> tag in a web page and although this is optional it is recommended that all messages have one of these. The information contained in it are items such as the version of the Grassroots schema and the incoming request.


## [Example](#Example)

~~~.json
{
  "service_name": "TGAC Elastic Search service",
  "description": "A service to access the TGAC Elastic Search data",
  "nickname": "TGAC Elastic Search service",
  "summary": "A service to access the TGAC Elastic Search data",
  "about_uri": "http://v0214.nbi.ac.uk:8080/wis-web/",
  "synchronous": true,
  "operations": {
    "parameter_set": {
      "parameters": [
        {
          "param": "Search field",
          "current_value": "study_accession",
          "type": "string",
          "default_value": "study_accession",
          "grassroots_type": "xsd:string",
          "level": 7,
          "level_text": "all",
          "description": "The field to search",
          "enum": [
            {
              "description": "Study Accession",
              "value": "study_accession"
            },
            {
              "description": "Scientific Name",
              "value": "scientific_name"
            },
            {
              "description": "Centre Name",
              "value": "center_name"
            },
            {
              "description": "Experiment Title",
              "value": "experiment_title"
            },
            {
              "description": "Study Title",
              "value": "study_title"
            }
          ]
        },
        {
          "param": "Search term",
          "type": "string",
          "current_value": "",
          "default_value": "",
          "level_text": "all",
          "level": 7,
          "grassroots_type": "xsd:string",
          "description": "The term to search for in the given field"
        }
      ]
    }
  }
}
~~~

##Operations 

The Operations tag is used to call an API on the Server.

* **operation_id** (required): A number which equates to a particular API call for the Operation.

 Value | C variable | Description  
--- | --- | ---
0 | OP_LIST_ALL_SERVICES | Get a list of all of the Services that the Server can offer. 
1 | OP_IRODS_MODIFIED_DATA | Get a list of all of a user's iRODS data objects and collections that have been modified within a given period. 
2 | OP_LIST_INTERESTED_SERVICES | Get a list of the Services that are able to run on a given Resource. 
3 | OP_RUN_KEYWORD_SERVICES | Run all of the Services that have Keyword Parameters. 
4 | OP_GET_NAMED_SERVICES | Get the full description of a named Service. 
5 | OP_CHECK_SERVICE_STATUS | For asynchronously-running jobs, check the status of these jobs.
6 | OP_GET_SERVICE_RESULTS | For asynchronously-running jobs, get their results. 
7 | OP_CLEAN_UP_JOBS | For asynchronously-running jobs, clean up any temporary resources that they used.
 
 So, for example, the JSON-based request to get a list of all available Services is:

 ~~~.json
{
		"operations": {
			"operation_id": 0
		}
}
 ~~~
 
## Service

* **service_name** (required):
The user-friendly name of the Service which will be displayed to the user.

* **about_uri**: 
A web address for more information about the Service.

* **description** (required): 
A user-friendly description of the Service.

* **synchronous**: 
When an Operation is run, if it is able to execute rapidly it will run to completion before sending the results back. However some Operations can take longer and rather than the block the rest of the system from running, they start running and return straight away. The system can then periodically check these running jobs to determine if they have finished successfully. Setting this variable to false, will declare that the Operation runs in this way. If this value is not set, then it will be assumed to be true and the Operation runs synchronously.

* **operations**:
An array of [Operation](#Operation) objects that the Service can perform.


## [Operation](#operation)

* **operation\_id** (required): The name for the Operation.

* **summary** (required):
 
* **description** (required):

* **parameter\_set** (required): The [ParameterSet](#ParameterSet).


## [ParameterSet](#parameterset)

The list of [Parameters](#Parameter) is contained within a **ParameterSet** along with hints on how to group them together to display to the user.

* **parameters**:
This is an array of [Parameter](#Parameter) objects.

* **groups**: 
This is an array of strings listing all of the available groups for this set of Parameters.

## [Parameter](#parameter)

* **name**:
The user-friendly name of the parameter for displaying to a user. If this is not set, then the value for the *param* key will be used.

* **param** (required): 
The programmatic name of the parameter. If the *name* is not set, then this value will be used for displaying to the user.

* **description** (required): 
The description of the parameter to display to the user.

* **default_value**:
The default value of the parameter.

* **current_value** (required):
The current value of the parameter.

* **type**  (required): This can take one of:
 1. string
 2. number
 3. integer
 4. boolean
 5. array 
     
* **grassroots\_type** (required): 
A number to describe the type of the Parameter. The values, along with their C definition, are shown below:

| C definition | JSON value | Description |
|-------------------|-------------------|-------------------| 
| *PT_BOOLEAN* | xsd:boolean | The variable can be true or false. |
| *PT_SIGNED_INT* | params:signed_integer |The variable is a non-negative integer. |
| *PT_UNSIGNED_INT* | params:unsigned_integer |The variable is an integer. |
| *PT_NEGATIVE_INT* | params:negative_integer | The variable is a negtive integer. |
| *PT_SIGNED_REAL* | xsd:double |The variable is a number. |
| *PT_UNSIGNED_REAL* | params:unsigned_number |  The variable is a non-negative number |
| *PT_STRING* | xsd:string | The parameter is a general string. |
| *PT_FILE_TO_WRITE* | params:output_filename | The parameter is the name of an output file. |
| *PT_FILE_TO_READ* | params:input_filename | The parameter is the name of an input file. |
| *PT_DIRECTORY* | params:directory | The parameter is the name of a directory. |
| *PT_CHAR* | params:character |The parameter is a single ASCII character. |
| *PT_PASSWORD* | params:password | The parameter is a password. |
| *PT_KEYWORD* | params:keyword |The parameter is a keyword meaning it will be set of the user chooses to run a keyword search. |
| *PT_LARGE_STRING* | params:large_string |The parameter is a string that can potentially get large in size. This is a hint to the Client to use a multi-line text box as opposed to a single one. |
| *PT_JSON* | params:json |The parameter is a JSON fragment. |
| *PT_TABLE* | params:tabular |The parameter holds tabular data with configurable row and column delimiters. These default to a newline and a comma respectively. |


* **enum**: 
If the Parameter can take only take one of set of restricted values, these can be specified as an array here.
The elements in this array have two fields:
 * *value*: The programmatic value that the Parameter will be set to.
 * *description*: The user-friendly name of the parameter for displaying to a user. If this is not set, then the value for the *value* will be used instead.

 An example of this is: 
 ~~~.json
 "enum": 
 [
      { "description": "Use Raw", "value": "z" },
      { "description": "Use Zip", "value": "zip" },
      { "description": "Use GZip", "value": "gz" }
 ]
 ~~~
 which indicate that the Parameter can take 1 of 3 possible values, "z", "zip" or "gz", and the values to show 
to the user are "Use Raw", "Use Zip" and "Use GZip". 

* **level**:
This is a number used to determine whether to show a Parameter to a user. The system defines 3 levels of Parameter; beginner, intermediate and advanced. The user can then choose which level of variables that they want displayed in their interface of choice. The values for the 3 levels are:
 * *beginner* = 1
 * *intermediate* = 2
 * *advanced* = 4
 
 These values can be added or or'ed together. For example if you wanted a parameter to be displayed in just the beginner level then you would set this value to 1. If you wanted it displayed at the intermediate and advanced level, the value would be 2 + 4 = 6. If you wanted the Parameter to always be displayed then the value needed would be 1 + 2 + 4 = 7.
 
* **group**:
If set, this specifies which of the groups listed in the [ParameterSet](#ParameterSet)'s groups that this Parameter belongs to and is detailed in the [ParameterGroup](#ParameterGroup) section below.


## [ParameterGroup](#parametergroup)

A ParameterGroup is a way of classing parameters together in a logical set. For example, a ParameterGroup called *Address* could contain Parameters for a house number, road name and town. It is used to give a hint to the user interface to display these parameters together to be more user-friendly.

## Credentials

The Credentials object is used if there is access to some form of restricted content or Services needed.

* **name**:
The name of the handler or authentication provider .

* **url**:
The web address of the authentication provider.

* **key**:
If using OAuth2, this is the client key token.

* **secret**:
If using OAuth2, this is the client secret token.

* **username**:
The encrypted username to use.

* **password**:
The encrypted password to use.

## Provider

This is the object that describes the entity that is providing the set of operations, described in the Service section. This could be an institution or a specific set of analysis tools.

* **name** (required):
The name of the Provider which will be displayed to the user.

* **url**: 
A web address for more information about the Provider.

* **description** (required): 
A description of the Provider.
 

## WebService

* **url** (required):
The url to call for this webservice.

* **format**:
How the webservice parameters will be set. This can be one of:
 * *get*: The webservice will be called using a GET request.
 * *post*: The webservice will be called using a POST request.
 * *json*: The webservice will be called using a POST request where the content of the request body is a json fragment.

## [Resource](#resource)

A Resource object describes a location and a piece of data such as a file, url, *etc.*

* **protocol** (required): The class for this Resource which will determine how to access this Resource. These use the Handlers available within the system and the currently available options are:

 * *file*: The Resource is a file or directory.
 * *http*: The Resource is an http-based web address.
 * *https*: The Resource is a secure https-based web address.
 * *irods*: The Resource is an iRODS data object, collection or zone.
 * *dropbox*: The Resource is a path to a Dropbox object. 
 * *inline*: The Resource is the raw data specified in the value field.
 
* **value** (required): The protocol-dependent value of how to access the object.

