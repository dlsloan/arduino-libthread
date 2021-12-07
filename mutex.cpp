#define LIB_DEP
#include <thread.h>
#include <mutex.h>

#include <Arduino.h>
#include <assert.h>

void mutex::lock() {
	noInterrupts();
	__sync_synchronize();
	assert(this->owner != _thread::current_thread);
	if (this->owner == nullptr) {
		this->owner = _thread::current_thread;
		__sync_synchronize();
		interrupts();
		return;
	}
	if (this->pending == nullptr) {
		_thread::current_thread->mtx_lst_next = _thread::current_thread;
		_thread::current_thread->mtx_lst_prev = _thread::current_thread;
		this->pending = _thread::current_thread;
	} else {
		_thread::current_thread->mtx_lst_next = this->pending;
		_thread::current_thread->mtx_lst_prev =
				this->pending->mtx_lst_prev;
		this->pending->mtx_lst_prev->mtx_lst_next =
				_thread::current_thread;
		this->pending->mtx_lst_prev = _thread::current_thread;
	}
	_thread::current_thread->_crit_remove_from_active();
	__sync_synchronize();
	interrupts();
	yield();
}

bool mutex::trylock() {
	noInterrupts();
	__sync_synchronize();
	assert(this->owner != _thread::current_thread);
	if (this->owner == nullptr) {
		this->owner = _thread::current_thread;
		__sync_synchronize();
		interrupts();
		return true;
	}
	__sync_synchronize();
	interrupts();
	return false;
}

void mutex::unlock() {
	noInterrupts();
	__sync_synchronize();
	if (this->pending == nullptr) {
		this->owner = nullptr;
		goto out;
	}
	this->owner = this->pending;
	if (this->pending->mtx_lst_next == this->pending) {
		this->pending = nullptr;
	} else {
		this->pending = this->pending->mtx_lst_next;
		this->pending->mtx_lst_prev = this->owner->mtx_lst_prev;
		this->pending->mtx_lst_prev->mtx_lst_next = this->pending;
	}
	this->owner->_crit_add_to_active();
out:
	__sync_synchronize();
	interrupts();
}