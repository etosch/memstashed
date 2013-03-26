CXX = clang++
CXXFLAGS = -std=c++11 -g -I. -Wno-write-strings 
PORT = 11211

SRCS = server.cpp client.cpp simplesocket.cpp simplesocket.h

stashtest: 
#	$(CXX) $(CXXFLAGS) whole_stash.cpp -o sanity
#	./sanity
	$(CXX) $(CXXFLAGS) Bucket.cpp -c -o bucket.o
	$(CXX) $(CXXFLAGS) Stash.cpp -c -o stash.o
	$(CXX) $(CXXFLAGS) Parser.cpp -c -o parser.o
	$(CXX) $(CXXFLAGS) sanity.cpp -c -o sanity.o 
	$(CXX) $(CXXFLAGS) parser.o stash.o bucket.o sanity.o -o sanity 
	./sanity

main:
	$(CXX) $(CXXFLAGS) memstashed.cpp Server.cpp simplesocket.cpp Parser.cpp errors.cpp exec.cpp Stash.cpp Bucket.cpp replies.cpp -o memstashed -lpthread
	./memstashed -p 1234 --debug

test: clean build #python
	./server $(PORT) &
	python testclient1.py &
	python testclient1.py

build: 
	$(CXX) $(CXXFLAGS) server.cpp simplesocket.cpp -o server -lpthread
	$(CXX) $(CXXFLAGS) client.cpp simplesocket.cpp -o client 

python:
	LIBMEMCACHED=/opt/local easy_install pylibmc

clean:
	rm -f server client

