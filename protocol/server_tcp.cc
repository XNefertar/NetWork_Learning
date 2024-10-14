#include "server_tcp.hpp"
#include "protocol.hpp"
#include <memory>

using namespace std;
using namespace server;
static void Usage(string proc)
{
    cerr << "\nUsage:\n\t" << proc << " server_port\n\n";
}


void callback(const Request &req, Response& res)
{
    switch (req.getOp())
    {
        case '+':
        {
            res.setResult(req.getX() + req.getY());
            break;
        }
        case '-':
        {
            res.setResult(req.getX() - req.getY());
            break;
        }
        case '*':
        {
            res.setResult(req.getX() * req.getY());
            break;
        }
        case '/':
        {
            if (req.getY() == 0)
            {
                res.setResult(-1);
                res.setExitcode(DIV_ZERO);
            }
            else
            {
                res.setResult(req.getX() / req.getY());
            }
            break;
        }
        case '%':
        {
            if (req.getY() == 0)
            {
                res.setResult(-1);
                res.setExitcode(MOD_ZERO);
            }
            else
            {
                res.setResult(req.getX() % req.getY());
            }
            break;
        }
        default:
            res.setResult(-1);
            res.setExitcode(OP_ERR);
            break;
    }
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);

    std::unique_ptr<ServerTCP> tcpServer(new ServerTCP(port));

    tcpServer->init();
    tcpServer->run(callback);

    return 0;
}