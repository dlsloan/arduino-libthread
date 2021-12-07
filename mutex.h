#ifndef __mutex_h
#define __mutex_h

class _thread;

class mutex {
private:
	friend class _thread;

	_thread *owner;
	_thread *pending;

	mutex(_thread *owner) : owner(owner), pending(nullptr) {}

public:
	mutex() : owner(nullptr), pending(nullptr) {}

	void lock();
	bool trylock();
	void unlock();
};

#endif