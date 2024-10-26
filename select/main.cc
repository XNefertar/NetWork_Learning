#include "Util.hpp"
#include <unistd.h>
#include <vector>
#include <functional>

#define InitFunc(func) do{\
    func.push_back(task1);\
    func.push_back(task2);\
    func.push_back(task3);\
}while(0)


#define CallFunc(func) do{\
    for(auto it : func)\
    {\
        it();\
    }\
}while(0)

using  Callback = std::function<void(void)>;
using namespace Util;
int main()
{
    std::vector<Callback> func_vec;
    InitFunc(func_vec);
    char buffer[1024]{};
    set_nonblock(0);
    while(true)
    {
        CallFunc(func_vec);
        
        std::cout << "Please Input # ";
        fflush(stdout);
        ssize_t s = read(0, buffer, sizeof(buffer));
        if(s > 0)
        {
            buffer[s] = '\0';
            std::cout << "read " << s << " bytes: " << buffer << std::endl;
        }
        else if(s == 0)
        {
            std::cout << "read EOF" << std::endl;
            break;
        }
        else
        {
            // std::cout << "read error" << std::endl;
        }
        
        // CallFunc(func_vec);
        sleep(1);
    }
    return 0;
}
