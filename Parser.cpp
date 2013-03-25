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

Cmd * Parser::parse_storage_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char flags[6]; char exptime[11]; char bytes[11]; 
  char casunique[21]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  advance()?read(key, KEY_SIZE, "Key too big!"):cmd_parse_err(c);
  advance()?read(flags, 5, "32-bit flags not supported."):cmd_parse_err(c);
  advance()?read(exptime, 10, "System time must fit in an int."):cmd_parse_err(c);
  advance()?read(bytes, 10, "Size of data block must fit in an int."):cmd_parse_err(c);
  if (c->cmd == 2){
    advance()?read(casunique, 19, "Unique value must fit in a 64-bit int."):cmd_parse_err(c);}
  if ((int)buffer[i]>32) { 
    advance()?read(noreply, 7, "To use noreply optional argument, type \"noreply\""):cmd_parse_err(c);}
  c->add(key); c->add(flags); c->add(exptime); c->add(bytes);
  if (c->cmd == 2) c->add(casunique);
  return c;
}

Cmd * Parser::parse_retrieval_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  auto advance = advance_fn(i, bufsize, buffer);
  auto read_key = read_fn(i, bufsize, buffer);
  while (advance()) {
    char key[KEY_SIZE+1];
    read_key(key, KEY_SIZE, "Key exceeds size of 250B");
    c->add(key);
  }
  return c;
}

Cmd * Parser::parse_deletion_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  advance()?read(key, KEY_SIZE, "Key exceeds size of 250B"):cmd_parse_err(c);
  c->add(key);
  if (advance()) {
    read(noreply, 7, "To use noreply optional argument, type \"noreply\"");
    c->add(noreply);
  }
  return c;
}

Cmd * Parser::parse_cr_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char value[21]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  advance()?read(key, KEY_SIZE, "Key exceeds size of 250B"):cmd_parse_err(c);
  c->add(key);
  advance()?read(value, 20, "Value should be the decimal rep. of a 64-bit unsigned int."):cmd_parse_err(c);
  c->add(value);
  if (advance()) {
    read(noreply, 7, "To use noreply optional argument, type \"noreply\"");
    c->add(noreply);
  }
  return c;
}
Cmd * Parser::parse_touch_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char exptime[11]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  advance()?read(key, KEY_SIZE, "Key exceeds size of 250B"):cmd_parse_err(c);
  c->add(key);
  advance()?read(exptime, 10, "System time must fit in an int."):cmd_parse_err(c);
  c->add(exptime);
  if (advance()) {
    read(noreply, 7, "To use noreply optional argument, type \"noreply\"");
    c->add(noreply);
  }
  return c;
}
Cmd * Parser::parse_slabs_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char slab_cmd[11];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  advance()?read(slab_cmd, 10, ""):cmd_parse_err(c);
  c->add(slab_cmd);
  if (strcmp(slab_cmd, "reassign")==0){
    char src[21]; char dest[21];
    advance()?read(src, 20, "Slab src id number doesn't fit"):cmd_parse_err(c);
    c->add(src);
    advance()?read(dest, 20, "Slab dest id number doesn't fit"):cmd_parse_err(c);
    c->add(dest);
    return c;
  }else if (strcmp(slab_cmd, "automove")==0){
    char indicator[2];
    advance()?read(indicator, 1, "Indicator should be a single value from 0,1,2"):cmd_parse_err(c);
    c->add(indicator);
    return c;
  }    
  cout << "Bad args to slabs command" << endl;
  exit(-1);
}

Cmd * Parser::parse_stats_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  auto advance = advance_fn(i, bufsize, buffer);
  if (advance()){
    cout << "WARNING: stats with args not implemented" << endl;
  }
  return c;
}
Cmd * Parser::parse_flush_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  if (advance()){
    char optarg[21];
    read(optarg, 20, "Something meaningful.");
    c->add(optarg);
  }
  return c;
}
Cmd * Parser::parse_singleton(Cmd * c, int bufsize, char buffer[]){
  return c;
}
Cmd * Parser::parse_verbosity(Cmd * c, int bufsize, char buffer[]){
  cout << "Logging not implemented." <<endl;

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
    const char * this_cmd_name = (index<0)?"not found":Cmd::cmds[index];
    cout << "Command: " << this_cmd_name << "\tArgs(" << c->args.size() << "): ";
    for (string s : c->args){
      cout << s << "\t";
    }
    cout << endl;
    delete c;
  };
  printtest("add ;lkasjdf 123 3845706987 938475");
  printtest("delete lkajsdhlfkajshlkjrhalkejrht\r\n");
  printtest("incr lajksdhflajksl3i4h5p9384jlqihw4r;oiwe 34567876");
  printtest("touch jio345ijo534jio354 97979797");
  printtest("slabs reassign 34534 3453453");
  printtest("slabs automove 1");
  printtest("stats");
  printtest("flush_all\r\n");
  printtest("get\tstuff more_stuff\r\n");
  printtest("version\r\n");
  printtest("laksjdfha");
  return true;
}
