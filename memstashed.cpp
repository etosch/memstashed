// memcached defaults to 4 worker threads
// use as much RAM as you can
// each server should have roughly the same amount of RAM : goal is "cluster uniformity"
// avoid swapping
// do not over-allocate memory

#include "Server.h"
// includes simplesocket.h, pthread.h, sstream, iostream?
#include <stdio.h>
#include <string>

int megs(int n){
  return 1024*1024*n;
}

int main(int argc, char * argv[]){
  char delim = ':';
  char * ip_addr = "localhost";
  char * protocol="auto";
  int size = megs(64), max_connections=1024, udp_port = 0, tcp_port = 11211, warn=0, max_threads=4, verbose=0;
  int memory_chunk_multiplier, default_slab_page, min_size;
  bool auto_free, daemon=false, stats=false, cas=true, debug=false;
  printf("WARNINGS:\n");
  for (int i = 1, j=2; i<argc ; i+=2, j+=2){
    auto match_flag = [&] (char * flag) { return strcmp(argv[i], flag)==0; };
    auto printwarn = [&] (char * msg) { printf("\t%s\t%s\t%s\n", argv[i], argv[j], msg); warn++; };
    if (match_flag("--debug")){
      debug=true;
      i-=1; j-=1;
  } else if (match_flag("-s")){
      printwarn("Assert that val is a path; Unix socket path to listen on (disables network support). Not implemented.");
    } else if (match_flag("-a")){
      printwarn("Permissions  (in  octal  format) for Unix socket created with -s option. Not implemented.");
    } else if (match_flag("-l")){
      ip_addr = argv[j];
    } else if (match_flag("-d")){
      printwarn("Run memcached as a daemon. Not implemented."); 
      daemon=true;
      i-=1; j-=1;
    } else if (match_flag("-u")) {
      printwarn("Assume the identity of <username> (only when run as root). Not implemented.");
    } else if (match_flag("-m")){
      size = megs(atoi(argv[j]));
    } else if (match_flag("-c")){
      max_connections = atoi(argv[j]);
    } else if (match_flag("-R")){
      printwarn("This option seeks to prevent  client  starvation  by  setting  a process from an individual client connection. Once a  connection has  exceeded this value, the server will attempt to process I/O on other connections before handling any  further  request  from this connection. The default value for this option is 20. Not implemented.");
    } else if (match_flag("-k")){
      printwarn("Lock down all paged memory. Not implemented.");
    } else if (match_flag("-p")){
      tcp_port = atoi(argv[j]);
    } else if (match_flag("-U")){
      printwarn("UDP connection not implemented."); 
      udp_port = atoi(argv[j]);
      tcp_port = 0;
    } else if (match_flag("-M")){
      auto_free = false;
    } else if (match_flag("-r")){
      printwarn("Raise the core file size limit to the maximum allowable. Not implemented.");
    } else if (match_flag("-f")){
      printwarn("memstashed is currently implemented to use a constant size memory chunk.");
      memory_chunk_multiplier = atoi(argv[j]);
    } else if (match_flag("-n")){
      //sets minimum size for keys, vals, and flags
      min_size = atoi(argv[j]);
    } else if (match_flag("-C")){
      //disables check and set
      cas=false;
    } else if (match_flag("-h")){
      printf("memstashed has the same options as memcached. Some of these options are not yet implemented; an error message will be printed if a provided option is not yet implemented. Someday I will move this junk from main to a hashmap for better printing and man facility.\n");
    } else if (match_flag("-v")){
      verbose=1;
    } else if (match_flag("-vv")){
      verbose=2;
    } else if (match_flag("-i")){
      printf("For licencing information, email etosch@gmail.com");
    } else if (match_flag("-P")){
      if (daemon){
	printwarn("Print pidfile to <filename>, only used under -d option. Not yet implemented");
      }
    } else if (match_flag("-t")){
      max_threads = atoi(argv[j]);
    } else if (match_flag("-D")){
      stats=true; delim=*argv[j];
    } else if (match_flag("-L")){
      printwarn("Try to use large memory pages (if available). Increasing the memory page size could reduce the number of TLB misses and improve the  performance. In order to get large pages from the OS, memcached will allocate the total item-cache in one large chunk. Only available if supported on your OS. Not supported.\n");
    } else if (match_flag("-B")){
      printwarn("Specify the binding protocol to use.  By default, the server will autonegotiate client connections.  By using this option, you can specify the protocol clients must speak. Possible options are \"auto\" (the default, autonegotiation behavior), \"ascii\" and \"binary\". Only auto implemented?");
      protocol=argv[j];
    } else if (match_flag("-I")){
              //       Override the default size of each slab page. Default is 1mb. Default is 1m, minimum is 1k, max is  128m.  Adjusting  this  value
              // changes  the item size limit.  Beware that this also increases the number of slabs (use -v to view), and the overal memory usage
              // of memcached.
      default_slab_page=atoi(argv[j]);
    } else if (match_flag("-o")){
      printwarn("Comma separated list of extended or experimental options. See -h or wiki for up to date list. Yeah, right!"); warn++;
    } else printwarn("UNRECOGNIZED ARGUMENT!");
  }
  if (not warn) printf("\tHuzzah! No startup warnings!\n");
  return Server::run(debug, delim, ip_addr, protocol, size, max_connections, udp_port,
	      tcp_port, max_threads, verbose, memory_chunk_multiplier,
	      default_slab_page, min_size, auto_free, daemon, stats, cas);
}
