# Services {#services}

A Service is the componnent that is used to add some sceintific functionality, *i.e.* text mining, scientific analysis, *etc.* to a Grassroots Server. Each Service consists of a number of API entry points that the Grassroots Server hooks into. These are the ability to respond to particular JSON-based queries and entry points for a given programming language. The Services are completely self-describing, the Server has no prior knowledge or need any configuration changes when installing a Service. Simply copy the service module into the services directory and it will be available for use straight away. There are two ways to add a Service to a Grassroots Server; as [standalone services](services.md#standalone_services) or as [referred services](services.md#referred_services). 


## Standalone Services {#standalone_services}

Standalone services are those which perform specific-tasks. They can be written in any language and just need to be loadable by the technology of whichever server application that they are running on. Many of the Services that come with the Grassroots system are written in C/C++ and are designed to be used by the Apache httpd server.

### C/C++-based services within httpd

Services built in C/C++-based are shared libraries that only require 2 exported functions:

~~~{.c}
ServicesArray *GetServices (const json_t *config_p);

void ReleaseServices (ServicesArray *services_p);
~~~

The first function gets an array detailing all of the operations that this Service can perform and the second function is used when these operations go out of scope. Effectively they a constructor/destructor pair for a given Service. 


## Referred Services {#referred_services}

These are Services that use generic modules for their functionality and only differ in their configuration. Each configuration is a JSON file that details the parameters and information about the Service. An example
of this is that the GrassrootsIS has a number of Referred Services that access various web-based searches. The core functionality for this is contained in a Service called *web_search_service*. This is then configured for each web-based search that is installed using a JSON file. The configuration files are stored in the *references* folder. For example, the GrassrootsIS can access the search engine at [Agris](http://agris.fao.org/agris-search/index.do) and it uses the configuration file shown below:
  
~~~json
	"schema_version": 0.1,
	"provider": {
		"name": "Agris",
		"description": "A service to search for academic articles.",
		"uri": "http://agris.fao.org/agris-search/index.do"
	},
	"services": {
    "path": "Agris Search service",
    "summary": "A service to obtain articles",
		"description": "A service to obtain articles using search terms",    
    "plugin": "web_search_service",
    "operations": [
			{
				"operationId": "Agris Web Search service",
				"summary": "An operation to obtain matching articles",
				"description": "An operation to obtain matching articles from Agris",
				"about_uri": "http://agris.fao.org/agris-search/index.do",
				"uri": "http://agris.fao.org/agris-search/searchIndex.do",
				"method": "GET",
				"selector": "li.result-item h3 a",
				"parameter_set": {
					"parameters": [
						{
							"param": "query",
							"name": "Query",
							"default": "",
							"current_value": "",
							"type": "string",
							"tag": 1,
							"grassroots_type": 11,
							"description": "The search term"
						}
					]
				}
			}
		]
  }
~~~

This details how to call Agris' search engine by specifying the URI to call (http://agris.fao.org/agris-search/searchIndex.do), the parameters to send, how to call the search engine and the css selector to use to extract each of the hits from the subsequent results page. So all that needs to be done to add another web-based search  service to the system is to add another configuration file to the references directory. 



# Entry Points


## JSON

The way that a Server is queried about its Services is via JSON-based messages. Dependent upon the Way that the Server is built these may be GET, POST or body-based requests and you will need to refer to the specific Grassroots Server implementation for more infromation on this. For example, the httpd-based Grassroots Server defaults to receiving the JSON-based messages within the bodies of http(s) requests. The list of available operations are available in the [schema](schema.md#ops).


### List all Services

The JSON to ask a Service for a list of all of its available operations is:

~~~{.json}
{
	"operations": {
		"operation_id": 0
	}
}
~~~

If your Service is built in C/C++ then it can use the ```GetServiceAsJSON``` function to build the appropriate response. The response is described in more detail in the [schema](schema.md) and there are [examples](examples.md) too.


### Run Services

The Server will send a message detailing which, if any, operations for the Service to perform. These are part of a "services" array where each service will have at least a name, denoted by a ```service``` key and a boolean value, ```run```, saying whether an operation should be performed or not.  For operations where the ```run``` value is set to ```true```, then the parameter values to be used will then be specified.

~~~{.json}
{
	"services": [{
		"service": "Foobar Keyword Contig service",
		"run": true,
		"parameter_set": {
			"parameters": [{
				"param": "contig_name",
				"current_value": "BC000000100",
				"tag": 2,
				"grassroots_type": 11,
				"concise": true
			}]
		}
	}, {
		"service": "Foobar Keyword Search service",
		"run": false
	}]
}
~~~

### Get Service Results

As described [elsewhere](async_services.md), Services can perform operations either synchronously or asynchronously. When an operation is ran synchronously the Service waits for the operation to finish before returning the results, whereas when ran asynchronously the Service will return straight away and the Server will need to send a message to the Service to check whether the operation has completed. 

Once the operation has completed, the Service will send the results in a format similar to the example below.

~~~.json
{
	"service": "Foobar Keyword Contig service",
	"status": 3,
	"description": "An operation to obtain contig information using SNP or Contig names",
	"uri": "http://foobar.com/search_by_contig",
	"results": [{
		"protocol": "inline",
		"title": "item title",
		"data": "item data"
	}, {
		"protocol": "http",
		"title": "item title",
		"data": "foobar.com/view_data_online/id1"
	}, {
		"protocol": "inline",
		"title": "item title",
		"data": {
		    "format": "SNP",
		    "custom key": "custom value"
		}
	}]
}
~~~



