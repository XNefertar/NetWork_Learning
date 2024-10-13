#ifndef _THREAD_HPP
#define _THREAD_HPP

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <cassert>
#include <unistd.h>

namespace td{
    using func_ptr = void*(*)(void*);
    // 线程封装

    class Thread{
    private:
        // static void* _start_routine(void* args){
        //     Thread* td = static_cast<Thread*>(args);
        // }


        pthread_t    _tid;
        func_ptr     _func_t;
        void*        _args;
        std::string  _name;

        static int thread_num;

    public:

        // 构造函数
        Thread()
            // :_func_t(func_ptr),
            // _args(args)
        {
            char namebuffer[1024];
            snprintf(namebuffer, sizeof namebuffer, "thread %-2d", thread_num++);
            _name = namebuffer;
        }

        // void start(func_ptr func, void *args = nullptr)
        // {
        //     _func_t = func;
        //     _args = args;
        //     int n = pthread_create(&_tid, nullptr, _start_routine, this); // TODO
        //     assert(n == 0);                                            
        //     (void)n;
        // }

        void start(func_ptr func_t, void* args = nullptr){
            _func_t = func_t;
            _args = args;
            int n = pthread_create(&_tid, nullptr, _func_t, _args);
            assert(n == 0);
            (void)n;
        }

        std::string get_name(){
            return this->_name;
        }

        // 默认析构函数
        ~Thread() = default;

        // 线程合并
        void join(){
            int n = pthread_join(_tid, nullptr);
            std::cout << this->_name << " is joining..." << std::endl;
            assert(0 == n);
            (void)n;
        }

    };

    int Thread::thread_num = 1;
}


#endif