#include "socketWinClient.hpp"
#include <memory>


using namespace std;
using namespace client;

//static void Usage(string proc)
//{
//    cerr << "\nUsage:\n\t" << proc << " server_ip server_port\n\n";
//}

// ./udpClient server_ip server_port
int main(int argc, char* argv[])
{

    unique_ptr<SocketWinClient> ucli(new SocketWinClient());

    ucli->init();
    ucli->start();

    return 0;
}