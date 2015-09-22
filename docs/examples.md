# Requests and Responses {#Examples}

These are the Server-Server and Server-Client messages that are used to transmit data, job requests and results throughout the WheatIS. For more information on these messages, take a look at the [Schema](@ref schema.md).

All messages are sent as JSON-based messages.

1. A Client queries the Operations that a Server can provide.
An operationId of 0 is the one for asking for all of its possible Operations.
 
 ~~~{.json}
{
		"operations": {
			"operationId": 0
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
					"wheatis_type": 11,
					"description": "The SNP name (a 10 character alphanumeric beginning with 'BS' or 'BA'; e.g., BS00010624 or BA0033400) to find the contig to which it maps and all other putative SNPs, validated or not, that have been mapped to it. SNP names (IDs) may take different forms depending on the platform on which the assay was developed"
				}, {
					"param": "contig_name",
					"name": "Contig name",
					"default": "",
					"current_value": "",
					"type": "string",
					"tag": 2,
					"wheatis_type": 11,
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
					"wheatis_type": 11,
					"description": "Any keywords  (e.g., 'yellow rust', 'hot') "
				}]			
			}
		}]
	}
}
 ~~~
 

3. The user decides to run the "Agris Web Search service" with *query* set to *wheat* but not to run the "Foobar Keyword Search service. The Client's request back to the Server would be:
~~~.json
{
	"services": [{
      "service": "Agris Web Search service",
      "run": true,
      "parameter_set": {
         "parameters": [{
            "param": "query",
            "current_value": "wheat",
            "tag": 1,
            "type": "string",
            "wheatis_type": 11,
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
    "jobs": [
        {
            "results": [
                {
                    "title": "Study of effects of economical factors on water use Efficiency in irrigated cereals under farmers Condition in areas of lower KRB in Khuzestan province. [2009] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=IR2010000398"
                },
                {
                    "title": "Variation of Organic carbon, Nitrogen, Phosphorous, Potassium, Iron, Zinc, Copper and Manganese in Soil in several Wheat Based Rotations. [2014] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=IR2015001102"
                },
                {
                    "title": "Variation of Organic carbon, Nitrogen, Phosphorous, Potassium, Iron, Zinc, Copper and Manganese in Soil in several Wheat Based Rotations. [2014] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=IR2015002210"
                },
                {
                    "title": "Prediction of metabolizable energy in poultry feedstuffs basis on their chemical composition [2007] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=IR2008001320"
                },
                {
                    "title": "A Study on Water Use of Two Fruit Tree-Wheat Intercropping Systems in the Rocky Hilly Region of North China with Stable Carbon Isotope Technique [May.2012] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=CN2013001081"
                },
                {
                    "title": "Effect of phosphorus and zinc application on growth and nutrients uptake and distribution of wheat and rye [Oct. 2010] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=CN2011000332"
                },
                {
                    "title": "Effect of crop rotation on Hordeum spontaneum C. koch control in wheat fields of Fars province. [2006] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=IR2010000168"
                },
                {
                    "title": "An overview of economic issues in wheat research and development in Sub-Saharan Africa [1990] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=QY9000063"
                },
                {
                    "title": "A study of the adoption of bread wheat production technologies in Arsi Zone [2000] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=QY2001000078"
                },
                {
                    "title": "Nitrogen nutrition of rainfed winter wheat in tilled and no-till sorghum and wheat residues [Jul-Aug 1993] ",
                    "protocol": "http",
                    "value": "agris.fao.org/agris-search/search.do?recordID=US9437092"
                }
            ],
            "status_text": "Succeeded",
            "service_uuid": "09571f5a-d530-4919-a847-bca6be552b9f"
        }
    ],
    "metadata": {
        "service": "Agris Web Search service",
        "description": "An operation to obtain matching articles from Agris"
    }
}~~~

