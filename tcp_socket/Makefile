.PHONY:all
all:server client

server:server_tcp.cc
	g++ -o $@ $^ -std=c++11 -lpthread

client:client_tcp.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f server client