#include "Stash.h"
// Stash.h includes Bucket.h
// Bucket.h includes <string.h>
#include <stdio.h>
// The actual hash table for storing values
// This will be implemented as an array
// How to determine the size of the array? - have some pluggable component 
// Recently accessed Buckets will be cached
// According to Wikipedia, L2 cache ~1MB

int Stash::initialized = 0;
Bucket *Stash::stash[];

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

const char * Stash::get(char * key) {
  // hashing done outside
  int index = Stash::hash(key);
  Bucket * b = stash[index];
  if (strcmp(b->getKey(), key)==0){
    return b->getData();
  } else return "Balls!\n";
}

int Stash::set(char * key, char * val) {
  int index = Stash::hash(key);
  Bucket * b = stash[index];
  char * k9 = b->getKey();
  if (strcmp(k9,key)==0) //already set
    return 1; 
  else if (k9 == '\0' || strcmp(k9, "")==0) { // empty
    b->setVals(key, val);
    return 2;
  } else {
    //evict someone
    // just replace for now
    b->setVals(key, val);
    return 3;
  }
}

float Stash::capacity(){
  //returns percent capacity
  int num_full_buckets = 0;
  for(int i=0; i<Stash::size; i++)
    if (not (stash[i]->isEmpty()))
      num_full_buckets += 1;
  return float(num_full_buckets)/float(Stash::size);
}



bool Stash::test() {
  printf("Stash test\n");
  Stash *s = new Stash();
  printf("stash size:%d\n", s->init());
  if(s->init()){
    printf("set:%d\t%0.4f\n", s->set("asdf","fdsa"), s->capacity());
    //same key, but different val - still doesn't set:
    printf("set:%d\t%0.4f\n", s->set("asdf","Fdsa"),  s->capacity());
    printf("get:%s\t%0.4f\n", s->get("asdf"),  s->capacity());
    printf("set:%d\t%0.4f\n", s->set("qwer","Rewq"),  s->capacity());
    printf("set:%d\t%0.4f\n", s->set("1234","4321"),  s->capacity());
    printf("get:%s\t%0.4f\n", s->get("asdf"),  s->capacity());
    printf("set:%d\t%0.4f\n", s->set("ert","Tre"),  s->capacity());
  } else return false;
  return true;
}
