#include <iostream>
#include <memory>
#include "reactor.hpp"

using namespace ReactorServer;
static void usage(const char *prog)
{
    std::cout << "Usage: " << prog << " <port>" << std::endl;
}

void callbackDemo(Connections *conns)
{
    conns->_outbuffer = conns->_inbuffer;
}


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    std::unique_ptr<Reactor> reactor(new Reactor(callbackDemo, std::stoi(argv[1])));

    reactor->ReactoInit();
    
    reactor->ReactorRun();

    return 0;
}