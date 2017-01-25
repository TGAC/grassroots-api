# SamTools service

The SamTools Service allows the Grassroots Server to use the functionality provided by [Samtools](http://www.htslib.org) by Heng Li *et al.*. 
Currently it has the ability to retrieve a named complete scaffold from some sequence data.

## Configuration options

* **index_files**: This is an array of objects with each consisting of two key-value pairs.
 Either option is sufficient when attempting to run the SamTools Service.
 
 * **Blast database**: The name of the Blast database file that SamTools can run against.
 * **Fasta**: The Fasta file that the Blast database was generated from.

 