== Known problems/todo ==
* Change the default initial uid to be 0 or 1 instead of -1.
* Data blocks are supposed to end with '\r\n', but if the user specifies a data block size that truncates the trailing '\r\n', this data will still be stored as-is. It is the responsibility of the client to ensure that the specified data blocks are the right size. Note that if the memstashed server is started up with the --debug option, it will print the prompt twice if there is a trailing '\r\n'. 
* Generally, proper behavior must be exhibited by the user, or the program will exit. It would be better to kill the offending thread than to kill the server.
* `incr/decr` instructions are not currently implemented.
* `flush_all` not implemented - need to figure out what our sense of time is. Right now calling this will throw an exception.
* `version` not implemnented - throws exception. Should be trivial to fix. Is just annoying right now.
* `quit` also crashes.

== NOTES ==
* When the number of bytes to send is specified as 0, the connection will close. 