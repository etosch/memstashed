#include <errors.h>
#include <iostream>

const char * errors[] = { 
  "SERVER_ERROR.\r\n" //0
  ,"CLIENT_ERROR Insufficient arguments to the %s command.\r\n" //1
  ,"SERVER_ERROR Buffer too small (%i); increase size in Server.cpp?\r\n" //2
  ,"CLIENT_ERROR Key too big!\r\n" //3
  ,"CLIENT_ERROR 32-bit flags not supported.\r\n" //4
  ,"CLIENT_ERROR System time must fit in an int.\r\n" //5
  ,"CLIENT_ERROR Size of data block must fit in an int.\r\n" //6
  ,"CLIENT_ERROR Unique value must fit in a 64-bit int.\r\n" //7
  ,"CLIENT_ERROR To use noreply optional argument, type \"noreply\".\r\n" //8
  ,"CLIENT_ERROR Slab type size error.\r\n" //9
  ,"CLIENT_ERROR Slab src id number doesn't fit.\r\n" //10
  ,"CLIENT_ERROR Slab dest id number doesn't fit.\r\n" //11
  ,"CLIENT_ERROR Indicator should be a single value from 0,1,2.\r\n" //12
  ,"CLIENT_ERROR Unrecognized slab command.\r\n" //13
  ,"WARNING: stats with args not implemented.\r\n" //14
  ,"CLIENT_ERROR flush_all args of the wrong size.\r\n" //15
  ,"WARNING: verbosity not implemented.\r\n" //16
  ,"ERROR\r\n" //17
  ,"CLIENT_ERROR Value should be the decimal rep. of a 64-bit unsigned int.\r\n" //18
  , "Problem initializing pthread_mutex_init.\r\n" //19
  , "incr/decr instructions not implemented.\r\n" //20
  , "fuck you.\r\n" //21
};

void server_error() { throw 0; }
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
void bucket_lock_err() { throw 19; }
void not_implemented_err() { throw 20; }
void fuck_you() { throw 21; }
