# [Server](#server)

A Grassroots Server exposes Services to be used by both Clients and also other Grassroots Servers. Grouping Servers together gives the ability to offer multiple Services without having to physically install each Service on every Server. Inside the Grassroots code, there are currently two Server instances. The first is the Apache httd module, mod_grassroots, which is the recommended solution and can be compiled and installed on at least httpd 2.2.x and higher. If for some reason you cannot run httpd there is also a standalone Server, which should be considered far less robust and should only be used by developers for experimental purposes and definitely never in a production environment.

## Mod_grassroots

All of the configuration details for this module are added to the appropriate conf folder within an httpd installation and is of the form:

~~~
<LocationMatch "/grassroots">
LoadModule grassroots_module modules/mod_grassroots.so
SetHandler grassroots-handler
GrassrootsRoot /opt/grassroots
</LocationMatch>

Alias /grassroots/docs /opt/grassroots/docs
~~~


## Connecting Multiple Servers

A benefit of using the Grassroots API is that Services can be federated. This means that instead of having to access each Server separately it is possible to connect two or more Grassroots Servers so that all of their Services become shared between all of the Servers. This means anyone accessing one of these Servers gets access to the combination of all of the Servers' Services. 

There are two ways that Servers can be connected. 
 * The first is where a list is produced of all of the possible Services and made accessible to a user. 
 * The second will also combine the Services and where possible merge the multiple instances of the same Service into a single point of access.

In either case, the resources of the 2 Servers appear to the user as a single instance.

To demonstrate this, let's take an example. Consider a pair of Grassroots Servers; the first being at https://server1.org/grassroots and https://server2.org/another/grassroots. The first Server has 2 Services: a Blast searching Service against a database called "foo" and a text searching Service. The second Server has 2 Services too: a Blast searching Service on a database called "bar" and a SNP search Service. 

Using the first method of connecting Servers together, a user would see the following Services:

 * The Blast Service against the foo database.
 * The text searching Service.
 * The Blast Service against the bar database.
 * The SNP search Service.

So if the user wished to do a Blast search, they would need to enter their parameters twice, once for searching against the foo database and once against the bar database.
Obviously this duplication doesn't make sense which is where the second method of connecting Servers comes in. Using this approach, Services can be connected together and their Parameters combined. For instance with instances of a Blast Service, the variables such as the scoring parameters input sequences, *etc.* would be in common and the actual databases to be searched against are where they differ. So with merging of Services, our example becomes:

 * The Blast Service against both the foo and bar databases.
 * The text searching Service.
 * The SNP search Service.
  
So the user doesn't need to repeat their chosen Blast Service parameters and the Grassroots architecture takes care of sending the required parameters to the separate Blast Services and collating the results together. 

### Configuration

To set up a connection between 2 Servers, the *grassroots.config* needs to be amended by adding an entry to the **servers** key. This is an array holding a list where each item relates to an external Grassroots Server. An example for one of these items is shown below. 

~~~~.json
	"server_name": "grassroots 1",
	"server_uri": "localhost:18080/grassroots",
	"paired_services": [{
		"local": "Foo Blast service",
		"remote": "Bar Blast service"
	}]
~~~~

Each item must have the following keys:

 * **server_name**: This is name used to denote the Server.
 * **server_uri**: This is the web address to send the JSON Grassroots messages to access the Server.
 * **paired_services**: If you wish to connect local and remote services together, this key points to an array of items consisting of two keys:
    * **local**: The name of the Service on the local Grassroots Server.
    * **remote**: The name of the Service on the remote Grassroots Server at *server_url*. 

For a Service to be paired, it needs to be written with this in mind. An example is the [Blast](http://blast.ncbi.nlm.nih.gov/Blast.cgi?CMD=Web&PAGE_TYPE=BlastHome/) Service for searching genetic sequences. See this example and the developer guide for more details.



