#ifndef STASH_H
#define STASH_H

#include "Bucket.h"
#define DEFAULT_SIZE 3

// stash is our assocaitive array

class Stash {

 public:
  Stash(){};
  ~Stash(){};
  int init();
  const char * get(const char * key);
  int set(const char * key, char * val, int size
	  , uint16_t flags, int exptime, int append);
  static int hash(const char * key){
    return (int) *key % DEFAULT_SIZE;
  };
  float capacity();
  int getUid(const char * key);
  int getDataSize(const char * key);
  bool keyExists(const char * key);
  static bool test();  
  static pthread_mutex_t uidLock;
  
 private:
  int y =0;
  static int initialized;
  const static int size = DEFAULT_SIZE;
  static Bucket *stash[DEFAULT_SIZE];
  static uint64_t uid;
  static uint64_t newUid();
  Bucket * getBucket(const char * key);
};

#endif
