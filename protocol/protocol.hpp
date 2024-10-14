#ifndef _PROTOCOL_HPP
#define _PROTOCOL_HPP

#include <string>
#include <cstring>

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
            *out = std::to_string(_x) + SEP + _op + SEP + std::to_string(_y) + LINE_SEP;
            return true;
        }

        // 反序列化
        // 输入型参数
        // 字符串切割
        bool deserialize(const std::string &in)
        {
            size_t pos = in.find(SEP);
            size_t rpos = in.rfind(SEP);
#ifdef TEST
            std::cout << "pos = " << pos << std::endl;
            std::cout << "rpos = " << rpos << std::endl;
#endif
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
            return true;
        }
    };

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
            *out = std::to_string(_result) + SEP + std::to_string(_exitcode) + LINE_SEP;
            return true;
        }

        // 反序列化
        // 输入型参数
        // 字符串切割
        bool deserialize(const std::string &in)
        {
            size_t pos = in.find(SEP);
            _result= std::stoi(in.substr(0, pos));
            _exitcode = std::stoi(in.substr(pos + 1, in.size() - pos - LINE_SEP_LEN));

            return true;
        }
    };
}

#endif // _PROTOCOL_HPP