# Samtools service

The Samtools Service allows the Grassroots Server to use the functionality provided by [Samtools](http://www.htslib.org) by Heng Li *et al.*.

## Configuration options

* **index_files**: This is an array of objects with each consisting of two key-value pairs.
 Either option is sufficient when attempting to run the Samtools Service.
 
 * **Blast database**: The name of the Blast database file that Samtools can run against.
 * **Fasta**: The Fasta file that the Blast database was generated from.

 