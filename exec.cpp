#include "Cmd.h"
#include "Stash.h"
#include "errors.h"
#include <string>
#include <iostream>
#include "replies.h"
#include "simplesocket.h"
#include "Packet.h"
#include <stdlib.h>

using namespace std;

Packet * Cmd::exec_storage_cmd(char * data_block, Stash * s){
  const char * key = this->args[0].c_str();
  uint16_t flags = atoi(this->args[1].c_str()); //stoi not working - may want to change storage?
  int exptime = atol(this->args[2].c_str());
  uint64_t casunique; 
  bool noreply = this->args.back()=="noreply";
  if (this->cmd == 2) casunique = atol(this->args[4].c_str());
  int reply;
  switch (this->cmd){
  case 0 : { //add
    reply = s->set(key, data_block, this->blocksize, flags, exptime, false);
  }
  case 1 : { //append
    int blocksize = this->blocksize;
    const char * olddata = s->get(key);
    int olddata_size = s->getDataSize(key);
    char newdata[olddata_size+blocksize];
    for (int i = 0 ; i < olddata_size ; i++)
      newdata[i]=olddata[i]; 
    for (int i = 0 ; i < blocksize ; i ++)
      newdata[olddata_size+i]=data_block[i];
    reply = s->set(key, newdata, olddata_size+blocksize, flags, exptime, true);
  }
  case 2 : { //cas
    int uid = s->getUid(key);
    if (casunique==uid)
      reply = EXISTS;
    else {
      if (s->keyExists(key)){
	s->set(key, data_block, this->blocksize, flags, exptime, true);
	reply=STORED;
      } else reply=NOT_FOUND;
    }
  }
  case 9 : { //prepend
    int blocksize = this->blocksize;
    const char * olddata = s->get(key);
    int olddata_size = s->getDataSize(key);
    char newdata[olddata_size+blocksize];
    for (int i = 0 ; i < blocksize ; i ++)
      newdata[i]=data_block[i];
    for (int i = 0 ; i < olddata_size ; i++)
      newdata[blocksize+i]=olddata[i]; 
    reply = s->set(key, newdata, olddata_size+blocksize, flags, exptime, true);
  }
  case 11 : { //replace
    reply = s->getDataSize(key);
    if (reply) reply = s->set(key, data_block, this->blocksize, flags, exptime, true);
  }
  default : {
    cout << "Woah, What happened Here?" << endl;
    exit(-1);
  }
  }
  Packet * p = new Packet();
  p->transmission_handler = [=] (simplesocket * sock) {
    char * reply_str;
    sprintf(reply_str, "%s\r\n", replies[reply]);
    sock->write(reply_str, strlen(reply_str));
  };
  return p;
}

Packet * Cmd::exec_retrieval_cmd(Stash * s){
  Packet * p = new Packet();
  p->transmission_handler = [=] (simplesocket * sock) {
    for (int i = 0 ; i < this->args.size() ; i++ ) { 
      int reply = -1;
      const char * key = this->args[i].c_str();
      int blocksize = s->getDataSize(key);
      int flags = s->getFlags(key);
      int casunique = s->getUid(key);
      char data[blocksize];
      try {
	memcpy(data, s->get(key), blocksize);
	data[blocksize-2]='\r'; data[blocksize-2]='\n';
      } catch (int e) {
	reply = e;
      }    
      char * reply_str ="";
      if (NOT_FOUND!=reply){
	if (this->cmd==7) {
	  reply_str = "VALUE %s %i %i %i\r\n";
	  sprintf(reply_str, key, flags, blocksize, casunique);
	} else {
	  reply_str = "VALUE %s %i %i\r\n";
	  sprintf(reply_str, key, flags, blocksize);
	}
      }
      int reply_len = strlen(reply_str);
      sock->write(reply_str, reply_len);
      sock->write(data, blocksize);
    }
    sock->write(replies[END], strlen(replies[END]));
  };
  return p;
}

Packet * Cmd::exec_deletion_cmd(Stash * s){
  Packet * p = new Packet();
  bool noreply = this->args.back()=="noreply";
  p->transmission_handler = [=] (simplesocket * sock) {
    const char * key = this->args[0].c_str();
    int reply = s->del(key);
    if (not noreply) sock->write(replies[reply], strlen(replies[reply]));
  };
  return p;
}

Packet * Cmd::exec_cr_cmd(Stash * s){
  Packet * p = new Packet();
  not_implemented_err();
  return p;
}

Packet * Cmd::exec_touch_cmd(Stash * s){
  Packet * p = new Packet();
  p->transmission_handler = [=] (simplesocket * sock) {
    const char * key = this->args[0].c_str();
    int exptime = atoi(this->args[1].c_str());
    int reply = s->replaceExptime(key, exptime);
    int noreply = this->args.back()=="noreply";
    if (not noreply)
      sock->write(replies[reply], strlen(replies[reply]));
  };
  return p;
}

Packet * Cmd::exec_slabs_cmd(Stash * s){
  Packet * p = new Packet();
  fuck_you();
  return p;
}

Packet * Cmd::exec_stats_cmd(Stash * s){
  Packet * p = new Packet ();
  p->transmission_handler = [=] (simplesocket * sock) {
    // char * out_str = "STAT %s %s\r\n";
    // sprintf(out_str, "pid", get_process_id());
    // sock->write(s, strlen(out_str));
  };
  return p;
}

Packet * Cmd::exec_flush_cmd(Stash * s){
  Packet * p = new Packet();
  return p;
}

Packet * Cmd::exec_singleton(Stash * s){
  Packet * p = new Packet();
  return p;
}

Packet * Cmd::exec_verbosity(Stash * s){
  Packet * p = new Packet();
  return p;
}

Packet * Cmd::exec_cmd(char * buffer, Stash * s){
  // this should be moved
  switch (Cmd::parse_types[this->cmd]) {
  case 1 : return this->exec_storage_cmd(buffer, s);
  case 2 : return this->exec_retrieval_cmd(s);
  case 3 : return this->exec_deletion_cmd(s);
  case 4 : return this->exec_cr_cmd(s);
  case 5 : return this->exec_touch_cmd(s);
  case 6 : return this->exec_slabs_cmd(s);
  case 7 : return this->exec_stats_cmd(s);
  case 8 : return this->exec_flush_cmd(s);
  case 9 : return this->exec_singleton(s);
  case 10 : return this->exec_verbosity(s);
  default : bad_cmd_err();
  }
  cout << "should never get here" << endl;
  return new Packet();
}
