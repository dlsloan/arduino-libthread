#define LIB_DEP
#include <Arduino.h>
#include <assert.h>
#include <asm_coop.h>
#include <thread.h>
#include <mutex.h>
#include <lock.h>

_thread *_thread::active_threads;
_thread _thread::main_thread(&_thread::active_threads);
_thread *_thread::current_thread = &_thread::main_thread;

static void *thread_start(void *_th) {
	_thread *th = (_thread *)_th;
	th->data = th->func(th->data);
	th->func = nullptr;
	noInterrupts();
	__sync_synchronize();
	th->_change_lst(nullptr);
	__sync_synchronize();
	interrupts();
	th->join_mtx.unlock();
	yield();
}

_thread::_thread(_thread **lst) : lst(nullptr), next(this), prev(this),
		sp(nullptr), data(nullptr), func(nullptr), join_mtx(this) {
	this->_change_lst(lst);
}

_thread::_thread(void *(*func)(void *), void *data, void *stack, int stacksize):
		lst(nullptr), next(this), prev(this), data(nullptr), func(func),
		join_mtx(this)
{
	sp = asm_task_init(thread_start, this, stack, stacksize);
}

bool _thread::active() {
	return this->lst == &_thread::active_threads;
}

bool _thread::done() {
	return this->func == nullptr;
}

void _thread::_change_lst(_thread **lst) {
	if (this->lst == lst)
		return;
	if (this->lst != nullptr) {
		if ((*this->lst)->next == *this->lst) {
			// last element in list
			*this->lst = nullptr;
		} else {
			// currently the list base
			if (*this->lst == this)
				*this->lst = (*this->lst)->next;
			// remove from list
			this->prev->next = this->next;
			this->next->prev = this->prev;
		}
	}
	this->lst = lst;
	// no list
	if (this->lst == nullptr)
		return;
	if (*this->lst == nullptr) {
		// empty list
		this->next = this;
		this->prev = this;
		*this->lst = this;
	} else {
		// add to end of list
		this->prev = (*this->lst)->prev;
		this->next = *this->lst;
		this->prev->next = this;
		this->next->prev = this;
	}
}

void _thread::activate() {
	noInterrupts();
	__sync_synchronize();
	if (this->active() || this->done())
		goto out;

	this->_change_lst(&_thread::active_threads);
out:
	__sync_synchronize();
	interrupts();
}

void _thread::deactivate() {
	noInterrupts();
	__sync_synchronize();
	this->_change_lst(nullptr);
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
	_thread::current_thread = _thread::active_threads;
	_thread::active_threads = _thread::active_threads->next;
	return _thread::current_thread->sp;
}