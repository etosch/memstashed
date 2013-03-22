#include "Server.h"
#include "Parser.h"
// Parser.h includes Cmd.h
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
void * Server::serve (void * cv){
  // Just read exactly one string.
  auto * c = (simplesocket *) cv; 
  // most messages are short
  int bufsize = 512;
  char buffer[bufsize];
  while (true) {
    int nbytesRead = c->read (buffer, bufsize);
    if (nbytesRead == 0) {
      cout << "No bytes read!" << endl;
      break;
    }
    if ((int) buffer[bufsize-1] == 0) {
      Cmd * c = Parser::parse(bufsize, buffer);
      cout << "Command: " << Cmd::cmds[c->cmd] << endl;
      c->exec();
    } else {
      printf("Messages greater than buffer size (%iB) not yet supported.", bufsize);
      exit(1);
    }
    c->write(buffer, bufsize);
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
  serversocket * s = new serversocket ((bool)tcp_port?tcp_port:udp_port);
  cout << "Entering loop.\n" << endl;
  while (messageNumber()<10) {
    // Create one thread per connection.
    auto * c = s->accept();
    pthread_t * t = new pthread_t;
    pthread_create (t, NULL, serve, (void *) c);
  }
  // We will never get here...
  s->close();
  return 0;
};
