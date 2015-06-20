# Schema

All of the messages between Servers and Clients use a JSON-based schema.

```.json
{
  "services": "TGAC Elastic Search service",
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
          "tag": 1163085380,
          "default": "study_accession",
          "wheatis_type": 5,
          "level": 7,
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
          "default": "",
          "tag": 1163086681,
          "level": 7,
          "wheatis_type": 5,
          "description": "The term to search for in the given field"
        }
      ]
    }
  }
}
```

## Service

* **name** (required):
The user-friendly name of the Service which will be displayed to the user.

* **about_uri**: 
A web address for more information about the Service.

* **description** (required): 
A user-friendly description of the Service.

* **synchronous**: 
When an Operation is run, if it is able to execute rapidly it will run to completion before sending the results back. However some Operations can take longer and rather than the block the rest of the sytem from running, they start running and return straight away. The system can then periodically check these running jobs to determine if they have finished successfully. Setting this variable to false, will declare that the Operation runs in this way. If this value is not set, then it will be assumed to be true and the Operation runs synchronously.

* **operations**:
An array of [Operation](#Operation) objects that the Service can perform.


## Operation {#Operation}

* **operationId** (required): The name for the Operation.

* **summary** (required):
 
* **description** (required):

* **parameter_set** (required): The [ParameterSet](#ParameterSet).


## ParameterSet {#ParameterSet}

The list of Parameters is contained within a ***ParameterSet***

* **parameters**:
This is an array of [Parameter](#Parameter) objects.

* **groups**: 
This is an array of strings listing all of the available groups for this set of Parameters.

## Parameter {#Parameter}

* **name**:
The user-friendly name of the parameter for displaying to a user. If this is not set, then the value for the *param* key will be used.

* **param** (required): 
The programmatic name of the parameter. If the *name* is not set, then this value will be used for displaying to the user.

* **description** (required): 

* **default**:
The default value of the parameter.

* **current_value** (required):
The current value of the parameter.

* **type**  (required): This can take one of:
 1. string
 2. number
 3. integer
 4. boolean
 5. array 
     
* **tag**: This can be any unique number
within an Operation to allow a Service to search for a Parameter by a tag value rather than a name if preferred. The
```MAKE_TAG``` macro can be used which constructs a 32-bit number from 4 given characters *e.g.* 

       ```MAKE_TAG(a,b,c,d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))```

* **wheatis_type** (required):
 1. *PT_BOOLEAN*: 
 2. *PT_SIGNED_INT*: The variable can hold a non-negative integer.
 3. *PT_UNSIGNED_INT*:
 4.	*PT_SIGNED_REAL*:
 5.	*PT_UNSIGNED_REAL*:
 6.	*PT_STRING*: The parameter is a generak string.
 7.	*PT_FILE_TO_WRITE*: The parameter is the name of an output file.
 8.	*PT_FILE_TO_READ*: The parameter is the name of an input file.
 9.	 *PT_DIRECTORY*: The parameter is The name of a directoty.
 10.	 *PT_CHAR*: The parameter is a single ASCII character.
 11.	 *PT_PASSWORD*: The parameter is a password.
 12.	 *PT_KEYWORD*: The parameter is a keyword meaning it will be set of the user chooses to
 run a keyword search.
 13. *PT_LARGE_STRING*:

* **description**:
The description of the parameter to display to the user.

* **enum**: 
If the Parameter can take only take one of set of restricted values, these can be specified as an array here.
The elements in this array have two fields:
 * *value*: The programmatic value that the Parameter will be set to.
 * *description*: The user-friendly name of the parameter for displaying to a user. If this is not set, then the value for the *value* will be used instead.

* **level**:
Parameters can be assigned whether they are 

* **group**:
If set, this specifies which of the groups listed in the [ParameterSet](#ParameterSet)'s groups that this Parameter belongs to.

## Credentials


## Provider
