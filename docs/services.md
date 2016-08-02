﻿[Services](#services)

# [Services Introduction](#services-introduction)

A Service is the component that is used to add some scientific functionality, *i.e.* text mining, scientific analysis, *etc.* to a Grassroots Server. Each Service consists of a number of API entry points that the Grassroots Server hooks into. These are the ability to respond to particular JSON-based queries and entry points for a given programming language. The Services are completely self-describing, the Server has no prior knowledge or need any configuration changes when installing a Service. Simply copy the service module into the services directory and it will be available for use straight away. There are two ways to add a Service to a Grassroots Server; as [standalone services](#standalone-services) or as [referred services](#referred-services). 


## [Standalone Services](#standalone-services)

Standalone services are those which perform specific-tasks. They can be written in any language and just need to be loadable by the technology of whichever server application that they are running on. Many of the Services that come with the Grassroots system are written in C/C++ and are designed to be used by the Apache httpd server.

### C/C++-based services within httpd

Services built in C/C++-based are shared libraries that only require 2 exported functions:

~~~{.c}
ServicesArray *GetServices (const json_t *config_p);

void ReleaseServices (ServicesArray *services_p);
~~~

The first function gets an array detailing all of the operations that this Service can perform. Most commonly the ServicesArray will contain a single Service, though it can have more if appropriate. The second function is used when these operations go out of scope. 
Effectively these 2 functions  a constructor/destructor pair for a given ServicesArray. 

So to 



## [Referred Services](#referred-services)

These are Services that use generic modules for their functionality and only differ in their configuration. Each configuration is a JSON file that details the parameters and information about the Service. The GrassrootsIS has examples
of these are that has a number of Referred Services that access various web-based searches. The core functionality for this is contained in a Service called *web_search_service*. The referred service accesses this functionality be setting the **plugin** key to *web_search_service*. This is then configured for each web-based search that is installed using a JSON file. The configuration files are stored in the *references* folder. For example, the GrassrootsIS can access the search engine at [Agris](http://agris.fao.org/agris-search/index.do) and it uses the configuration file shown below:
  
~~~{.json}
{
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
    "operations": [{
			"operation_id": "Agris Web Search service",
			"summary": "An operation to obtain matching articles",
			"description": "An operation to obtain matching articles from Agris",
			"about_uri": "http://agris.fao.org/agris-search/index.do",
			"uri": "http://agris.fao.org/agris-search/searchIndex.do",
			"method": "GET",
			"selector": "li.result-item h3 a",
			"parameter_set": {
				"parameters": [{
					"param": "query",
					"name": "Query",
					"default_value": "",
					"current_value": "",
					"type": "string",
					"grassroots_type": 11,
					"description": "The search term"
				}]
			}
		}]
  }
~~~

This details how to call Agris' search engine by specifying the URI to call (http://agris.fao.org/agris-search/searchIndex.do), the parameters to send, how to call the search engine and the Cascading Style Sheet (CSS) selector to use to extract each of the hits from the subsequent results page. So all that needs to be done to add another web-based search  service to the system is to add another configuration file to the references directory. 


## Web Search Service

The **Web Search Service** wraps up the access to a web-based search page and retrieves the results and puts them into the standard results format. It uses a JSON file that has the standard format describing its operations, parameters, *etc.* along with the following additional keys:

  * **plugin**: This is set to "web_search_service*.
  * **uri**: The web address of the search page to use.
  * **method**: This states the method used to send the query to the search page.
  	* **POST**: To send the search query as an HTTP POST request.
  	* **GET**: To send the search query as an HTTP GET request.
  * **selector**: The CSS selector to get each of the resultant hits from the search 
page's response. 



## Example Web Search Service

This section details how to write and a web search service as a JSON file. 

### Setting the service provider details

For this example imagine we have a web search engine at http://foobar.com/search.html for searching articles to do with wheat research, out initial JSON file might be:

~~~{.json}
{
	"schema_version": 0.1,
	"provider": {
		"name": "Foobar",
		"description": "A comapny specializing in wheat research",
		"uri": "http://foobar.com"
	},
	"services": {
	    "path": "Foobar Search service",
	    "summary": "A service to search articles",
		"description": "A service to search for wheat research articles.",
		"plugin": "web_search_service"
	 }
}
~~~

The functionality to wrap this web-based search up into a Grassroots service is stored in the **web_search_service** plugin, so we make sure that we have that information in our JSON file. 

~~~{.json}
"plugin": "web_search_service"
~~~



### Adding the parameters

On our example http://foobar.com/search.html, imagine that there is a single html-based search form defined and a snippet of its HTML is

~~~{.html}
<form action="/search" method="GET">
	Query: <input type="text" name="query" />
	Number of hits: <input type="number" name="size" value="20" />
	<input type="submit">
</form>
~~~


We need to use this information to specify the ParameterSet that our Service will use. In this case, we will need to specify 2 parameters: *query* and *size*.
The first is of type *text* and given that it is a search engine keyword, the matching Grassroots parameter type is a *keyword* and the its label in the form is "Query". This gives the following JSON description for the *query* parameter:

~~~{.json}
{
    "param": "query",
    "name": "Query",
    "default_value": "",
    "current_value": "",
    "type": "string",
    "grassroots_type_text": "keyword",
    "description": "The search term"
}
~~~


The next parameter is *size* which is the number of hits to return so maps to an *unsigned integer* value and has a default value of 20. 

~~~{.json}
{
    "param": "size",
    "name": "Number of hits",
    "default_value": "20",
    "current_value": "20",
    "type": "number",
    "grassroots_type_text": "unsigned integer",
    "description": "The number of hits to return"
}
~~~

So adding these to our Service description gives: 


~~~{.json}
{
	"schema_version": 0.1,
	"provider": {
		"name": "Foobar",
		"description": "A comapny specializing in wheat research",
		"uri": "http://foobar.com"
	},
	"services": {
	  "path": "Foobar Search service",
	  "summary": "A service to data mine wheat articles",
		"description": "A service to search for wheat research articles.",
		"plugin": "web_search_service",
		"operations": [{
      "operation_id": "Foobar Search service",
      "summary": "An operation to search for matching articles",
      "description": "An operation to search for matching articles",
      "parameter_set": {
        "parameters": [{
					"param": "query",
					"name": "Query",
					"default_value": "",
					"current_value": "",
					"type": "string",
					"grassroots_type_text": "keyword",
					"description": "The search term"
				}, {
					"param": "size",
					"name": "Number of hits",
					"default_value": "20",
					"current_value": "20",
					"type": "number",
					"grassroots_type_text": "unsigned integer",
					"description": "The number of hits to return"
				}]
			}
    }]		
  }
}
~~~

### Submitting the search

Now that we have specified the parameters, we need to tell the Grassroots system how and where to submit the search parameters to.
The key for the submission URI is *uri* and its value should be set to the action of the form, which in this case is */search*.
The key for the http method is *method* and it specifies the HTTP protocol used to submit the search parameters. In this case the form uses the *GET* HTTP method, so the updated JSON file becomes:

~~~{.json}
{
	"schema_version": 0.1,
	"provider": {
		"name": "Foobar",
 		"description": "A comapny specializing in wheat research",
 		"uri": "http://foobar.com"
	},
	"services": {
		"path": "Foobar Search service",
		"summary": "A service to data mine wheat articles",
		"description": "A service to search for wheat research articles.",
		"plugin": "web_search_service",
		"operations": [{
			"operation_id": "Foobar Search service",
			"summary": "An operation to search for matching articles",
			"description": "An operation to search for matching articles",
			"uri": "http://foobar.com/search",
			"method": "GET",
			"parameter_set": {
				"parameters": [{
					"param": "query",
					"name": "Query",
					"default_value": "",
					"current_value": "",
					"type": "string",
					"grassroots_type_text": "keyword",
					"description": "The search term"
				}, {
					"param": "size",
					"name": "Number of hits",
					"default_value": "20",
					"current_value": "20",
					"type": "number",
					"grassroots_type_text": "unsigned integer",
					"description": "The number of hits to return"
				}]
			}
		}]		
	}
}
~~~

### Getting the search results

The final part that we need to add are the details for how to extract the hits from the results page that the search engine returns.
This is done by specifying a key called *selector* that has the CSS selector for each entry on the list of results. 
So imagine that on the results page that http://foobar.com/search returns, our hits are given in a structure such as 

~~~{.html}
<ol id="results">
    <li>
        <a href="/articles/a.html">Wheat research</a>
        Some descriptive text about the first article
    </li>
    <li>
        <a href="/articles/b.html">Chinese Spring</a>
        Some descriptive text about the second article
    </li>
    <li>
        <a href="/articles/c.html">Oakley variety of Wheat</a>
        Some descriptive text about the third article
    </li>
</ol>
~~~

then the CSS selector that the Grassroots Service needs is for the links within the results which in this case would be:

~~~{.css}
ol.results li a
~~~

The Grassroots Web Search Service would convert these into results as shown below

~~~.{json}
{
    "path": "Foobar Search service",
    "status": 3,
    "description": "An operation to search for matching articles",
    "uri": "http://foobar.com/search",
    "results": [{
		"protocol": "http",
		"title": "Wheat research",
		"data": "http://foobar.com/a.html"
	}, {
		"protocol": "http",
		"title": "Chinese Spring",
		"data": "http://foobar.com/b.html"
	}, {
		"protocol": "http",
		"title": "Oakley variety of Wheat",
		"data": "http://foobar.com/c.html"
	}]
}
~~~

# Entry Points


## JSON

The way that a Server is queried about its Services is via JSON-based messages. Dependent upon the Way that the Server is built these may be GET, POST or body-based requests and you will need to refer to the specific Grassroots Server implementation for more information on this. For example, the httpd-based Grassroots Server defaults to receiving the JSON-based messages within the bodies of http(s) requests. The list of available operations are available in the [schema](schema.md#ops).


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
				"grassroots_type": 11,
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



