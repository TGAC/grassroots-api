# MongoDB Interaction

One of the facilities that Grassroots system provides is an API to access a [MongoDB](https://www.mongodb.org/) instance through JSON-based messages. All of the MongoDB messages are accessed using a **mongodb** key. 


## Inserting data

The client would send a child **insert** key with a list of child key-value pairs. These can be in a single object if you just want to insert a single document or as an array if you want to insert multiple documents. For example, both of the following requests are valid:

~~~{.json}
{
	"mongodb": {
		"collection": "bands",
		"insert": [{
			"band": "spinal tap",
			"volume": 11,
			"recorded in": "dobly"		
		}, {
			"band": "bad news",
			"label": "frilly pink"
		}]	
	}
}
~~~

~~~{.json}
{
	"mongodb": {
		"collection": "bands",
		"insert": {
			"band": "spinal tap",
			"volume": 11,
			"recorded in": "dobly"		
		}
	}
}
~~~

The response from the Server will be a list of **success** values along with the document ids in the same order as they were specified in the incoming request 

~~~{.json}
{
	"mongodb": {
		"response": [{
			"success": true,
			"id": "123456789012345678901234"
		}, {
			"success": false
		}]
	}
}
~~~

## Deleting documents

To delete documents from a collection, a Client would send a list of the queries that would specify which documents to remove.

~~~{.json}
{
	"mongodb": {
		"collection": "bands",
		"delete": [{
			"id": "123456789012345678901234"
		}, {
			"id": "1112223334445556667778889"
		}, {
			"crop": "wheat"
		}]
	}
}
~~~

## Updating documents

Documents that were previously added can be subsequently updated.
