#include "Bucket.h"
#include <stdio.h>
#include <iostream>
#include <cassert>
#include "replies.h"

int Bucket::bucket_lock(){
  return pthread_mutex_lock(&this->lock);
}

int Bucket::bucket_unlock(){
  return pthread_mutex_unlock(&this->lock);
}

bool Bucket::setVals(const char * newkey, char * newdata, int data_size
		     , uint16_t flags, int exptime, int uid) {
  bool retval = false;
  this->bucket_lock(); std::cout<<"lock bucket"<<std::endl;
  if (strlen(newdata)<=(VAL_SIZE)){
    strcpy(this->key, newkey);
    strcpy(this->data, newdata);
    this->flags = flags;
    this->exptime = exptime;
    this->data_size = data_size;
    this->bucket_uid = uid;
    retval=true;
  }
  this->bucket_unlock(); std::cout<<"unlock bucket"<<std::endl;
  return retval;
}

uint16_t Bucket::getFlags(){
  return this->flags;
}

bool Bucket::replaceExptime(const char * key, int exptime){
  bool outcome= false;
  this->bucket_lock();
  if (strcmp(this->key, key)==0){
    this->exptime=exptime;
    outcome = true;
  }
  this->bucket_unlock();
  return outcome;
}

bool Bucket::isEmpty(){
  return (this->key=='\0' || strcmp(this->key,"")==0)
    && (this->data=='\0' || strcmp(this->data,"")==0);
}

bool Bucket::test() {
  // Bucket basic functionality test suite
  // This will be called in a test file
  // initialize a bucket
  // std::cout << "Bucket test" << std::endl;
  // Bucket * b = new Bucket();
  // std::cout << "Empty?" << b->isEmpty();
  // char * uninitKey = b->getKey();
  // b->setVals("asdf","fdsa");
  // // determine whether we're actually copying to new address space
  // std::cout << b->getKey() << std::endl;
  return true;
}
