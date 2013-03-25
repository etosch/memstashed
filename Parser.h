#ifndef PARSER_H
#define PARSER_H

#include "Cmd.h"

class Parser{
 private:
  static Cmd * parse_cmd(int bufsize, char buffer[]);
  static Cmd * parse_storage_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_retrieval_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_deletion_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_cr_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_touch_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_slabs_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_stats_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_flush_cmd(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_singleton(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_verbosity(Cmd * c, int bufsize, char buffer[]);


 public: 
  static Cmd * parse(int bufsize, char buffer[]);
  static bool test();
};

#endif
