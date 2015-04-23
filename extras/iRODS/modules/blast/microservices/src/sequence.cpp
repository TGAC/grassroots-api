/*

In any case, the basic (expected) usage pattern for this code is as
follows.  Most of these steps are optional; see also the doxygen info
for each of these methods.

1. Construct the object and configure the database:

    - The constructor takes database name and configuration info, and
      a logfile to which to write status information.
    - SetMaskLetters
    - SetVerbosity (does not affect output, only logging)
    - SetMaxFileSize
    - RegisterMaskingAlgorithm

2. Configure sources of meta-data using these methods:

    - SetTaxids
    - SetLinkouts
    - SetMembBits
    - SetMaskDataSource

3. Configure local/remote sequence data "sources"; these will be
   searched for sequences mentioned by AddIds (see step 6).

    - SetSourceDb
    - SetUseRemote

4. Do all processing from a list of Ids and a FASTA data source;
   if this choice is used, do not use steps 5-8.

    - Build

5. Alternative to 4:  Start the processing

    - StartBuild

(Steps 6 and 7 may be done in any order.)

6. Add sequences using a list of ids; sequence data will come from
   remote and/or local source databases.

    - AddIds

7. Add sequences by copying all sequences found in specific sources:

    - AddFasta
    - AddSequences (2 variations)

8. Finish processing (finalize the constructed database).

    - End Build

*/


