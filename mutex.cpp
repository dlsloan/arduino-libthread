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
	_thread::current_thread->_change_lst(&this->pending);
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
	this->owner->_change_lst(&_thread::active_threads);
out:
	__sync_synchronize();
	interrupts();
}