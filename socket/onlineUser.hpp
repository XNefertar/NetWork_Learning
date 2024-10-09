#ifndef _USER_MANAGER_HPP_
#define _USER_MANAGER_HPP_

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

class User{
private:
    std::string _address;
    std::string _username;
    uint16_t _port;
    // std::string password;
    // std::string email;
    // std::string phone;
public:
    User(std::string address, uint16_t port, std::string username)
        : _address(address), _port(port), _username(username)
    {}

    std::string getUsername(){
        return _username;
    }

    std::string getAddress(){
        return _address;
    }

    uint16_t getPort(){
        return _port;
    }

    ~User() = default;
};

static int numberID = 0;

class onlineUser{
private:
    std::unordered_map<std::string, std::shared_ptr<User>> _users;
public:
    onlineUser() = default;
    ~onlineUser() = default;

    void addUser(std::string address, uint16_t port){
        std::string username = "User " + std::to_string(numberID++);
        std::shared_ptr<User> user(new User(address, port, username));
        _users.insert(std::make_pair(address, user));
    }

    void removeUser(std::string username, uint16_t port){
        _users.erase(username);
    }

    void showUsers(){
        for(auto it = _users.begin(); it != _users.end(); ++it){
            std::cout << "username: " << it->first << " address: " << it->second->getAddress() << " port: " \
            << it->second->getPort() << "name: " << it->second->getUsername() << std::endl;
        }
    }

    std::string getUsersName(std::string address){
        return _users[address]->getUsername();
    }

    bool isUserExist(std::string address){
        return _users.find(address) != _users.end();
    }

    std::string getUser(sockaddr_in clientAddr){
        if(isUserExist(inet_ntoa(clientAddr.sin_addr)))
            return _users[inet_ntoa(clientAddr.sin_addr)]->getUsername();
        else return "offline user";
    }

    void broadcast(int sockfd, const std::string& address, const uint16_t& port, const std::string& message){
        for(auto it = _users.begin(); it != _users.end(); ++it){
            struct sockaddr_in _clientAddr;
            bzero(&_clientAddr, sizeof(_clientAddr));
            _clientAddr.sin_family = AF_INET;
            _clientAddr.sin_port = htons(it->second->getPort());
            _clientAddr.sin_addr.s_addr = inet_addr(it->second->getAddress().c_str());

            std::string temp = "From " + address + "[" + std::to_string(port) + "]" + " name: " + it->second->getUsername() + " # " + message;
            sendto(sockfd, temp.c_str(), temp.size(), 0, (struct sockaddr*)&_clientAddr, sizeof(_clientAddr));
        }
    }
};


#endif