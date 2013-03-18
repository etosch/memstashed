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
      //      delete bp;
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
  if (k9 == key) //already set
    return 1; 
  else if (k9 == '\0') { // empty
    b->setVals(key, val);
    return 2;
  } else {
    //evict someone
    // just replace for now
    b->setVals(key, val);
    return 3;
  }
}

bool Stash::test() {
  printf("Stash::test\n");
  Stash *s = new Stash();
  printf("%d\n", s->init());
  if(s->init()){
    printf("Fuck me.\n");
    s->set("asdf","fdsa");
    printf("Fuck you.\n");
    printf("get:%s\n", s->get("asdf"));
  } else return false;
  return true;
}
