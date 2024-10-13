#ifndef _SERVER_TCP_HPP
#define _SERVER_TCP_HPP

#include "Thread_Pool.hpp"
#include "Task.hpp"
#include "log.hpp"

#include <iostream>
#include <string>
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

using namespace td;

namespace server
{

    void socketIO(int socket)
    {
        char buffer[1024] = {0}; // 初始化缓冲区
        for (;;)
        {
            memset(buffer, 0, sizeof(buffer)); // 每次读取前清空缓冲区

            int valread = read(socket, buffer, sizeof(buffer));
            if (valread == 0)
            {
                logMessage(NORMAL, "Client disconnected");
                // std::cout << "Client disconnected" << std::endl;
                break;
            }
            else if (valread == -1)
            {
                logMessage(ERROR, "Read error. errno: %d", errno);
                // std::cerr << "Read error. errno: " << errno << std::endl;
                break;
            }

            // std::cout << "Client # " << buffer << std::endl;
            logMessage(NORMAL, "Client # %s", buffer);

            std::string message = std::string(buffer) + " server[received]";
            write(socket, message.c_str(), message.size());
        }
        close(socket); // 关闭套接字
    }

    class ServerTCP;
    struct ThreadData{
        int _socket;
        ServerTCP* _server;
        ThreadData(ServerTCP* server, int socket)
            : _server(server), _socket(socket)
        {}
    };

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
                // std::cerr << "Error creating socket" << std::endl;
                exit(1);
            }
            logMessage(NORMAL, "socket %d created", _socketfd);
            // std::cout << "socket " << _socketfd << " created" << std::endl;

            // bind
            if (bind(_socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
            {
                logMessage(ERROR, "Error binding socket");
                // std::cerr << "Error binding socket" << std::endl;
                exit(1);
            }
            logMessage(NORMAL, "bind success");
            // std::cout << "bind success" << std::endl;

            // listen
            if (listen(_socketfd, 5) < 0)
            {
                logMessage(ERROR, "Error listening on socket");
                // std::cerr << "Error listening on socket" << std::endl;
                exit(1);
            }
            logMessage(NORMAL, "listen success");
            // std::cout << "listen success" << std::endl;
        }

        void run()
        {
            Thread_pool<Task>::getInstance()->run();
            // 连接客户端
            for (;;)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_size = sizeof(client_addr);
                int client_socket = accept(_socketfd, (struct sockaddr *)&client_addr, &client_addr_size);
                if (client_socket < 0)
                {
                    logMessage(ERROR, "Error accepting client");
                    // std::cerr << "Error accepting client" << std::endl;
                    continue;
                }
                logMessage(NORMAL, "Client connected");
                // std::cout << "Client connected" << std::endl;

                // version 1
                // 串行执行版
                // socketIO(client_socket);
                // close(client_socket);

                // version 2
                // 多进程版
                // pid_t id = fork();
                // if (id == 0) // child
                // {
                //     close(_socketfd);
                //     if(fork()>0) exit(0);
                //     socketIO(client_socket);
                //     close(client_socket);
                //     exit(0);
                // }
                // close(client_socket);

                // //father
                // pid_t ret = waitpid(id, nullptr, 0);
                // if(ret>0)
                // {
                //     std::cout << "waitsuccess: " << ret << std::endl;
                // }

                // version 3 
                // 多线程版
                // pthread_t _tid;
                // ThreadData *data = new ThreadData(this, client_socket);
                // pthread_create(&_tid, nullptr, threadFunc, data);


                // version 4 
                // 线程池版
                Thread_pool<Task>::getInstance()->push(Task(socketIO, client_socket));
            }
        }

        // static void *threadFunc(void *args)
        // {
        //     pthread_detach(pthread_self());
        //     ThreadData *data = static_cast<ThreadData *>(args);
        //     data->_server->socketIO(data->_socket);
        //     close(data->_socket);
        //     delete data;
        //     return nullptr;
        // }

    };
}

#endif // _SERVER_TCP_HPP