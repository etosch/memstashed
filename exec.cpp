#include "Cmd.h"
#include "Stash.h"
#include "errors.h"
#include <string>
#include <iostream>
#include "replies.h"
#include "simplesocket.h"
#include "Packet.h"
#include <stdlib.h>
// this macro brought to you by john "jazz" foley
#define show(var) std::cout << __FILE__ ":"<<__LINE__<<":" #var << " = " << var << std::endl; 

using namespace std;

Packet * Cmd::exec_storage_cmd(char * data_block, Stash * s){
  const char * key = this->args[0].c_str();
  uint16_t flags = atoi(this->args[1].c_str()); 
  int exptime = atol(this->args[2].c_str());
  uint64_t casunique; 
  bool noreply = this->args.back()=="noreply";
  if (this->cmd == 2) casunique = atol(this->args[4].c_str());
  int reply; show(this->cmd);
  switch (this->cmd){
  case 0 : { //add
    reply = s->set(key, data_block, this->blocksize, flags, exptime, false);
    break;
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
    break;
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
    break;
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
    break;
  }
  case 11 : { //replace
    reply = s->getDataSize(key);
    if (reply) reply = s->set(key, data_block, this->blocksize, flags, exptime, true);
    break;
  }
  case 12 : { //set
    reply = s->set(key, data_block, this->blocksize, flags, exptime, true);
    break;
  }
  default : {
    cout << "Woah, What happened Here?" << endl;
    exit(-1);
  }
  }
  Packet * p = new Packet();
  p->transmission_handler = [=] (simplesocket * sock) {
    sock->write(replies[reply], strlen(replies[reply]));
    return 0;
  };
  return p;
}

Packet * Cmd::exec_retrieval_cmd(Stash * s){
  Packet * p = new Packet();
  p->transmission_handler = [=] (simplesocket * sock) {
    for (int i = 0 ; i < this->args.size() ; i++ ) {       
      int reply = -1;
      const char * key = this->args[i].c_str(); show(key);
      Bucket * b = s->getBucket(key);
      if (strcmp(key, b->getKey())) continue;
      int blocksize = b->data_size;
      char data[blocksize];
      try {
	memcpy(data, b->getData(), blocksize);
	show(data);
      } catch (int e) {
	reply = e;
      }    
      char reply_str[512]; show(reply); show(NOT_FOUND);
      if (NOT_FOUND!=reply){ show(this->cmd);
	if (this->cmd==7) 
	  sprintf(reply_str, "VALUE %s %i %i %i\r\n", b->getKey(), b->getFlags(), blocksize, b->bucket_uid);
	else
	  sprintf(reply_str, "VALUE %s %i %i\r\n", b->getKey(), b->getFlags(), blocksize);
      }
      show(reply_str);
      int reply_len = strlen(reply_str); 
      sock->write(reply_str, reply_len);
      sock->write(data, blocksize);
      sock->write("\r\n", 2);
    }
    sock->write(replies[END], strlen(replies[END]));
    return 0;
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
    return 0;
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
    return 0;
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
    return 0;
  };
  return p;
}

Packet * Cmd::exec_flush_cmd(Stash * s){
  Packet * p = new Packet();
  p->transmission_handler = [=] (simplesocket * sock) {
    sock->write("Not implemented\r\n", 17);
    return 0;
  };
  return p;
}

Packet * Cmd::exec_singleton(Stash * s){
  Packet * p = new Packet(); show(this->cmd);
  switch (this->cmd) { 
  case 17 : {
    p->transmission_handler = [=] (simplesocket * sock) {      
      sock->write("VERSION 1.never\r\n", 17);
      return 0;
    };
    return p;
  }
  case 10 : {
    p->transmission_handler = [=] (simplesocket * sock) {
      sock->write("GOODBYE\r\n", 9);
      return EXIT;
    };
    return p;
  }
  default : return p;
  }
}

Packet * Cmd::exec_verbosity(Stash * s){
  Packet * p = new Packet();
  return p;
}

Packet * Cmd::exec_cmd(char * buffer, Stash * s){
  show(Cmd::parse_types[this->cmd]);
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
