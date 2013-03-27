#include "Stash.h"
#include "Bucket.h"
#include <string>
#include <stdio.h>
#include "replies.h"
// The actual hash table for storing values
// This will be implemented as an array
// How to determine the size of the array? - have some pluggable component 
// Recently accessed Buckets will be cached
// According to Wikipedia, L2 cache ~1MB

int Stash::initialized = 0;
Bucket *Stash::stash[];
pthread_mutex_t Stash::uidLock = PTHREAD_MUTEX_INITIALIZER;
int Stash::total_items_stored = 0;
int Stash::cmd_get = 0;
int Stash::get_hits = 0;
int Stash::cmd_set = 0;
int Stash::set_hits = 0;
uint64_t Stash::uid = -1;

// set array size
int Stash::init() {
  // the policy for this will be set later
  // for now I'm hard-coding the size
  if (not Stash::initialized) {
    for (int i=0; i< Stash::size; i++){
      Bucket * bp = new Bucket();
      Stash::stash[i] = bp;
    }
    Stash::initialized = 1;
  }
  return Stash::size;
}

uint16_t Stash::getFlags(const char * key) {
  if (this->keyExists(key)){
    Bucket * b = this->getBucket(key);
    return b->getFlags();
  } else throw NOT_FOUND;
}

Bucket * Stash::getBucket(const char * key){
  int index = Stash::hash(key);
  return this->stash[index];
}

int Stash::del(const char * key){
  if (this->keyExists(key)) {
    int index = Stash::hash(key);
    Bucket * bp = stash[index];
    this->stash[index] = new Bucket();
    delete bp;
    return DELETED;
  } else return NOT_FOUND;
}


const char * Stash::get(const char * key) {
  // hashing done outside
  Stash::cmd_get++;
  Bucket * b  = getBucket(key);
  if (strcmp(b->getKey(), key)==0){
    Stash::get_hits++;
    return b->getData();
  } else throw NOT_FOUND;
}

int Stash::set(const char * key, char * val, int size, uint16_t flags
	       , int exptime, int append = false) {
  Bucket * b = getBucket(key);
  char * k9 = b->getKey();
  Stash::cmd_set++;
  if (strcmp(k9,key)==0 && not append) //already set
    return NOT_STORED; 
  else if (k9 == '\0' || strcmp(k9, "")==0) { // empty
    b->setVals(key, val, size, flags, exptime, Stash::newUid());
    total_items_stored++;
    set_hits++;
    return STORED;
  } else {
    //evict someone
    // just replace for now
    b->setVals(key, val, size, flags, exptime, Stash::newUid());
    total_items_stored++;
    set_hits++;
    return STORED;
  }
}

int Stash::current_items_stored(){
  int num_full_buckets = 0;
  for(int i=0; i<Stash::size; i++)
    if (not (stash[i]->isEmpty()))
      num_full_buckets += 1;
  return num_full_buckets;
}

float Stash::capacity(){
  //returns percent capacity
  return float(Stash::current_items_stored())/float(Stash::size);
}

uint64_t Stash::newUid(){
  pthread_mutex_lock(&uidLock);
  int top = uid;
  uid++;
  pthread_mutex_unlock(&uidLock);
  return top;
}

int Stash::getUid(const char * key){
  return Stash::getBucket(key)->bucket_uid;
}

int Stash::replaceExptime(const char * key, int exptime){
  if (this->keyExists(key)){
    Bucket * b = Stash::getBucket(key);
    b->replaceExptime(key, exptime);
    return TOUCHED;
  } else return NOT_FOUND;
}

bool Stash::keyExists(const char * key){
  Bucket * b = Stash::getBucket(key);
  return strcmp(b->getKey(), key)==0;
}

int Stash::getDataSize(const char * key){
  Bucket * b = Stash::getBucket(key);
  return b->data_size;
}

bool Stash::test() {
  // printf("Stash test\n");
  // Stash *s = new Stash();
  // printf("stash size:%d\n", s->init());
  // if(s->init()){
  //   printf("set:%d\t%0.4f\n", s->set("asdf","fdsa"), s->capacity());
  //   //same key, but different val - still doesn't set:
  //   printf("set:%d\t%0.4f\n", s->set("asdf","Fdsa"),  s->capacity());
  //   printf("get:%s\t%0.4f\n", s->get("asdf"),  s->capacity());
  //   printf("set:%d\t%0.4f\n", s->set("qwer","Rewq"),  s->capacity());
  //   printf("set:%d\t%0.4f\n", s->set("1234","4321"),  s->capacity());
  //   printf("get:%s\t%0.4f\n", s->get("asdf"),  s->capacity());
  //   printf("set:%d\t%0.4f\n", s->set("ert","Tre"),  s->capacity());
  // } else return false;
  return true;
}
