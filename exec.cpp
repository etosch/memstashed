#include "Cmd.h"
#include "errors.h"

int Cmd::exec_storage_cmd(){
  // handle noreply
  return 0;
}

int Cmd::exec_retrieval_cmd(){
  return 0;
}

int Cmd::exec_deletion_cmd(){
  return 0;
}

int Cmd::exec_cr_cmd(){
  return 0;
}

int Cmd::exec_touch_cmd(){
  return 0;
}

int Cmd::exec_slabs_cmd(){
  return 0;
}

int Cmd::exec_stats_cmd(){
  return 0;
}

int Cmd::exec_flush_cmd(){
  return 0;
}

int Cmd::exec_singleton(){
  return 0;
}

int Cmd::exec_verbosity(){
  return 0;
}

int Cmd::exec_cmd(){
  // this should be moved
  switch (Cmd::parse_types[this->cmd]) {
  case 1 : return this->exec_storage_cmd();
  case 2 : return this->exec_retrieval_cmd();
  case 3 : return this->exec_deletion_cmd();
  case 4 : return this->exec_cr_cmd();
  case 5 : return this->exec_touch_cmd();
  case 6 : return this->exec_slabs_cmd();
  case 7 : return this->exec_stats_cmd();
  case 8 : return this->exec_flush_cmd();
  case 9 : return this->exec_singleton();
  case 10 : return this->exec_verbosity();
  default : bad_cmd_err();
  }
  // should never get here.
  return 1;
}
