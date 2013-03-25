#ifndef __CMD_H__
#define __CMD_H__
#include <string>
#include <vector>

using namespace std;

class Cmd{
 public:
  const static int num_cmds = 18;
  static const char * cmds[num_cmds];
  static const int parse_types[num_cmds];
  int cmd;
  vector<string> args;
  void add(string s){ args.push_back(s); }
  Cmd(int cmd){ this->cmd=cmd; }
  ~Cmd(){ }
  int exec_storage_cmd();
  int exec_retrieval_cmd();
  int exec_deletion_cmd();
  int exec_cr_cmd();
  int exec_touch_cmd();
  int exec_slabs_cmd();
  int exec_stats_cmd();
  int exec_flush_cmd();
  int exec_singleton();
  int exec_verbosity();
  int exec_cmd();
  static bool test();
};
#endif
