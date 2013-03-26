#ifndef __CMD_H__
#define __CMD_H__

#include "Stash.h"
#include "Packet.h"
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
  Packet * exec_cmd(char * buffer, Stash * s);

 private:
  Packet * exec_add(const char * key, uint16_t flags, int exptime
		    , bool noreply, char * data_block, Stash * s);
  Packet * exec_append(const char * key, uint16_t flags, int exptime
		       , bool noreply, char * data_block, Stash * s);
  Packet * exec_cas(const char * key, uint16_t flags, int exptime
		    , uint64_t casunique,bool noreply, char * data_block, Stash * s);
  Packet * exec_prepend(const char * key, uint16_t flags, int exptime
			, bool noreply, char * data_block, Stash * s);
  Packet * exec_replace(const char * key, uint16_t flags, int exptime
			, bool noreply, char * data_block, Stash * s);
  Packet * exec_storage_cmd(char * buffer, Stash * s);
  Packet * exec_retrieval_cmd(Stash * s);
  Packet * exec_deletion_cmd(Stash * s);
  Packet * exec_cr_cmd(Stash * s);
  Packet * exec_touch_cmd(Stash * s);
  Packet * exec_slabs_cmd(Stash * s);
  Packet * exec_stats_cmd(Stash * s);
  Packet * exec_flush_cmd(Stash * s);
  Packet * exec_singleton(Stash * s);
  Packet * exec_verbosity(Stash * s);
  static bool test();
};
#endif
