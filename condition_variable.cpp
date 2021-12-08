#include <condition_variable.h>

#define LIB_DEP
#include <assert.h>
#include <critical.h>
#include <mutex.h>
#include <thread.h>

condition_variable::condition_variable(mutex *mtx) : mtx(mtx), pending(nullptr)
{}

void condition_variable::wait() {
    {
        critical crit;
        assert(_thread::current_thread == this->mtx->owner);
        this->mtx->_crit_unlock();
        _thread::current_thread->_change_lst(&this->pending);
    }
    yield();
}

void condition_variable::signal() {
    critical crit;
    assert(_thread::current_thread == this->mtx->owner);
    if (this->pending != nullptr)
        this->pending->_change_lst(&this->mtx->pending);
}

void condition_variable::broadcast() {
    critical crit;
    assert(_thread::current_thread == this->mtx->owner);
    while (this->pending != nullptr)
        this->pending->_change_lst(&this->mtx->pending);
}