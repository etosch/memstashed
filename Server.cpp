#include "Server.h"
#include "Parser.h"
#include "Cmd.h"
#include "errors.h"
#include "simplesocket.h"
#include <pthread.h>
#include <sstream>
#include <stdio.h>

using namespace std;

static int debug_mode;

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
  auto * client = (simplesocket *) cv; 
  // most messages are short
  int bufsize = 512;
  char buffer[bufsize];
  Cmd * c;
  char err[250];
  while (true) {
    int nbytesRead = client->read (buffer, bufsize);
    if (strcmp(buffer, "\r\n")){
      if (nbytesRead == 0) {
	cout << "No bytes read!" << endl;
	break;
      }
      try{
	c = Parser::parse_cmd(bufsize, buffer);
	cout << "cmd#:"<<c->cmd<<endl;
	Parser::parse(c, bufsize, buffer);
	c->exec_cmd();
      } catch (int e) {
	sprintf(err, errors[e], Cmd::cmds[c->cmd]); 
	//      strcpy(err, errors[e]);
	client->write(err, strlen(err));
      }
      if (debug_mode){
	cout << c->cmd << Cmd::cmds[c->cmd] << endl;
	client->write("\r\nmemstashed>", 13);
      }
    }
  }
  delete c;
  return NULL;
}
 
// formerly main
int Server::run (bool debug, char delim, char * ip_addr, char * protocol, 
	 int size, int max_connections, int udp_port, int tcp_port,
	 int max_threads, int verbose, int memory_chunk_multiplier,
	 int default_slab_page, int min_size, bool auto_free, 
	 bool daemon, bool stats, bool cas){
  debug_mode = debug;
  serversocket * s = new serversocket ((bool)tcp_port?tcp_port:udp_port);
  cout << "Entering loop.\n" << endl;
  while(true){
    // Create one thread per connection.
    auto * c = s->accept();
    pthread_t * t = new pthread_t;
    pthread_create (t, NULL, serve, (void *) c);
  }
  // We will never get here...
  s->close();
  return 0;
};
