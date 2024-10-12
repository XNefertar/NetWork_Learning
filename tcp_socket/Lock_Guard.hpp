#ifndef _LOCK_GUARD_HPP
#define _LOCK_GUARD_HPP

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <vector>


class lock_guard{
private:
    pthread_mutex_t* _mtx;

public:
    lock_guard(pthread_mutex_t* mtx)
        :_mtx(mtx)    
    {
        pthread_mutex_lock(_mtx);
    }

    ~lock_guard(){
        pthread_mutex_unlock(_mtx);
    }
};



















#endif