#include "Server.h"
#include "simplesocket.h"
#include <pthread.h>
#include <sstream>
#include <iostream>

using namespace std;

// from emery's code
int Server::messageNumber () {
  // like a ref box in ml
  static int number = 0;
  static pthread_mutex_t numberLock = PTHREAD_MUTEX_INITIALIZER;
  int v;
  pthread_mutex_lock (&numberLock); // why are we locking the address?
  v = number; 
  number++;
  pthread_mutex_unlock (&numberLock);
  return v;
}

//so is void * a shortcut for having to write char * and returning '\0'?
void * Server::serve (void * cv)
{
  // Just read exactly one string.
  auto * c = (simplesocket *) cv; 
  char buffer[256];
  while (true) {
    int nbytesRead = c->read (buffer, 256);
    if (nbytesRead == 0) {
      break;
    }
    c->write(buffer, 256);
    cout << "[" << messageNumber() << "] " << buffer << endl;
  }
  delete c;
  return NULL;
}
 
// formerly main
int Server::run (char delim, char * ip_addr, char * protocol, 
	 int size, int max_connections, int udp_port, int tcp_port,
	 int max_threads, int verbose, int memory_chunk_multiplier,
	 int default_slab_page, int min_size, bool auto_free, 
	 bool daemon, bool stats, bool cas){
//   if (argc < 2) {
//     cerr << "Usage: " << argv[0] << " <port>" << endl;
//     exit (-1);
//   }
//  int port;
//  stringstream (argv[1]) >> port;
  serversocket * s = new serversocket ((bool)tcp_port?tcp_port:udp_port);
  cout << "Entering loop.\n" << endl;
  while (true) {
    // Create one thread per connection.
    auto * c = s->accept();
    pthread_t * t = new pthread_t;
    pthread_create (t, NULL, serve, (void *) c);
  }
  // We will never get here...
  s->close();
  return 0;
};
