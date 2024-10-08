#include "udp_server.hpp"
#include <memory>
#include <fstream>
#include <unordered_map>
#include <signal.h>

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


static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
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

    std::unique_ptr<UDPServer> usvr(new UDPServer(handlerMessage, port));

    usvr->initSocket();
    usvr->run();

    return 0;
}