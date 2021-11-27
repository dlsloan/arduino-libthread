#ifndef __thread_h
#define __thread_h

#include <asm_coop.h>
#include <stdint.h>

class _thread {
public:
	_thread();

	void join();

	static void swap();
protected:
	bool running;
	void *sp;

	void init();
	void activate();
	void deactivate();

private:
	_thread *prev, *next;

	static _thread *active;
	static _thread *idle;
};

template<int depth=256>
class thread : _thread {
public:
	thread(ThreadFunc func, void *param=nullptr) : _thread() {
		sp = asm_task_init(func, param, stack, sizeof(stack));
	}
private:
	uint32_t stack[depth];
};

#endif