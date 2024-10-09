#include "udp_server.hpp"
#include "onlineUser.hpp"
#include <memory>
#include <fstream>
#include <unordered_map>
#include <signal.h>
#include <strings.h>

using namespace std;
using namespace server;


const std::string dictPos = "./dictionary.txt";
unordered_map<string, string> dict;

// static void cutString(){

// }


static void initDictionary(){
    ifstream ifs(dictPos, std::ios::binary);
    if(!ifs.is_open()){
        cerr << "Failed to open dictionary file." << endl;
        exit(OPEN_ERR);
    }

    std::string line;
    while(getline(ifs, line)){
        size_t pos = line.find(":");
        if(pos == string::npos){
            continue;
        }
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        dict.insert(make_pair(key, value));
    }

    ifs.close();
}

// Demo1
//  客户-服务端通信
static void handlerMessage(int sockfd, string address, uint16_t port, string message){
    initDictionary();
    struct sockaddr_in _clientAddr;
    memset(&_clientAddr, 0, sizeof(_clientAddr));
    _clientAddr.sin_family = AF_INET;
    _clientAddr.sin_port = htons(port);
    _clientAddr.sin_addr.s_addr = inet_addr(address.c_str());
    if(dict.find(message) != dict.end()){
        string response = dict[message];
        sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&_clientAddr, sizeof(_clientAddr));
    }else{
        string response = "Not found.";
        sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&_clientAddr, sizeof(_clientAddr));
    }
}

// Demo2
// 命令行参数处理
static void execCommand(int sockfd, string address, uint16_t port, string cmd){
    string response;
    FILE* fp = popen(cmd.c_str(), "r");
    if(fp == NULL){
        response = "Failed to execute command.";
    }
    else{
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), fp)){
            response += buffer;
        }
        pclose(fp);
    }
    
    // 信息返回给客户端
    struct sockaddr_in _clientAddr;
    bzero(&_clientAddr, sizeof(_clientAddr));

    _clientAddr.sin_family = AF_INET;
    _clientAddr.sin_port = htons(port);
    _clientAddr.sin_addr.s_addr = inet_addr(address.c_str());
    sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&_clientAddr, sizeof(_clientAddr));
}


// Demo3
// 简易聊天室

// 创建一个在线用户管理对象
static int inode = 0;
onlineUser onlineUsers{};

static void routeMessage(int sockfd, string address, uint16_t port, string message){
    if (message == "online"){
        // std::string nickName = "Online users:" + to_string(inode);
        onlineUsers.addUser(address, port);
    }
    if (message == "offline"){
        onlineUsers.removeUser(address, port);
    }
    // if (message == "show"){
    //     onlineUsers.showUsers();
    // }
    if(onlineUsers.isUserExist(address)){
        onlineUsers.broadcast(sockfd, address, port, message);
    }
    else{
        struct sockaddr_in _clientAddr;
        _clientAddr.sin_family = AF_INET;
        _clientAddr.sin_port = htons(port);
        _clientAddr.sin_addr.s_addr = inet_addr(address.c_str());
        string response = "Please connect to the Internet and try again...";
        sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&_clientAddr, sizeof(_clientAddr));
    }
}


static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}


void reload(int sig){
    (void)sig;
    dict.clear();
    initDictionary();
}

// ./udpServer port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);

    // string ip = argv[1];
    // signal(2, reload);
    // initDict();
    // debugPrint();
    // std::unique_ptr<UDPServer> usvr(new UDPServer(handlerMessage, port));
    // std::unique_ptr<UDPServer> usvr(new UDPServer(execCommand, port));

    std::unique_ptr<UDPServer> usvr(new UDPServer(routeMessage, port));

    usvr->initSocket();
    usvr->run();

    return 0;
}