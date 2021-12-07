#define LIB_DEP
#include <Arduino.h>
#include <assert.h>
#include <asm_coop.h>
#include <thread.h>
#include <mutex.h>
#include <lock.h>

_thread _thread::main_thread;
_thread *_thread::current_thread = &_thread::main_thread;
_thread *_thread::next_thread = &_thread::main_thread;

static void *thread_start(void *_th) {
	_thread *th = (_thread *)_th;
	th->data = th->func(th->data);
	th->func = nullptr;
	noInterrupts();
	__sync_synchronize();
	th->_crit_remove_from_active();
	__sync_synchronize();
	interrupts();
	th->join_mtx.unlock();
	yield();
}

_thread::_thread() : active_lst_next(this), active_lst_prev(this),
	mtx_lst_next(nullptr), mtx_lst_prev(nullptr), sp(nullptr),
	data(nullptr), func(nullptr), join_mtx(this) {}

_thread::_thread(void *(*func)(void *), void *data, void *stack, int stacksize):
		active_lst_next(nullptr), active_lst_prev(nullptr),
		mtx_lst_next(nullptr), mtx_lst_prev(nullptr),
		data(nullptr), func(func), join_mtx(this)
{
	sp = asm_task_init(thread_start, this, stack, stacksize);
}

bool _thread::active() {
	return !this->done() && (this->active_lst_next != nullptr);
}

bool _thread::done() {
	return this->func == nullptr;
}

void _thread::_crit_add_to_active() {
	assert(!this->done());
	this->active_lst_next = _thread::next_thread;
	this->active_lst_prev = _thread::next_thread->active_lst_prev;
	_thread::next_thread->active_lst_prev->active_lst_next = this;
	_thread::next_thread->active_lst_prev = this;
}

void _thread::_crit_remove_from_active() {
	if (this == _thread::next_thread) {
		assert(this->active_lst_next != this);
		_thread::next_thread = _thread::next_thread->active_lst_next;
	}
	this->active_lst_next->active_lst_prev = this->active_lst_prev;
	this->active_lst_prev->active_lst_next = this->active_lst_next;
}

void _thread::activate() {
	noInterrupts();
	__sync_synchronize();
	if (this->active() || this->done())
		goto out;

	this->_crit_add_to_active();
out:
	__sync_synchronize();
	interrupts();
}

void _thread::deactivate() {
	noInterrupts();
	__sync_synchronize();
	this->_crit_remove_from_active();
	__sync_synchronize();
	interrupts();
	yield();
}

void *_thread::join() {
	assert(_thread::current_thread != this);
	lock lck(&this->join_mtx);
	return this->data;
}

void *cpp_swap(void *sp) {
	_thread::current_thread->sp = sp;
	_thread::current_thread = _thread::next_thread;
	_thread::next_thread = _thread::next_thread->active_lst_next;
	return _thread::current_thread->sp;
}