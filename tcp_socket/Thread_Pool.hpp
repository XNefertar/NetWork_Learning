#ifndef _THREAD_POOL_HPP
#define _THREAD_POOL_HPP

#include "vector"
#include "queue"
#include "mutex"
#include "Lock_Guard.hpp"
#include "Task.hpp"
#include "Thread.hpp"
#include "Thread_Pool.hpp"
#include "pthread.h"

const int global_num = 10;
using namespace td;


template<class T>
struct Thread_Data;

template<class T>
class Thread_pool{
private:
    static void* _start_func(void* args){
        Thread_Data<T>* thread_d = static_cast<Thread_Data<T>*>(args);

        while(1){
            // 锁的生命周期和任务的处理需要分开
            // 使得任务处理阶段可以多线程并发执行
            T t;
            {
                // 加锁
                lock_guard lock(thread_d->thread_pool->get_mutex());
                // thread_d->thread_pool->lock_mutex();
                while(thread_d->thread_pool->isEmpty()) { thread_d->thread_pool->thread_wait(); }
                t = thread_d->thread_pool->pop();
                
                // thread_d->thread_pool->unlock_mutex();
            }
            t();
        }
        delete thread_d;
        return nullptr;
    }

    Thread_pool(const int num = global_num)
        :_num(global_num)
    {
        // 互斥量及信号量初始化
        pthread_mutex_init(&_mtx, nullptr);
        pthread_cond_init(&_cond, nullptr);

        // 线程对象构建
        for(int i = 0; i < _num; ++i){
            _threads.push_back(new Thread());
        }
    }

    void operator=(const Thread_pool&)   = delete;
    Thread_pool(const Thread_pool&) = delete;

    int _num;
    std::vector<Thread*> _threads;
    std::queue<T> _task_queue;
    pthread_mutex_t _mtx;
    pthread_cond_t _cond;

    static Thread_pool<T>* tp;
    static std::mutex _single_lock;

public:
    void lock_mutex() { pthread_mutex_lock(&_mtx); }
    void unlock_mutex() { pthread_mutex_unlock(&_mtx); }
    bool isEmpty() { return _task_queue.empty(); }
    T pop() {
        T tmp = _task_queue.front();
        _task_queue.pop();
        return tmp;
    }
    void thread_wait() { pthread_cond_wait(&_cond, &_mtx); }
    pthread_mutex_t *get_mutex() { return &_mtx; }

    
    static Thread_pool<T>* getInstance(){
        if(nullptr == tp){
            std::lock_guard<std::mutex> _lock(_single_lock);
            tp = new Thread_pool<T>();
        }
        return tp;
    }


    void push(const T& in){
        lock_guard lock(&_mtx);
        _task_queue.push(in);
        pthread_cond_signal(&_cond);
    }

    void run(){
        for(const auto& it : _threads){
            
            Thread_Data<T>* td = new Thread_Data<T>(this, it->get_name());
            it->start(_start_func, td);
            std::cout << it->get_name() << " is starting..." << std::endl;
        }
    }

    ~Thread_pool(){
        pthread_mutex_destroy(&_mtx);
        pthread_cond_destroy(&_cond);
        for(const auto& it : _threads){
            it->join();
        }
    }

};

template<class T>
struct Thread_Data{
    Thread_pool<T>* thread_pool;
    std::string _name;

    Thread_Data(Thread_pool<T>* tp, const std::string name)
        :thread_pool(tp),
        _name(name)
    {}

};

template<class T>
Thread_pool<T>* Thread_pool<T>::tp = nullptr;

template<class T>
std::mutex Thread_pool<T>::_single_lock{};


#endif