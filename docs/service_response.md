# Service Results

When a Client sends a request to the Server asking a Service to be run, the Service will initiate one or more ServiceJobs and send a response back to the Client as soon as possible. This response will contain the status of these ServiceJobs and any results.


* **service**:
The user-friendly name of the Service which will be displayed to the user.

* **description**: 
A user-friendly description of the Service.

* **jobs**:
An array of ServiceJobs that have been requested to run by the Client.

## Jobs

* **status_text** / **status**: Each ServiceJob will have either one of these values which describe the current status of the given ServiceJob

 status | C variable | status_text | description
--- | --- | --- | ---
-3 | OS_FAILED | Failed | The ServiceJob failed
-2 | OS_FAILED_TO_START | Failed to start | The ServiceJob was unable to start
-1 | OS_ERROR | Error | The ServiceJob expreienced a general error
0 | OS_IDLE | Idle | The ServiceJob has yet to start
1 | OS_PENDING | Pending | The ServiceJob is waiting to start 
2 | OS_STARTED | Started | The ServiceJob has started
3 | OS_FINISHED | Finished | The ServiceJob has finished unsuccessfully
4 | OS_PARTIALLY_SUCCEEDED | Partially succeeded | The ServiceJob has partially succeeded
5 | OS_SUCCEEDED | Succeeded | The ServiceJob has completed succeeded
6 | OS_CLEANED_UP | Cleaned up | The ServiceJob has released all of its memory and resources

 Developers should use ```GetOperationStatusAsString``` and 
 ```GetOperationStatusFromString``` to convert between the numeric variables and the equivalent status text.

* **job_uuid**:
 This is a unique identifier used to denote the given instance of this ServiceJob upon the Server.
  
* **results**:
 This is an array of [Resources](schema#resource) that theServiceJob has generated successfully
* **errors**:
 This is an array of errors that the ServiceJob encountered whilst running.


~~~~{.json}
{
	"service": "Agris Web Search service",
	"description": "An operation to obtain matching articles from Agris",
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
    ]
}
~~~~ 