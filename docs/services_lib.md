# Services {#services}

Services provide the ability to perform one or more operations. The Services are completely self-describing, the Server has no prior knowledge or need any configuration changes when installing a Service. Simply copy the service module into the services directory and it will be available for use straight away. There are two ways to add a Service to a Grassroots Server; as standalone services or as referred services. 


## Standalone Services

Standalone services are those which perform specific-tasks. 



## Referred Services

These are Services that use generic modules for their functionality and only differ in their configuration. An example
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
