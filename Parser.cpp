// ugh, lexing. only doing this because bison is apparently only for c/c++ gurus
// protocol can be found here : https://github.com/memcached/memcached/blob/master/doc/protocol.txt
#include "Parser.h"
#include "Bucket.h"
#include "errors.h"
#include <iostream>
#include <functional>
#define NUM_CMDS 18

using namespace std;

const char * Cmd::cmds[NUM_CMDS] = {"add","append","cas","decr","delete"
				    ,"flush_all","get","gets","incr","prepend"
				    ,"quit","replace","set","slabs","stats"
				    ,"touch","verbosity", "version"};

const int Cmd::parse_types[NUM_CMDS] = {1,1,1,4,3,8,2,2,4,1,9,1,1,6,7,5,10,9};

auto advance_fn = [] (int &i, int bufsize, char buffer[]) {
  return [&i,bufsize,buffer] ()->int {
    do{ 
      if (!(int)buffer[i] || (buffer[i]=='\r' && buffer[i+1]=='\n')) 
	return 0;
      else i++; 
    } while ( i<bufsize && (int)buffer[i]<33 );
    return 1;    
  };
};

auto read_fn = [] (int &i, int bufsize, char buffer[]){
  return [&i,bufsize,buffer] (char target[], int bound, void (*err_fn)()) -> void{
    int j = 0;
    for( ; (int)buffer[i]>32 ; j++, i++){
      if (j>bound) err_fn();
      if (i==bufsize) buffer_size_err();
      target[j]=buffer[i];
    }
    if (j==0) cmd_parse_err();
    target[j]='\0';
  };
};

void Parser::parse_storage_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char flags[6]; char exptime[11]; char bytes[11]; 
  char casunique[21]; char noreply[8] = "";
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  if (advance()) read(key, KEY_SIZE, key_size_err); else cmd_parse_err();
  if (advance()) read(flags, 5, flag_size_err); else cmd_parse_err();
  if (advance()) read(exptime, 10, exptime_size_err); else cmd_parse_err();
  if (advance()) read(bytes, 10, datasize_size_err); else cmd_parse_err();
  if (c->cmd == 2){
    if (advance()) 
      read(casunique, 19, casunique_size_err); 
    else cmd_parse_err();
  }
  if (advance()) read(noreply, 7, noreply_size_err); 
  c->add(key); c->add(flags); c->add(exptime); c->blocksize=atoi(bytes);
  if (c->cmd == 2) c->add(casunique);
  if (strcmp(noreply, "")) c->add(noreply);
}

void Parser::parse_retrieval_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  auto advance = advance_fn(i, bufsize, buffer);
  auto read_key = read_fn(i, bufsize, buffer);
  while (advance()) {
    char key[KEY_SIZE+1];
    read_key(key, KEY_SIZE, key_size_err);
    c->add(key);
  }
}

void Parser::parse_deletion_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  if (advance()) read(key, KEY_SIZE, key_size_err); else cmd_parse_err();
  c->add(key);
  if (advance()) {
    read(noreply, 7, noreply_size_err);
    c->add(noreply);
  }
}

void Parser::parse_cr_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char value[21]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  if (advance()) read(key, KEY_SIZE, key_size_err); else cmd_parse_err();
  c->add(key);
  if (advance()) read(value, 20, cr_size_err); else cmd_parse_err();
  c->add(value);
  if (advance()) {
    read(noreply, 7, noreply_size_err);
    c->add(noreply);
  }
}

void Parser::parse_touch_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char key[KEY_SIZE+1]; char exptime[11]; char noreply[8];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  if (advance()) read(key, KEY_SIZE, key_size_err); else cmd_parse_err();
  c->add(key);
  if (advance()) read(exptime, 10, exptime_size_err); else cmd_parse_err();
  c->add(exptime);
  if (advance()) {
    read(noreply, 7, noreply_size_err);
    c->add(noreply);
  }
}

void Parser::parse_slabs_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  char slab_cmd[11];
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  if (advance()) read(slab_cmd, 10, slab_size_err); else cmd_parse_err();
  c->add(slab_cmd);
  if (strcmp(slab_cmd, "reassign")==0){
    char src[21]; char dest[21];
    if (advance()){
      read(src, 20, slab_src_size_err); 
      c->add(src);
    } else cmd_parse_err();
    if (advance()) {
      read(dest, 20, slab_dest_size_err); 
      c->add(dest);
    } else cmd_parse_err();
  } else if (strcmp(slab_cmd, "automove")==0){
    char indicator[2];
    if (advance()) {
      read(indicator, 1, indicator_size_err); 
      c->add(indicator);
    } else cmd_parse_err();
  } else unrecognized_slab_cmd();
}

void Parser::parse_stats_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  auto advance = advance_fn(i, bufsize, buffer);
  if (advance()) stats_args_err();
}

void Parser::parse_flush_cmd(Cmd * c, int bufsize, char buffer[]){
  int i = strlen(Cmd::cmds[c->cmd]); 
  auto advance = advance_fn(i, bufsize, buffer);
  auto read = read_fn(i, bufsize, buffer);
  if (advance()){
    char optarg[21];
    read(optarg, 20, flush_args_err);
    c->add(optarg);
  }
}

void Parser::parse_singleton(Cmd * c, int bufsize, char buffer[]){
}

void Parser::parse_verbosity(Cmd * c, int bufsize, char buffer[]){
  verbosity_err();
}

void Parser::parse(Cmd * c, int bufsize, char buffer[]){
  //  Cmd * c = Parser::parse_cmd(bufsize, buffer);
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
  default: bad_cmd_err();
  }
  cout << "This should never happen!" << endl;
  exit(-1);
}

Cmd * Parser::parse_cmd(int bufsize, char buffer[]){
  int i = 0; char cmd_str[10]; 
  read_fn(i,bufsize,buffer)(cmd_str,9,bad_cmd_err);
  for (int j = 0; j < NUM_CMDS ; j++) {
    if (strcmp(cmd_str, Cmd::cmds[j])==0)
      return new Cmd(j);
  }
  bad_cmd_err();
  cout << "Danger Will Robinson! You should never get here." <<endl;
  exit(-1);
}


bool Parser::test(){
 auto printtest = [] (char s[]) {
    Cmd * c = Parser::parse_cmd(512, s);
    int index = c->cmd;
    cout << "cmd#:"<<index;
    Parser::parse(c, 512, s);
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
