#include "Bucket.h"
// Bucket.h includes <string.h>
#include <stdio.h>
#include <iostream>
#include <cassert>


bool Bucket::setVals(char * newkey, char * newdata) {
  assert(strlen(newkey)<=KEY_SIZE);
  if (strlen(newdata)>(VAL_SIZE)) {
    return false;
  } else {
    strcpy(this->key, newkey);
    strcpy(this->data, newdata);
    // maybe I should check that these are properly copied before returning?
    return true;
  }
}

bool Bucket::test() {
  // Bucket basic functionality test suite
  // This will be called in a test file
  // initialize a bucket
  Bucket * b = new Bucket();
  char * uninitKey = b->getKey();
  b->setVals("asdf","fdsa");
  // determine whether we're actually copying to new address space
  std::cout << b->getKey() << std::endl;
  return true;
}
