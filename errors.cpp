#include <errors.h>

const char * errors[] = { 
  "Messages greater than buffer size (%iB) not yet supported."
  ,"Insufficient arguments to the %s command."
  ,"Buffer too small (%i); increase size in Server.cpp?"
  ,"Key too big!"
  ,"32-bit flags not supported."
  ,"System time must fit in an int."
  ,"Size of data block must fit in an int."
  ,"Unique value must fit in a 64-bit int."
  ,"To use noreply optional argument, type \"noreply\""
  ,"Slab type size error"
  ,"Slab src id number doesn't fit"
  ,"Slab dest id number doesn't fit"
  ,"Indicator should be a single value from 0,1,2"
  ,"Unrecognized slab command."
  ,"WARNING: stats with args not implemented"
  ,"flush_all args of the wrong size"
  ,"WARNING: verbosity not implemented"
  ,"Bad command."
  ,"Value should be the decimal rep. of a 64-bit unsigned int."
};

void cmd_parse_err () { throw 1; }
void buffer_size_err() { throw 2; }
void key_size_err() { throw 3; }
void flag_size_err() { throw 4; }
void exptime_size_err() { throw 5; }
void datasize_size_err() { throw 6; }
void casunique_size_err() { throw 7; }
void noreply_size_err() { throw 8; }
void slab_size_err() { throw 9; }
void slab_src_size_err() { throw 10; }
void slab_dest_size_err() { throw 11; }
void indicator_size_err() { throw 12; }
void unrecognized_slab_cmd() { throw 13; }
void stats_args_err() { throw 14; }
void flush_args_err() { throw 15; }
void verbosity_err() { throw 16; }
void bad_cmd_err() { throw 17; }
void cr_size_err() { throw 18; }
