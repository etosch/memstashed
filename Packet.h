#ifndef PACKET_H
#define PACKET_H
#include "simplesocket.h"
#include "Bucket.h"
#include <functional>

class Packet{
 public:
  Packet(){};
  ~Packet(){};
  std::function<int (simplesocket *)> transmission_handler; 
};

#endif
