** Memstashed **

== How I would have done things if I knew three weeks ago what I know now about C programming. ==

Right now there's a variable that determines the size of the hash table. According to the memcached documents, this is supposed to be a command line argument. While my program parses and passes back all of the command line arguments of memcached, few are actually implemented. I am provably guaranteed to have the worst performance in the class, due to the fact that my hash table is currently set to size 3. 

I would have liked to have done some tests at start up to get an estimate of how large cache is. I looked into this when thinking about bucket sizes, etc, and I found some resources indicating that the best way to do this was to do a series of experiements and find a dropoff point. I would use this to indicate the optimal size of "fast things." While data has a cap in size, the actual sizes of the units vary. It seems that memcached proper uses a "slab" based system, which manages like-sized chunks of memory as units. Kind of looks like bin packing something something. Anyway, I was hoping to have the chance to see if there was some kind of performance drop when access things of a certain size and use this knowledge when managing memory. 

The hash table is currently implemented as an array of Bucket objects. Buckets are a collection of header information and data. They are only locked for writes. When reading all the pieces of data, I try to return a pointer to the Bucket. I know I'll get some crappy stuff here, since when I write I don't just write a new Bucket, which I think would replace the memory reference and leave the pointer used in data access a-ok. We don't care about staleness and even inconsistency isn't too much of a concern. We just push all these annoying details onto the client.

I would have preferred to have had a heirarchical hash table, to mimic cache. It also would have been nice to have a better hash function, but alas right now I'm just modding over the size of the table. I don't check the timestamps to implement LRU and don't have flush_all running. I'm a little unclear on whether this timestamp that compares freshness of data is supposed to be real time or if it's supposed to be memcached time. While the protocol states that time is measured in seconds, I don't know how accurate we can expect this to be. Assuming we can just query some system time and compare, this should be easy for SWIM to implement.

== Known problems/todo ==

* Change the default initial uid to be 0 or 1 instead of -1.
* Data blocks are supposed to end with '\r\n', but if the user specifies a data block size that truncates the trailing '\r\n', this data will still be stored as-is. It is the responsibility of the client to ensure that the specified data blocks are the right size. Note that if the memstashed server is started up with the --debug option, it will print the prompt twice if there is a trailing '\r\n'. 
* Generally, proper behavior must be exhibited by the user, or the program will exit. It would be better to kill the offending thread than to kill the server.
* `incr/decr` instructions are not currently implemented.
* `flush_all` not implemented - need to figure out what our sense of time is. Right now calling this will throw an exception. -> FIXED
* `version` not implemnented - throws exception. Should be trivial to fix. Is just annoying right now. -> FIXED
* `quit` also crashes. -> FIXED

== NOTES ==

* When the number of bytes to send is specified as 0, the connection will close. 
* Fuck all my haters.