#ifndef __condition_variable_h
#define __condition_variable_h

class mutex;
class _thread;

class condition_variable {
private:
    mutex *mtx;
    _thread *pending;

public:
    condition_variable(mutex *mtx);

    void wait();
    void signal();
    void broadcast();
};

#endif