#ifndef _SERVER_TCP_HPP
#define _SERVER_TCP_HPP
#include "protocol.hpp"

#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

#define NORMAL 0
#define WARNING 1
#define ERROR 2
#define FATAL 3
#define DEBUG 4
#define INFO 5

using namespace std;
using namespace Protocol;

namespace server
{
    using func_t = std::function<void(const HttpRequest &, HttpResponse &)>;

    class ServerHttp
    {
    private:
        int _socketfd;
        int _port;
        func_t _func;

    public:
        ServerHttp(func_t func, int port, int socketfd = -1)
            : _func(func),
              _port(port)
        {
        }

        ~ServerHttp() { close(_socketfd); }

        void init()
        {
            _socketfd = socket(AF_INET, SOCK_STREAM, 0);

            // sock接口复用
            int opt = 1;
            setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            
            struct sockaddr_in server_addr;
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(_port);
            server_addr.sin_addr.s_addr = INADDR_ANY;

            if (_socketfd < 0)
            {
                std::cerr << "Error creating socket" << std::endl;
                exit(1);
            }
            std::cout << "socket " << _socketfd << " created" << std::endl;

            // bind
            if (bind(_socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                std::cerr << "Error binding socket" << std::endl;
                exit(1);
            }
            std::cout << "bind success" << std::endl;

            // listen
            if (listen(_socketfd, 5) < 0)
            {
                std::cerr << "Error listening on socket" << std::endl;
                exit(1);
            }
            std::cout << "listen success" << std::endl;
        }

        void HttpHandle(int sockfd)
        {
            char buffer[40096]{};
            HttpRequest req;
            HttpResponse res;

            int len = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (len > 0)
            {
                buffer[len] = '\0';
                // std::cout << "len: " << len << std::endl;
                // logMessage(NORMAL, "Received: %s", buffer);
                // std::cout << "Received: " << buffer << std::endl;

                std::string inbuffer(buffer);
                req.setInbuffer(inbuffer);
                req.parse();

                _func(req, res);

                send(sockfd, res._outbuffer.c_str(), res._outbuffer.size(), 0);
            }
        }

        void run()
        {
            // 连接客户端
            for (;;)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_size = sizeof(client_addr);
                int client_socket = accept(_socketfd, (struct sockaddr *)&client_addr, &client_addr_size);
                if (client_socket < 0)
                {
                    std::cerr << "Error accepting client" << std::endl;
                    continue;
                }
                std::cout << "Client connected" << std::endl;

                pid_t id = fork();
                // child
                if (id == 0)
                {
                    close(_socketfd);
                    if (fork() > 0)
                        exit(0);
                    HttpHandle(client_socket);
                    close(client_socket);
                    exit(0);
                }
                close(client_socket);

                // father
                pid_t ret = waitpid(id, nullptr, 0);
            }
        }
    };
}

#endif // _SERVER_TCP_HPP