#include "Bucket.h"
// Bucket.h includes <string.h>
#define DEFAULT_SIZE 3

// stash is our assocaitive array

class Stash {

public:
  int init();
  const char * get(char * key);
  int set(char * key, char * val);
  int hash(char * key){
    return (int) *key % DEFAULT_SIZE;
  };
  
  static bool test();  
  
 private:
  static int initialized;
  const static int size = DEFAULT_SIZE;
  static Bucket *stash[DEFAULT_SIZE];
  
};
