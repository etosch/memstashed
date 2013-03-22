class Server {

 public:
  
  static int messageNumber();
  static void * serve(void * cv);
  static int run (char delim, char * ip_addr, char * protocol, 
		  int size, int max_connections, int udp_port, int tcp_port,
		  int max_threads, int verbose, int memory_chunk_multiplier,
		  int default_slab_page, int min_size, bool auto_free, 
		  bool daemon, bool stats, bool cas);
  static bool test();
};
