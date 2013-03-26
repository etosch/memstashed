CXX = clang++
CXXFLAGS = -std=c++11 -g -I. -Wno-write-strings -stdlib=libc++
PORT = 11211

SRCS = memstashed.cpp Server.cpp Parser.cpp errors.cpp exec.cpp Stash.cpp Bucket.cpp replies.cpp

main:
	$(CXX) $(CXXFLAGS) $(SRCS) simplesocket.cpp -o memstashed -lpthread
	./memstashed -p $(PORT) --debug

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

