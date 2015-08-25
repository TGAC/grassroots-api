﻿# Database Access {#database_access}

Many services uses a database to host the given data for a service and this can be accessed in a generic way using JSON-based queries.


##Searches

Searches are specified using various fields

* **data**:
This object contains all of the fields for your query. Each of the keys specify the name of the field that you want to add a search criterion for. These are described in more detail [below](#Comparisons) .


* **fields**:
This is an array of the fields to return for each item matching the query. If this is not specified then all of the possible fields will be returned. An example of how to specify which named fields to return, *e.g.* id, name and location is
~~~.json	
"fields": [
		"id", "name", "location"
]
~~~

### Comparsions {#Comparisons}

#### Equality Comparisons 

The default behaviour is that comparisons are searches for equality. 
When the value is a simple type such as a string or a number then this represents a *key = value* criterion, *i.e.*
~~~.json
{
	"data": {
		"name": "billy"
	}
}
~~~
would search for all entries with a *name* field set to billy.
    
For matching a field when it has one of a finite set of values, you can use a JSON array as the field. This is equivalent to an *IN* query where we match any row where the *key* has any of the values within this array. For example,
~~~.json
{
	"data": {
		"crop": [ "wheat", "oats", "barley", "malt" ]
	}
}
~~~    
would search for all entries with a *crop* field set to wheat, oats, barley or malt.


#### Other Comparisons
  
So far the comparisons have been for equality, however you can also do other comparisons. This can be done by setting the value to a JSON object. This object contains at least two sub-keys: *operator* and *value*. 

* **operator**:
The value for this key can be one of "=", "<", "<=", ">", ">=", "in", "range" and "not". 

* **value**: 
This is the value that the *operator* will act upon. This will be generally be a single value except for when *operator* is set to "in" or "range". For "in" it acts as described above. For "range", this value will be  an array of two values that are the lower and upper inclusive bounds to match against. For example to get a *longtitude* value greater than or equal to 14 degrees, the corresponding json would be

~~~.json
{
	"data": {
		"longtitude": {
			"operator": ">=",
			"value": 14.000
		 }
	}
}
~~~

Whereas to get a *longtitude* value between 14 and 16 degrees, the corresponding json would be

~~~.json
{
	"data": {
		"longtitude": {
			"operator": "range",
			"value": [ 14.000, 16.000 ]
		 }
	}
}
~~~


So a complete example to search for entries where the *crop* is wheat, the *longtitude* is between 10 and 11 degrees, the *latitude* is between 6 and 7 degrees, the *country* is either China, UK or France and getting the *ID*, *date* and *disease* fields would be

~~~.json
{
	"data": {
		"crop": "wheat",
		"longtitude": {
			"operator": "range",
			"value": [ 10.000, 11.000 ]
		 },
		"latitude": {
			"operator": "range",
			"value": [ 6.000, 7.000 ]
		 },
		 "country": [
			"China", "UK", "France"		 
		]
	},
	"fields": [
		"ID", "date", "disease"	
	]
}
~~~

