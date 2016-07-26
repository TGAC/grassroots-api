Schema Examples
===============

# Requests and Responses {#Examples}

These are the Server-Server and Server-Client messages that are used to transmit data, job requests and results throughout the WheatIS. For more information on these messages, take a look at the [Schema](@ref schema.md).

All messages are sent as JSON-based messages.

1. A Client queries the Operations that a Server can provide.
An operationId of 0 is the one for asking for all of its possible Operations.
 
 ~~~{.json}
{
		"operations": {
			"operation_id": 0
		}
}
 ~~~

2. The Server sends its response. In this example, it has 2 Operations: "*Foobar Contig Search service*" and  
"*Foobar Keyword Search service*"
 ~~~.json
{
	"schema_version": 0.1,
	"provider": {
		"name": "foobar",
		"description": "A service to give information about SNPs and general search terms.",
		"url": "http://foobar.com/about"
	},
	"services": {
		"path": "Contig service",
		"summary": "A service to obtain contig information",
		"description": "A service to obtain contig information using SNP names, contig names or generic search terms",    
		"operations": [{
			"operationId": "Foobar Contig Search service",
			"summary": "An operation to obtain contig information using SNP or Contig names",
			"description": "An operation to obtain contig information using SNP or Contig names",
			"uri": "http://foobar.com/search_by_contig",
			"parameter_set": {
				"parameters": [{
					"param": "SNP_id",
					"name": "SNP id",
					"default": "BS00003643",
					"current_value": "BS00003643",
					"type": "string",
					"tag": 1,
					"grassroots_type": 11,
					"description": "The SNP name (a 10 character alphanumeric beginning with 'BS' or 'BA'; e.g., BS00010624 or BA0033400) to find the contig to which it maps and all other putative SNPs, validated or not, that have been mapped to it. SNP names (IDs) may take different forms depending on the platform on which the assay was developed"
				}, {
					"param": "contig_name",
					"name": "Contig name",
					"default": "",
					"current_value": "",
					"type": "string",
					"tag": 2,
					"grassroots_type": 11,
					"description": "The contig name (e.g. BC000000280)"
				}]
			}
		}, {
			"operationId": "Foobar Keyword Search service",
			"summary": "An operation to obtain contig information using generic search terms",
			"description": "An operation to obtain contig information generic search terms",
			"uri": "http://foobar.com/search_by_keyword",
			"parameter_set": {
				"parameters": [{
					"param": "Search_term",
					"name": "Search term",
					"default": "",
					"current_value": "",
					"type": "string",
					"tag": 3,
					"grassroots_type": 11,
					"description": "Any keywords  (e.g., 'yellow rust', 'hot') "
				}]			
			}
		}]
	}
}
 ~~~
 

3. The user decides to run the "Foobar Contig Search service" with contig_name set to *BC000000100* but not to run the "Foobar Keyword Search service. The Client's request back to the Server would be:
~~~.json
{
	"services": [{
		"service": "Foobar Keyword Contig service",
		"run": true,
		"parameter_set": {
			"parameters": [{
				"param": "contig_name",
				"current_value": "BC000000100",
				"tag": 2,
				"type": "string",
				"grassroots_type": 11,
				"level": 7,
				"concise": true
			}]
		}
	}, {
		"service": "Foobar Keyword Search service",
		"run": false
	}]
}
~~~

4. The Server can run the requested Operations. In this case, the results are available straight away as the Operations run synchronously.
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

