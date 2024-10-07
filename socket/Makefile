cc=g++

.PHONY:all
all:udpClient udpServer


udpClient:udp_client.cc
	$(cc) -o $@ $^ -std=c++11
udpServer:udp_server.cc
	$(cc) -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f udpClient udpServer