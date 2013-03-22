class Cmd{
 public:
  const static int num_cmds = 18;
  static const char * cmds[num_cmds];
  static const int parse_types[num_cmds];
  int cmd;
  char * cmd_args[];

  Cmd(int cmd){
    this->cmd=cmd;
  }

  ~Cmd(){
  }

  int exec();
  static bool test();
};
