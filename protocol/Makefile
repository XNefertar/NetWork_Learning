.PHONY:all
all:server client

server:server_tcp.cc
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp #-DTEST #-DMYSTYLE

client:client_tcp.cc
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp #-DTEST #-DMYSTYLE

.PHONY:clean
clean:
	rm -f server client log.txt