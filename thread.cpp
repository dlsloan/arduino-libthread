#define LIB_DEP
#include <Arduino.h>
#include <asm_coop.h>
#include <thread.h>

_thread _thread::main_thread;
_thread *_thread::active_thread = &_thread::main_thread;

static void *thread_start(void *_th) {
	_thread *th = (_thread *)_th;
	th->data = th->func(th->data);
	th->done = true;
	th->deactivate();
}

_thread::_thread() : sp(nullptr), _active(false), done(false), data(nullptr) {
	next = this;
}

_thread::_thread(void *(*func)(void *), void *data, void *stack, int stacksize):
		 _active(false), done(false), data(nullptr), func(func)
{
	next = this;
	sp = asm_task_init(thread_start, this, stack, stacksize);
}

void _thread::activate() {
	noInterrupts();
	__sync_synchronize();
	if (this->active() || this->done)
		goto out;
	this->next = _thread::active_thread->next;
	_thread::active_thread->next = this;
out:
	__sync_synchronize();
	interrupts();
}

void _thread::deactivate() {
	_active = false;
	yield();
}

void *_thread::swap(void *sp) {
	_thread::active_thread->sp = sp;
	if (!_thread::active_thread->_active) {
		// todo: deactivate
	}
	_thread::active_thread = _thread::active_thread->next;
	return _thread::active_thread->sp;
}

void *_thread::join() {
	//todo fail on active thread
	while (!this->done)
		yield();

	return this->data;
}

void *cpp_swap(void *sp) {
	return _thread::swap(sp);
}