#define KEY_SIZE 250
#define VAL_SIZE (1024*1024)
#include <string.h>
// A bucket is the basic storage unit
// The Bucket class stores the 250B key, the 1MB max data, and an id for the source.
// A bucket may be read multiple times if the hashing algorithm is bad.
// Since we don't want to return incorrect data, it is better for a read to be stale
// and to try another bucket than mind meld bad data.
// Writes need to be synchronized; reads do not.
// Jess pointed out that we might want an unsigned char, since there

class Bucket {

public:

  bool setVals(char * key, char * data);

  char * getKey() const {
    char * retval = new char[KEY_SIZE]; 
    return strcpy(retval, this->key);
  }

  char * getData() const { 
    char * retval = new char[VAL_SIZE];
    return strcpy(retval, this->data);
  }

  static bool test();

private:

  char key[KEY_SIZE+1] = {'\0'};
  char data[VAL_SIZE+1] = {'\0'};

};


