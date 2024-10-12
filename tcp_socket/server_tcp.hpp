#ifndef _SERVER_TCP_HPP
#define _SERVER_TCP_HPP

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

namespace server
{
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

                // version 1
                // 串行执行
                // socketIO(client_socket);
                // close(client_socket);

                // version 2 多进程版
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

                // version 3 多线程版
                pthread_t _tid;
                ThreadData *data = new ThreadData(this, client_socket);
                pthread_create(&_tid, nullptr, threadFunc, data);

            }
        }

        static void *threadFunc(void *args)
        {
            pthread_detach(pthread_self());
            ThreadData *data = static_cast<ThreadData *>(args);
            data->_server->socketIO(data->_socket);
            close(data->_socket);
            delete data;
            return nullptr;
        }

        void socketIO(int socket)
        {
            // 读取和写入可以看作对文件的读取和写入
            // 采用系统调用接口read/write
            char buffer[1024] = {0};
            for (;;)
            {
                int valread = read(socket, buffer, 1024);
                if (valread == 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    break;
                }
                std::cout << "Client # " << buffer << std::endl;

                std::string message = buffer;
                message += " server[received]";
                write(socket, message.c_str(), message.size());
            }
        }
    };
}

#endif // _SERVER_TCP_HPP