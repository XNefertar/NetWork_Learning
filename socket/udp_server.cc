#include "udp_server.hpp"
#include <memory>
#include <fstream>
#include <unordered_map>
#include <signal.h>

using namespace std;
using namespace server;

// const std::string dictTxt="./dict.txt";
// unordered_map<string, string> dict;

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

    // std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));
    // std::unique_ptr<udpServer> usvr(new udpServer(execCommand, port));
    std::unique_ptr<UDPServer> usvr(new UDPServer(port));

    usvr->initSocket();
    usvr->run();

    return 0;
}