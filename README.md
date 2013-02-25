**REWRITE THIS**
Your assignment is "memstashed". You will write a multithreaded memcached
server in C++11, the new dialect of C++. Due date is *MONDAY MARCH 18*.

What is memcached? See http://memcached.org/ and
http://en.wikipedia.org/wiki/Memcached.

Basically, it is a glorified in-memory hash table that uses consistent
hashing (see http://en.wikipedia.org/wiki/Consistent_hashing and
http://dl.acm.org/citation.cfm?id=258660). You can store values with a
given key, and then retrieve the value later using that key. However, the
value may have been evicted from memory.

You should INDIVIDUALLY write all of the code in your system; however, you
are free (and welcome) to discuss issues like the API (I expect you to work
out as a class which parts to implement, and run this by me), software
architecture, test cases, etc. If you are unsure as to what would
constitute plagiarism, please contact me.

Set up a *private* github repository to hold your code. You need to learn
how to use git to some extent; see github.com for Mac and PC GUI-based
clients. I expect to see lots of intermediate commits rather than one big
commit for your grade.

Your submission needs to include a Makefile that builds the whole thing,
and a "test" target so that I can easily run your tests. I will naturally
also run my own tests.

In addition to the usual grading criteria, this project is something of a
bake-off: the faster and more memory-efficient it is, the higher your grade
will be.

Make sure to make the cache replacement part pluggable. You are encouraged
to experiment with different replacement policies: examples include random
replacement and Greedy-Dual (which takes both recency and size into
account; see
http://static.usenix.org/publications/library/proceedings/usits97/full_papers/cao/cao_html/node8.html).
Something that does a cost-benefit analysis ( <value of reclaimed space> -
Pr[value needed again]*<cost of recomputation> ) would be cool. An extended
version of the API might be needed for this latter case.