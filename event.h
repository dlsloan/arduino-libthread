#ifndef __event_h
#define __event_h

class _thread;

class event {
    _thread *pending;
    bool sig;
public:
    event();

    void wait(bool reset=false);
    void signal();
    void reset();
};

#endif
