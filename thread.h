// Define threading specific features (yield and int handlers in header in main
// arduino file to allow lib install without poluting other projects)
#ifndef LIB_DEP
#ifndef __thread_h__lib_deb
#define __thread_h__lib_deb
	#include "asm_coop.cpp.h"

	void yield() {
		__sync_synchronize();
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}
#endif
#endif

#ifndef __thread_h
#define __thread_h

#include <stdint.h>
#include <mutex.h>

static void *thread_start(void *_th);
void *cpp_swap(void *sp);

class _thread {
private:
	friend class mutex;
	friend void *thread_start(void *_th);
	friend void *cpp_swap(void *sp);

	static _thread main_thread;
	static _thread *current_thread;
	static _thread *active_threads;

	_thread **lst;
	_thread *next;
	_thread *prev;
	void *sp;
	void *data;
	void *(*func)(void *);
	mutex join_mtx;

	_thread(_thread **lst);

	void _change_lst(_thread **lst);

public:
	_thread(void *(*func)(void *), void *data, void *stack, int stacksize);
	bool active();
	bool done();
	void activate();
	void deactivate();
	void *join();
};

template<int depth=256>
class thread: public _thread {
public:
	thread(void *(*func)(void*), void *data) :
		_thread(func, data, stack, sizeof(stack)) {}

	uint32_t stack[depth];
};

#endif