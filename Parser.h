#ifndef PARSER_H
#define PARSER_H

#include "Cmd.h"

class Parser{
 private:
  static void parse_storage_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_retrieval_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_deletion_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_cr_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_touch_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_slabs_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_stats_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_flush_cmd(Cmd * c, int bufsize, char buffer[]);
  static void parse_singleton(Cmd * c, int bufsize, char buffer[]);
  static void parse_verbosity(Cmd * c, int bufsize, char buffer[]);


 public: 
  static void parse(Cmd * c, int bufsize, char buffer[]);
  static Cmd * parse_cmd(int bufsize, char buffer[]);
  static bool test();
};

#endif
