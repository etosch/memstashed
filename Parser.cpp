// ugh, lexing. only doing this because bison is apparently only for c/c++ gurus
// protocol can be found here : https://github.com/memcached/memcached/blob/master/doc/protocol.txt
// map for commands 
// 
#include "Parser.h"
// Parser.h includes Cmd.h
#include "Bucket.h"
// Bucket.h includes string.h
#include <iostream>

using namespace std;

const char * Cmd::cmds[] = {"add","append","cas","decr","delete"
			    ,"flush_all","get","gets","incr","prepend"
			    ,"quit","replace","set","slabs","stats"
			    ,"touch","verbosity", "version"};
const int Cmd::parse_types[] = {1,1,1,4,3,8,2,2,4,1,9,1,1,6,7,5,10,9};

Cmd * Parser::parse_storage_cmd(Cmd * c, int bufsize, char buffer[]){
  char key[KEY_SIZE+1]; int i = strlen(Cmd::cmds[c->cmd]); int j;
  char flags[6]; char exptime[11]; char bytes[11]; char casunique[20];
  // advance i until it points at a non-control/whitespace char
  auto advance = [&] () { 
    do{ i++; } while ( i<bufsize && (int)buffer[i]<33 );
  };
  // read key until whitespace
  auto read = [&] (char target[], int bound, char err_msg[]){
    j=0;
    for( ; (int)buffer[i]>32 ; j++, i++){
      if (j>bound){ cout << err_msg  << endl; exit(-1); }
      if (i==bufsize){ 
	cout << "Buffer too small (" << i; 
	cout<< "); increase size in Server.cpp?" << endl; exit(-1);}
      target[j]=buffer[i];
    }
    target[j+1]='\0';
  };
  advance(); read(key, KEY_SIZE, "Key too big!");
  advance(); read(flags, 5, "32-bit flags not supported.");
  advance(); read(exptime, 10, "System time must fit in an int.");
  advance(); read(bytes, 10, "Size of data block must fit in an int.");
  if (c->cmd == 2){
    advance(); read(casunique, 19, "Unique value must fit in a 64-bit int.");
  }
  // handle noreply later
  c->cmd_args[0]=key; c->cmd_args[1]=flags; c->cmd_args[2]=exptime; c->cmd_args[3]=bytes;
  if (c->cmd == 2) c->cmd_args[4]=casunique;
  return c;
}
Cmd * Parser::parse_retrieval_cmd(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_deletion_cmd(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_cr_cmd(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_touch_cmd(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_slabs_cmd(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_stats_cmd(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_flush_cmd(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_singleton(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_verbosity(Cmd * c, int bufsize, char buffer[]){
  return c;
}


Cmd * Parser::parse(int bufsize, char buffer[]){
  Cmd * c = Parser::parse_cmd(bufsize, buffer);
  switch (Cmd::parse_types[c->cmd]){
  case 1: return Parser::parse_storage_cmd(c, bufsize, buffer);
  case 2: return Parser::parse_retrieval_cmd(c, bufsize, buffer);
  case 3: return Parser::parse_deletion_cmd(c, bufsize, buffer);
  case 4: return Parser::parse_cr_cmd(c, bufsize, buffer);
  case 5: return Parser::parse_touch_cmd(c, bufsize, buffer);
  case 6: return Parser::parse_slabs_cmd(c, bufsize, buffer);
  case 7: return Parser::parse_stats_cmd(c, bufsize, buffer);
  case 8: return Parser::parse_flush_cmd(c, bufsize, buffer);
  case 9: return Parser::parse_singleton(c, bufsize, buffer);
  case 10: return Parser::parse_verbosity(c, bufsize, buffer);
  default: cout << "Bad command." << endl; exit(-1);
  }
}

Cmd * Parser::parse_cmd(int bufsize, char buffer[]){
  int cmd,i,j;
  for(cmd=-1, i=0, j=0 ; j<Cmd::num_cmds && i<bufsize ; i++){
    //    cout << buffer[i] << "(" << (int) buffer[i]<< ")," << Cmd::cmds[j] << endl;
    if (buffer[i] < 33) //commands are whitespace delimited
      return new Cmd(cmd);
    else if (buffer[i]!=Cmd::cmds[j][i]){
      cmd=-1; j++; i=0; //not the "smart" way
    } else // there is a match
      cmd=j;
  }
  // should never get here!
  return new Cmd(-1);
}

int Cmd::exec(){
  return 0;
}

bool Parser::test(){
  auto printtest = [] (char s[]) {
    Cmd * c = Parser::parse(512, s);
    int index = c->cmd;
    const char * this_cmd = (index<0)?"not found":Cmd::cmds[index];
    cout << "Command: " << this_cmd << "\tArgs: ";
    for (int i=0; c->cmd_args[i]!='\0' ; i++)
      cout << c->cmd_args[i];
    cout << endl;
  };
  printtest("add ;lkasjdf 123 ");
  printtest("touch");
  printtest("flush_all\r\n");
  printtest("get\tstuff");
  printtest("version");
  printtest("laksjdfha");
  return true;
}
