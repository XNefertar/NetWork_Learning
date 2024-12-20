#ifndef _SERVER_TCP_HPP
#define _SERVER_TCP_HPP
#include "log.hpp"
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

#define NORMAL  0
#define WARNING 1
#define ERROR   2
#define FATAL   3
#define DEBUG   4
#define INFO    5

using namespace std;
using namespace Procotol;

namespace server
{
    using func_t = std::function<void(const Request&, Response&)>;

    
    void handlerLink(int sockfd, func_t func)
    {
        std::string inbuffer;
        for(;;)
        {
            std::string req_text;
            recvPackage(sockfd, inbuffer, &req_text);

#ifdef TEST
            std::cout << "***********************************" << std::endl;
            std::cout << "Received message: " << req_text << std::endl;
            std::cout << "***********************************" << std::endl;
#endif
            std::string temp;
            if(!deLength(req_text, &temp))
            {
                logMessage(ERROR, "Failed to deLength message");
                // std::cerr << "Failed to deLength message" << std::endl;
                break;
            }
#ifdef TEST
            std::cout << "temp: " << temp << std::endl;

#endif
            // 反序列化
            Request req;
            if(!req.deserialize(temp))
            {
                logMessage(ERROR, "Failed to deserialize message");
                // std::cerr << "Failed to deserialize message" << std::endl;
                break;
            }

            Response res;
            func(req, res);

            // 序列化
            std::string message;
            if(!res.serialize(&message))
            {
                logMessage(ERROR, "Failed to serialize message");
                // std::cerr << "Failed to serialize message" << std::endl;
                break;
            }

            message = enLength(message);
#ifdef TEST
            std::cout << "HandlerLink: " << message << std::endl;
#endif


            write(sockfd, message.c_str(), message.size());
#ifdef TEST
            std::cout << "HandlerLink: " << message << std::endl;
#endif
        }
    }
    class ServerTCP
    {
    private:
        int _socketfd;
        int _port;

    public:
        ServerTCP(int port, int socketfd = -1)
            : _port(port)
        {
        }

        ~ServerTCP() { close(_socketfd); }

        void init()
        {
            _socketfd = socket(AF_INET, SOCK_STREAM, 0);

            struct sockaddr_in server_addr;
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(_port);
            server_addr.sin_addr.s_addr = INADDR_ANY;

            if (_socketfd < 0)
            {
                logMessage(ERROR, "Error creating socket");
                std::cerr << "Error creating socket" << std::endl;
                exit(1);
            }
            logMessage(NORMAL, "socket %d created", _socketfd);
            std::cout << "socket " << _socketfd << " created" << std::endl;

            // bind
            if (bind(_socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                logMessage(ERROR, "Error binding socket");
                std::cerr << "Error binding socket" << std::endl;
                exit(1);
            }
            logMessage(NORMAL, "bind success");
            std::cout << "bind success" << std::endl;

            // listen
            if (listen(_socketfd, 5) < 0)
            {
                logMessage(ERROR, "Error listening on socket");
                std::cerr << "Error listening on socket" << std::endl;
                exit(1);
            }
            logMessage(NORMAL, "listen success");
            std::cout << "listen success" << std::endl;
        }

        void run(func_t func)
        {
            // 连接客户端
            for (;;)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_size = sizeof(client_addr);
                int client_socket = accept(_socketfd, (struct sockaddr *)&client_addr, &client_addr_size);
                if (client_socket < 0)
                {
                    logMessage(ERROR, "Error accepting client");
                    std::cerr << "Error accepting client" << std::endl;
                    continue;
                }
                logMessage(NORMAL, "Client connected");
                std::cout << "Client connected" << std::endl;

                pid_t id = fork();
                // child
                if (id == 0)
                {
                    close(_socketfd);
                    if(fork()>0) exit(0);
                    handlerLink(client_socket, func);
                    close(client_socket);
                    exit(0);
                }
                close(client_socket);

                //father
                pid_t ret = waitpid(id, nullptr, 0);
                if(ret>0)
                {
                    std::cout << "waitsuccess: " << ret << std::endl;
                }
            }
        }
    };
}

#endif // _SERVER_TCP_HPP