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

  void add(string s){
    args.push_back(s);
  }

  Cmd(int cmd){
    this->cmd=cmd;
  }

  ~Cmd(){
  }

  int exec();
  static bool test();
};
#endif
