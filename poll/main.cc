#include "pollServer.hpp"
#include <memory>
using namespace Poll;


static void usage(const char *prog)
{
    std::cerr << "Usage:\n\t " << prog << " [port]" << std::endl << std::endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
    }

    std::unique_ptr<PollServer> server(new PollServer());
    server->ServerInit();
    server->ServerStart();

    return 0;
}