#include <iostream>
#include <memory>
#include "reactor.hpp"

#define LINE_SEP "\r\n"
#define CALCULATE_SIGNAL "+-*/"

using namespace ReactorServer;

struct Request
{
    int x;
    int y;
    char op;

    Request(int x = 0, int y = 0, char op = '+')
        : x(x), y(y), op(op)
    {}
};


static void usage(const char *prog)
{
    std::cout << "Usage: " << prog << " <port>" << std::endl;
}




void Calculate(Request &request, std::string &out)
{
    switch(request.op)
    {
        case '+':
            out = std::to_string(request.x + request.y);
            break;
        case '-':
            out = std::to_string(request.x - request.y);
            break;
        case '*':
            out = std::to_string(request.x * request.y);
            break;
        case '/':
            if(request.y == 0)
            {
                out = "Divide by zero";
            }
            else
            {
                out = std::to_string(request.x / request.y);
            }
            break;
        default:
            out = "Invalid operator";
            break;
    }
    LOG_MESSAGE(NORMAL, "result: %s\n", out.c_str());
}


void HandlerCalTask(std::string text, std::string &out)
{
    auto iter = text.find_first_of(CALCULATE_SIGNAL);
    if(iter == std::string::npos)
    {
        return;
    }
    int x = std::stoi(text.substr(0, iter));
    int y = std::stoi(text.substr(iter + 1));
    char op = text[iter];
    LOG_MESSAGE(NORMAL, "x: %d, op: %c, y: %d\n", x, op, y);
    Request request(x, y, op);
    Calculate(request, out);
}


void CalulateTask(Connections *conns)
{
    auto iter = conns->_inbuffer.find(LINE_SEP);
    if(iter == std::string::npos)
    {
        return;
    }
    std::string text = conns->_inbuffer.substr(0, iter);
    conns->_inbuffer.erase(0, iter + LINE_SEP_LEN);
    HandlerCalTask(text, conns->_outbuffer);
    LOG_MESSAGE(DEBUG, "recv: %s\n", text.c_str());
    LOG_MESSAGE(DEBUG, "send: %s\n", conns->_outbuffer.c_str());
    std::string header = "Server # ";
    conns->_outbuffer = header + conns->_outbuffer + LINE_SEP;
    conns->_sendHandler(conns);
}



int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    std::unique_ptr<Reactor> reactor(new Reactor(CalulateTask, std::stoi(argv[1])));

    reactor->ReactoInit();
    
    reactor->ReactorRun();

    return 0;
}