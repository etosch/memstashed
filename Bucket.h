#ifndef BUCKET_H
#define BUCKET_H
#define KEY_SIZE 250
#define VAL_SIZE (1024*1024)
#include <string.h>
#include <pthread.h>
#include <errors.h>
// A bucket is the basic storage unit
// The Bucket class stores the 250B key, the 1MB max data, and an id for the source.
// A bucket may be read multiple times if the hashing algorithm is bad.
// Since we don't want to return incorrect data, it is better for a read to be stale
// and to try another bucket than mind meld bad data.
// Writes need to be synchronized; reads do not.
// Jess pointed out that we might want an unsigned char, since there

class Bucket {

public:

  Bucket(){
    if (pthread_mutex_init(&lock, NULL))
      bucket_lock_err();
  };

  ~Bucket(){
    pthread_mutex_destroy(&lock);
  };

  bool setVals(const char * key, char * data, int data_size
	       , uint16_t flags, int exptime, int uid);

  bool replaceExptime(const char * key, int exptime);

  bool isEmpty();

  char * getKey() const {
    char * retval = new char[KEY_SIZE]; 
    return strcpy(retval, this->key);
  }

  char * getData() const { 
    char * retval = new char[VAL_SIZE];
    return strcpy(retval, this->data);
  }

  uint16_t getFlags(const char * key);

  static bool test();
  int bucket_uid = 0;
  int data_size;


private:
  pthread_mutex_t lock;
  int bucket_lock();
  int bucket_unlock();
  char key[KEY_SIZE+1] = {'\0'};
  uint16_t flags;
  int exptime;
  char data[VAL_SIZE+1] = {'\0'};
};
#endif
