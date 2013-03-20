CXX = clang++
CXXFLAGS = -std=c++11 -g -I. -Wno-write-strings 
PORT = 11211

SRCS = server.cpp client.cpp simplesocket.cpp simplesocket.h

stashtest: 
#	$(CXX) $(CXXFLAGS) whole_stash.cpp -o sanity
#	./sanity
	$(CXX) $(CXXFLAGS) Bucket.cpp -c -o bucket.o
	$(CXX) $(CXXFLAGS) Stash.cpp -c -o stash.o
	$(CXX) $(CXXFLAGS) sanity.cpp -c -o sanity.o 
	$(CXX) $(CXXFLAGS) stash.o bucket.o sanity.o -o sanity 
	./sanity

main:
	$(CXX) $(CXXFLAGS) memstashed.cpp -o memstashed
	./memstashed -U 9384

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

