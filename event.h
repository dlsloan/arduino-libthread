#ifndef __event_h
#define __event_h

class _thread;

class event {
    _thread *pending;
public:
    event();

    void wait();
    void signal();
    void broadcast();
};

#endif
