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

class _thread {
public:
	static _thread main_thread;
	static _thread *active_thread;

	static void *swap(void *sp);

	_thread *next;
	void *sp;
	bool _active;

	_thread();
	_thread(void *(*func)(void *), void *data, void *stack, int stacksize);
	bool active() { return this == &main_thread || _active; }
	void activate();
	void deactivate();
};

template<int depth=256>
class thread: public _thread {
public:
	thread(void *(*func)(void*), void *data) :
		_thread(func, data, stack, sizeof(stack)) {}

	uint32_t stack[depth];
};

#endif