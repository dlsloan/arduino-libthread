#include <event.h>

#define LIB_DEP
#include <critical.h>
#include <thread.h>

event::event() : pending(nullptr) {}

void event::wait(bool reset) {
    while (true)
    {
        {
            critical crit;
            if (this->sig) {
                if (reset)
                    this->sig = false;
                else if (this->pending != nullptr)
                    this->pending->_change_lst(&_thread::active_threads);
                return;
            }
            _thread::current_thread->_change_lst(&this->pending);
        }
        yield();
    }
}

void event::signal() {
    critical crit;
    this->sig = true;
    if (this->pending == nullptr)
        return;
    this->pending->_change_lst(&_thread::active_threads);
}

void event::reset() {
    critical crit;
    this->sig = false;
}