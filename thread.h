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

static void *thread_start(void *_th);
void *cpp_swap(void *sp);

class _thread {
private:
	friend void *thread_start(void *_th);
	friend void *cpp_swap(void *sp);

	static _thread main_thread;
	static _thread *current_thread;
	static _thread *next_thread;

	_thread *active_lst_next;
	_thread *active_lst_prev;
	_thread *mtx_lst_next;
	_thread *mtx_lst_prev;
	void *sp;
	void *data;
	void *(*func)(void *);

	_thread();

	void _crit_add_to_active();
	void _crit_remove_from_active();

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