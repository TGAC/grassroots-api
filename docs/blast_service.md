# BLAST Service

The [BLAST](http://blast.ncbi.nlm.nih.gov/Blast.cgi) Service allows BLAST queries to be submitted against a number of databases.

## Configuration

 * **working_directory**: This is the directory where are any input, output and log files created by the BLAST Service. This directory must be writeable by the user running the Grassroots Server. For instance, the httpd server is often run as the daemon user.
 * **databases**: This is an array of objects giving the details of the available databases. The objects in this array have the following keys: 
  * **name**: This is the database value that the BLAST query will use to search against.
  * **description**: This is a user-friendly description to display to the user.
  * **active**: This is a boolean value that specifies whether the database is selected to search against by default. 
 * **blast_formatter**: This key determines how the output from the BLAST searches can be converted between the different available output formats. Currently the only available option for this is **system**. 
 * **blast_command**: This is the path to the executable used to perform the searches. 
 * **blast_tool**: This determines how the BLAST search will be run and currently has the following options:
  * **system**: This will be run using the executable specified by *blast_command* to the ANSI-specified *system()* function. This is the default *blast_tool* option.
  * **drmaa**: This will be run by submitting a job to a DRMAA environment.
