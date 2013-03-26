#include "Cmd.h"
#include "Stash.h"
#include "errors.h"
#include <string>
#include <iostream>
#include "replies.h"

using namespace std;

int Cmd::exec_add(const char * key, uint16_t flags, int exptime
		  , bool noreply, char * data_block, Stash * s){
  int reply = s->set(key, data_block, this->blocksize, flags, exptime, false);
  if (noreply)
    return noreply;
  else return reply;
}

int Cmd::exec_append(const char * key, uint16_t flags, int exptime
		     , bool noreply, char * data_block, Stash * s){
  int blocksize = this->blocksize;
  const char * olddata = s->get(key);
  int olddata_size = s->getDataSize(key);
  char newdata[olddata_size+blocksize];
  for (int i = 0 ; i < olddata_size ; i++)
    newdata[i]=olddata[i]; 
  for (int i = 0 ; i < blocksize ; i ++)
    newdata[olddata_size+i]=data_block[i];
  int reply = s->set(key, newdata, olddata_size+blocksize, flags, exptime, true);
  if (noreply)
    return noreply;
  else return reply;
}

int Cmd::exec_cas(const char * key, uint16_t flags, int exptime
		  , uint64_t casunique, bool noreply, char * data_block, Stash * s){
  int uid = s->getUid(key);
  int reply;
  if (casunique==uid)
    reply = EXISTS;
  else {
    if (s->keyExists(key)){
      s->set(key, data_block, this->blocksize, flags, exptime, true);
      reply=STORED;
    } else reply=NOT_FOUND;
  }
  if (noreply) 
    return noreply;
  else return reply;
}

int Cmd::exec_prepend(const char * key, uint16_t flags, int exptime
		      , bool noreply, char * data_block, Stash * s){
  int blocksize = this->blocksize;
  const char * olddata = s->get(key);
  int olddata_size = s->getDataSize(key);
  char newdata[olddata_size+blocksize];
  for (int i = 0 ; i < blocksize ; i ++)
    newdata[i]=data_block[i];
  for (int i = 0 ; i < olddata_size ; i++)
    newdata[blocksize+i]=olddata[i]; 
  int reply = s->set(key, newdata, olddata_size+blocksize, flags, exptime, true);
  if (noreply)
    return noreply;
  else return reply;
}

int Cmd::exec_replace(const char * key, uint16_t flags, int exptime
		      , bool noreply, char * data_block, Stash * s){
  int reply = s->getDataSize(key);
  if (reply) reply = s->set(key, data_block, this->blocksize, flags, exptime, true);
  if (noreply)
    return noreply;
  else return reply;
}

int Cmd::exec_storage_cmd(char * data_block, Stash * s){
  string key = this->args[0];
  uint16_t flags = atoi(this->args[1].c_str()); //stoi not working - may want to change storage?
  int exptime = atol(this->args[2].c_str());
  uint64_t casunique; 
  bool noreply = false;
  if (this->cmd == 2) casunique = atol(this->args[4].c_str());
  if (this->args.back()=="noreply") noreply=true;
  switch (this->cmd){
  case 0 : return exec_add(key.c_str(), flags, exptime, noreply, data_block, s);
  case 1 : return exec_append(key.c_str(), flags, exptime, noreply, data_block, s);
  case 2 : return exec_cas(key.c_str(), flags, exptime, casunique, noreply, data_block, s);
  case 9 : return exec_prepend(key.c_str(), flags, exptime, noreply, data_block, s);
  case 11 : return exec_replace(key.c_str(), flags, exptime, noreply, data_block, s);
  default : return -1;
  }
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

int Cmd::exec_cmd(char * buffer, Stash * s){
  // this should be moved
  switch (Cmd::parse_types[this->cmd]) {
  case 1 : return this->exec_storage_cmd(buffer, s);
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
