# Schema

All of the messages between Servers and Clients use a JSON-based schema.




## Service

* **name** (required):
The user-friendly name of the Service which will be displayed to the user.

* **url**: 
A web address for more information about the Service.

* **description** (required): 
A user-friendly description of the Service.

* **operations**:
An array of [Operation](#Operation) objects that the Service can perform.


## Operation {#Operation}

* **operationId** (required): The name for the Operation.

* **summary** (required):
 
* **description** (required):

* **parameters** (required): An array of [Parameter](#Parameter) objects.


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


## Credentials


## Provider
