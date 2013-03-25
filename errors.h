#ifndef ERRORS_H
#define ERRORS_H

#include "Cmd.h"
#include <vector>
#include <string>

extern const char *  errors[];
void cmd_parse_err ();
void buffer_size_err ();
void key_size_err ();
void flag_size_err();
void exptime_size_err();
void datasize_size_err();
void casunique_size_err();
void noreply_size_err();
void slab_size_err();
void slab_src_size_err();
void slab_dest_size_err();
void indicator_size_err();
void unrecognized_slab_cmd();
void stats_args_err();
void flush_args_err();
void verbosity_err();
void bad_cmd_err();
void cr_size_err();

#endif
