# Database Access {#database_access}

Many services uses a database to host teh given data for a service and this can be accessed in a generic way using JSON-based queries.


##Searches

Searches are specified using various fields

* **data**:


* **fields**:
This is an array of the fields to return for each item matching the query. If this is not specfied then all of the possible fields will be returned.



~~~.json
{
	"data": {
		"crop": "wheat",
		"location": "UK",
		"date": 20140901
	},
	
	"fields": [
		"id", "name", "location"
	]
}
~~~