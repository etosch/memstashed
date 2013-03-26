#ifndef __CMD_H__
#define __CMD_H__

#include "Stash.h"
#include <string>
#include <vector>

using namespace std;

class Cmd{
 public:
  const static int num_cmds = 18;
  static const char * cmds[num_cmds];
  static const int parse_types[num_cmds];
  int cmd;
  int blocksize;
  vector<string> args;
  void add(string s){ args.push_back(s); }
  Cmd(int cmd){ this->cmd=cmd; }
  ~Cmd(){ }
  int exec_cmd(char * buffer, Stash * s);

 private:
  int exec_add(const char * key, uint16_t flags, int exptime
	       , bool noreply, char * data_block, Stash * s);
  int exec_append(const char * key, uint16_t flags, int exptime
		  , bool noreply, char * data_block, Stash * s);
  int exec_cas(const char * key, uint16_t flags, int exptime
	       , uint64_t casunique,bool noreply, char * data_block, Stash * s);
  int exec_prepend(const char * key, uint16_t flags, int exptime
		   , bool noreply, char * data_block, Stash * s);
  int exec_replace(const char * key, uint16_t flags, int exptime
		   , bool noreply, char * data_block, Stash * s);
  int exec_storage_cmd(char * buffer, Stash * s);
  int exec_retrieval_cmd();
  int exec_deletion_cmd();
  int exec_cr_cmd();
  int exec_touch_cmd();
  int exec_slabs_cmd();
  int exec_stats_cmd();
  int exec_flush_cmd();
  int exec_singleton();
  int exec_verbosity();
  static bool test();
};
#endif
