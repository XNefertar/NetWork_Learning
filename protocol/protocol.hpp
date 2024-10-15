#ifndef _PROTOCOL_HPP
#define _PROTOCOL_HPP

#include <string>
#include <cstring>
#include <jsoncpp/json/json.h>

#define SEP " "
#define SEP_LEN strlen(SEP)
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)

// 自定义协议
namespace Procotol
{
    enum
    {
        OK = 0,
        DIV_ZERO,
        MOD_ZERO,
        OP_ERR
    };

    std::string enLength(const std::string& text)
    {
        // "len\n" + text + "\n"
        std::string len = std::to_string(text.size());
        std::string message = len + LINE_SEP + text + LINE_SEP;
        return message;
    }

    bool deLength(const std::string& package, std::string* text)
    {
        auto it = package.find(LINE_SEP);
        if(it == std::string::npos)
        {
            return false;
        }
        size_t len = std::stoi(package.substr(0, it));
        // 获取用户正文
        *text = package.substr(it + LINE_SEP_LEN, len);
        return true;
    }

    
    bool recvPackage(int sockfd, std::string &inbuffer, std::string *text){
        char buffer[1024] = {0};
        for(;;)
        {
            memset(buffer, 0, sizeof(buffer));
            int valread = read(sockfd, buffer, sizeof(buffer));
            if(valread == 0)
            {
                logMessage(NORMAL, "Client disconnected");
                std::cout << "Client disconnected" << std::endl;
                return false;
            }
            else if(valread == -1)
            {
                logMessage(ERROR, "Read error. errno: %d", errno);
                std::cerr << "Read error. errno: " << errno << std::endl;
                return false;
            }

            inbuffer += buffer;

            // 对 inbuffer 内的数据进行处理
            size_t pos = inbuffer.find(LINE_SEP);
            if(pos == std::string::npos)
            {
                return false;
            }
            int text_len = std::stoi(inbuffer.substr(0, pos));

            int total_len = text_len + pos + LINE_SEP_LEN * 2;

            // 处理正文长度小于等于报头中给定长度的情况
            if(total_len > inbuffer.size())
            {
                continue;
            }
            *text = inbuffer.substr(0, total_len);
            inbuffer.erase(0, total_len);
            break;
        }
        return true;
    }
    
    // Class Request
    class Request
    {
    private:
        int _x;
        int _y;
        char _op;

    public:
        Request()
            : _x(0), _y(0), _op('+')
        {}

        Request(int x, int y, char op)
            : _x(x), _y(y), _op(op)
        {}

        int getX() const { return _x; }
        int getY() const { return _y; }
        char getOp() const { return _op; }

        // 序列化
        // 输出型参数
        // "x op y"
        bool serialize(std::string *out)
        {
#ifdef MYSTLYE
            *out = std::to_string(_x) + SEP + _op + SEP + std::to_string(_y);
#else
            Json::Value root;
            root["first"] = _x;
            root["second"] = _y;
            root["op"] = _op;

            Json::FastWriter writer;
            *out = writer.write(root);
#endif

            return true;
        }
        // 反序列化
        // 输入型参数
        // 字符串切割
        bool deserialize(const std::string &in)
        {
#ifdef MYSTLYE
            size_t pos = in.find(SEP);
            size_t rpos = in.rfind(SEP);
            if (pos == std::string::npos || rpos == std::string::npos)
            {
                return false;
            }
            if(pos == rpos)
            {
                return false;
            }

            _x = std::stoi(in.substr(0, pos));
            _op = in[pos + 1];
            _y = std::stoi(in.substr(rpos + 1));
#else
            Json::Reader reader;
            Json::Value root;
            if(!reader.parse(in, root))
            {
                return false;
            }
            _x = root["first"].asInt(); 
            _y = root["second"].asInt();
            _op = static_cast<char>(root["op"].asInt());
#endif
            return true;
        }
    };


    // Class Response
    class Response
    {
    private:
        int _result;
        int _exitcode;

    public:
        Response(int result = 0, int exitcode = 0)
            : _result(result), _exitcode(exitcode)
        {}

        void setResult(int result) { _result = result; }
        void setExitcode(int exitcode) { _exitcode = exitcode; }

        int getResult() const { return _result; }
        int getExitcode() const { return _exitcode; }

        // 序列化
        // 输出型参数
        // "x op y"
        bool serialize(std::string *out)
        {
#ifdef MYSTLYE
            *out = std::to_string(_result) + SEP + std::to_string(_exitcode) + LINE_SEP;
#else
            Json::Value root;
            root["result"] = _result;
            root["exitcode"] = _exitcode;

            Json::FastWriter writer;
            *out = writer.write(root);
#endif
            return true;
        }

        // 反序列化
        // 输入型参数
        // 字符串切割
        bool deserialize(const std::string &in)
        {
#ifdef MYSTLYE
            size_t pos = in.find(SEP);
            _result= std::stoi(in.substr(0, pos));
            _exitcode = std::stoi(in.substr(pos + 1, in.size() - pos - LINE_SEP_LEN));
#else
            Json::Reader reader;
            Json::Value root;
            if(!reader.parse(in, root))
            {
                return false;
            }
            _result = root["result"].asInt();
            _exitcode = root["exitcode"].asInt();
#endif
            return true;
        }
    };
}

#endif // _PROTOCOL_HPP