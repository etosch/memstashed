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
  uint16_t getFlags(const char * key);
  bool keyExists(const char * key);
  static bool test();  
  static pthread_mutex_t uidLock;
  int del(const char * key);
  int replaceExptime(const char * key, int exptime);
  int current_items_stored();
  Bucket * getBucket(const char * key);
  
 private:
  static int total_items_stored;
  static int cmd_get; static int get_hits;
  static int cmd_set; static int set_hits;
  static int initialized;
  const static int size = DEFAULT_SIZE;
  static Bucket *stash[DEFAULT_SIZE];
  static uint64_t uid;
  static uint64_t newUid();
};

#endif
