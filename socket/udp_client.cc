#include "udp_client.hpp"
#include <memory>

using namespace client;
using namespace std;
static void Usage(string proc)
{
    cerr << "\nUsage:\n\t" << proc << " server_ip server_port\n\n";
}

// ./udpClient server_ip server_port
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    string serverip = argv[1];
    uint16_t serverport = atoi(argv[2]);

    unique_ptr<UDPClient> ucli(new UDPClient(serverip, serverport));

    ucli->createSocket();
    ucli->run();

    return 0;
}