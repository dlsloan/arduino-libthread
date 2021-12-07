#ifndef __lock_h
#define __lock_h

class mutex;

class lock {
private:
	mutex *mtx;
public:
	lock(mutex *mtx);
	~lock();
};

#endif