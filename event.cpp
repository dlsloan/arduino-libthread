#include <event.h>

#define LIB_DEP
#include <critical.h>
#include <thread.h>

event::event() : pending(nullptr) {}

void event::wait() {
    {
        critical crit;
        _thread::current_thread->_change_lst(&this->pending);
    }
    yield();
}

void event::signal() {
    critical crit;
    _thread *th = this->pending;
    if (th == nullptr)
        return;
    th->_change_lst(&_thread::active_threads);
}

void event::broadcast() {
    critical crit;
    while(this->pending != nullptr) {
        this->pending->_change_lst(&_thread::active_threads);
    }
}