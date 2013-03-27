#include "Packet.h"
#include "Server.h"
#include "Stash.h"
#include "Parser.h"
#include "Cmd.h"
#include "errors.h"
#include "replies.h"
#include "simplesocket.h"
#include <pthread.h>
#include <sstream>
#include <stdio.h>

using namespace std;

static bool debug_mode;
static Stash * stash = new Stash();
static int num_threads = 0;
static int total_threads = 0;

// from emery's code
int Server::messageNumber () {
  static int number = 0;
  static pthread_mutex_t numberLock = PTHREAD_MUTEX_INITIALIZER;
  int v;
  pthread_mutex_lock (&numberLock); 
  v = number; 
  number++;
  pthread_mutex_unlock (&numberLock);
  return v;
}

// this macro brought to you by john "jazz" foley
#define show(var) do { if (debug_mode) { std::cout << __FILE__ ":"<<__LINE__<<":" #var << " = " << var << std::endl; } } while(0)

void * Server::serve (void * cv){
  // Just read exactly one string.
  auto * client = (simplesocket *) cv; 
  show(num_threads);
  // most messages are short
  int cmd_bufsize = 512;
  char cmd_buffer[cmd_bufsize];
  int nbytesRead; int outcome;
  if (debug_mode) client->write("memstashed>", 11);
  while ((nbytesRead = client->read (cmd_buffer,cmd_bufsize)) && outcome!=EXIT) {
    Cmd * c = new Cmd(-1);
    char err[250];
    for (int i = nbytesRead ; i < cmd_bufsize ; i++)
      cmd_buffer[i]='\0';
    show(cmd_buffer); show(nbytesRead);
    if (strcmp(cmd_buffer, "\r\n")){
      try{
	c = Parser::parse_cmd(cmd_bufsize, cmd_buffer);
	if (not c) bad_cmd_err();
	Parser::parse(c, cmd_bufsize, cmd_buffer);
	show(Cmd::parse_types[c->cmd]);
	if (Cmd::parse_types[c->cmd]==1){
	  int data_bufsize = c->blocksize; show(data_bufsize);
	  char data_buffer[data_bufsize];
	  memset(data_buffer, '\0', data_bufsize);
	  client->read(data_buffer, data_bufsize); show(data_buffer);
	  Packet * p = c->exec_cmd(data_buffer, stash);
	  outcome = p->transmission_handler(client);
	} else {	  
	  Packet * p = c->exec_cmd(cmd_buffer, stash);
	  outcome = p->transmission_handler(client);
	}
      } catch (int e) {
	sprintf(err, errors[e], Cmd::cmds[c->cmd]); 
	client->write(err, strlen(err));
      }
      if (debug_mode){
	cout << c->cmd << Cmd::cmds[c->cmd] << endl;
      }
    }
    if (outcome!=EXIT && debug_mode) client->write("memstashed>", 11);
  }
  delete client;
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
  stash->init();
  cout << "Entering loop.\n" << endl;
  while(true){
    // Create one thread per connection.
    auto * c = s->accept();
    pthread_t * t = new pthread_t;
    int r = pthread_create (t, NULL, serve, (void *) c);
    num_threads++; total_threads++;
    if (r) {
      pthread_join(*t, NULL);
      num_threads--;
    }
  }
  // We will never get here...
  s->close();
  return 0;
};
