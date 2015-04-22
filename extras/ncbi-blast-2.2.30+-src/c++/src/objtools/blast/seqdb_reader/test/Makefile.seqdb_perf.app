# $Id: Makefile.seqdb_perf.app 429818 2014-03-19 17:24:17Z ivanov $

APP = seqdb_perf
SRC = seqdb_perf
LIB_ = seqdb xobjutil blastdb $(SOBJMGR_LIBS)
LIB = $(LIB_:%=%$(STATIC))

CFLAGS    = $(FAST_CFLAGS) 
CXXFLAGS  = $(FAST_CXXFLAGS) 
LDFLAGS   = $(FAST_LDFLAGS) 

LIBS = $(CMPRS_LIBS) $(NETWORK_LIBS) $(DL_LIBS) $(ORIG_LIBS)

WATCHERS = madden camacho

CHECK_REQUIRES = in-house-resources
CHECK_CMD = seqdb_perf -db pataa -dbtype prot -scan_uncompressed -num_threads 4 /CHECK_NAME=scan_blastdb_mt
CHECK_CMD = seqdb_perf -db pataa -dbtype prot -scan_uncompressed -num_threads 1 /CHECK_NAME=scan_blastdb_st
CHECK_CMD = seqdb_perf -db pataa -dbtype prot -get_metadata /CHECK_NAME=get_blastdb_metadata

# This unit test suite shouldn't run longer than 15 minutes
CHECK_TIMEOUT = 900
