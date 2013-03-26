#ifndef PACKET_H
#define PACKET_H
#include "simplesocket.h"
#include "Bucket.h"
#include <functional>

class Packet{
 public:
  Packet(){};
  ~Packet(){};
  // this doesn't work unless I compile with -std=libc++, which breaks simplesocket
  std::function<void (simplesocket *)> transmission_handler; 
};

#endif
