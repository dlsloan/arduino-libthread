#define LIB_DEP
#include <Arduino.h>
#include <asm_coop.h>
#include <thread.h>

_thread _thread::main_thread;
_thread *_thread::active_thread = &_thread::main_thread;

_thread::_thread() : sp(nullptr), _active(false) {
	next = this;
}

_thread::_thread(void *(*func)(void *), void *data, void *stack, int stacksize):
		 _active(false)
{
	next = this;
	sp = asm_task_init(func, data, stack, stacksize);
}

void _thread::activate() {
	noInterrupts();
	__sync_synchronize();
	if (this->active())
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

void *cpp_swap(void *sp) {
	return _thread::swap(sp);
}