#include "select_server.hpp"
#include <memory>
using namespace Select;

static void usage(const char *prog)
{
    std::cerr << "Usage:\n\t " << prog << " [port]" << std::endl << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
    }

    std::unique_ptr<SelectServer> server(new SelectServer(atoi(argv[1])));
    server->init();
    server->start();
    return 0;

}