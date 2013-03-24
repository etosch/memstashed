// ugh, lexing. only doing this because bison is apparently only for c/c++ gurus
// protocol can be found here : https://github.com/memcached/memcached/blob/master/doc/protocol.txt
// map for commands 
// 
#include "Parser.h"
// Parser.h includes Cmd.h
#include "Bucket.h"
// Bucket.h includes string.h
#include <iostream>
#include <functional>

using namespace std;

const char * Cmd::cmds[] = {"add","append","cas","decr","delete"
			    ,"flush_all","get","gets","incr","prepend"
			    ,"quit","replace","set","slabs","stats"
			    ,"touch","verbosity", "version"};

const int Cmd::parse_types[] = {1,1,1,4,3,8,2,2,4,1,9,1,1,6,7,5,10,9};

void cmd_parse_err (Cmd * c) {
  cout << "Insufficient arguments to the " << Cmd::cmds[c->cmd] << " command." << endl;
  exit(-1);
}

auto advance_fn = [] (int &i, int bufsize, char buffer[]) {
  return [&i,bufsize,buffer] ()->int {
    do{ 
      if (!(int)buffer[i] || (buffer[i]=='\r' && buffer[i+1]=='\n')) {
	return 0;
      } else i++; 
    } while ( i<bufsize && (int)buffer[i]<33 );
    return 1;    
  };
};

auto read_fn = [] (int &i, char bufsize, char buffer[]){
  return [&i,bufsize,buffer] (char target[], int bound, char err_msg[])->void{
    int j = 0;
    for( ; (int)buffer[i]>32 ; j++, i++){
      if (j>bound){ cout << err_msg  << endl; exit(-1); }
      if (i==bufsize){ 
	cout << "Buffer too small (" << i; 
	cout << "); increase size in Server.cpp?" << endl; exit(-1);}
      target[j]=buffer[i];
    }
    target[j]='\0';
  };
};

auto add_node = [] (Cmd::cmd_node &initial_node) {
  return [&] (char s[]){
    static Cmd::cmd_node * last_ref = &initial_node;
    last_ref->cmd = string(s);
    last_ref->next = new Cmd::cmd_node();
    last_ref = last_ref->next;
  };
};

Cmd * Parser::parse_storage_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); int num_args;
  char key[KEY_SIZE+1]; char flags[6]; char exptime[11]; char bytes[11]; 
  char casunique[20]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  advance(); read(key, KEY_SIZE, "Key too big!");
  advance(); read(flags, 5, "32-bit flags not supported.");
  advance(); read(exptime, 10, "System time must fit in an int.");
  advance(); read(bytes, 10, "Size of data block must fit in an int.");
  if (c->cmd == 2){
    advance(); read(casunique, 19, "Unique value must fit in a 64-bit int.");}
  if ((int)buffer[i]>32) { 
    advance(); read(noreply, 7, "To use noreply optional argument, type \"noreply\"");}
  num_args = 4;
  auto add = add_node(c->cmd_args);
  add(key); add(flags); add(exptime); add(bytes);
  if (c->cmd == 2) { add(casunique); num_args++; } 
  c->num_args=num_args;
  return c;
}

Cmd * Parser::parse_retrieval_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]);
  int num_args = 0;
  Cmd::cmd_node *cmdp = &c->cmd_args;
  auto advance = [&] () {
    do {
      if (buffer[i]=='\r' && buffer[i+1]=='\n'){
  	if (num_args == 0) {
  	  cout << Cmd::cmds[c->cmd] << " requires at least one key." << endl;
  	  exit(-1);
  	} else {
  	  c->num_args=num_args;
  	  return 0;
  	}
      } else i++;
    } while ( i < bufsize && (int)buffer[i]<33 );
    return 1;
  };
  auto read_key = [&] () {
    char key[KEY_SIZE]; int j;
    for ( j = 0 ; (int)buffer[i]>32 ; j++, i++ ) {
      if (j==KEY_SIZE+1) { cout << "Key size too big!" << endl; exit(-1); }
      key[j]=buffer[i];
    }
    key[j]='\0';
    num_args++;
    cmdp->cmd = string(key);
    cmdp->next = new Cmd::cmd_node();
    cmdp = cmdp->next;
  };
  while (advance()) read_key();
  return c;
}

Cmd * Parser::parse_deletion_cmd(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  
  return c;
}
Cmd * Parser::parse_cr_cmd(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  // c->cmd_arg[0]="0";
  return c;
}
Cmd * Parser::parse_touch_cmd(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  // c->cmd_arg[0]="0";
  return c;
}
Cmd * Parser::parse_slabs_cmd(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  // c->cmd_arg[0]="0";
  return c;
}
Cmd * Parser::parse_stats_cmd(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  // c->cmd_arg[0]="0";
  return c;
}
Cmd * Parser::parse_flush_cmd(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  // c->cmd_arg[0]="0";
  return c;
}
Cmd * Parser::parse_singleton(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  // c->cmd_arg[0]="0";
  return c;
}
Cmd * Parser::parse_verbosity(Cmd * c, int bufsize, char buffer[]){
  c->num_args=0;
  // c->cmd_arg[0]="0";
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
    int num_args = c->num_args;
    Cmd::cmd_node this_cmd = c->cmd_args;
    const char * this_cmd_name = (index<0)?"not found":Cmd::cmds[index];
    cout << "Command: " << this_cmd_name << "\tArgs(" << num_args << "): ";
    for ( ; num_args > 0 ; num_args--){
      cout << this_cmd.cmd  << "\t";
      this_cmd=*this_cmd.next;
    }
    cout << endl;
    delete c;
  };
  printtest("add ;lkasjdf 123 3845706987 938475");
  printtest("touch");
  printtest("flush_all\r\n");
  printtest("get\tstuff more_stuff\r\n");
  printtest("version\r\n");
  printtest("laksjdfha");
  return true;
}
