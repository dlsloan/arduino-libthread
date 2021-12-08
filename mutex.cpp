#define LIB_DEP
#include <critical.h>
#include <thread.h>
#include <mutex.h>

#include <Arduino.h>
#include <assert.h>

void mutex::lock() {
	{
		critical crit;
		assert(this->owner != _thread::current_thread);
		if (this->owner == nullptr) {
			this->owner = _thread::current_thread;
			return;
		}
		_thread::current_thread->_change_lst(&this->pending);
	}
	yield();
}

bool mutex::trylock() {
	critical crit;
	assert(this->owner != _thread::current_thread);
	if (this->owner == nullptr) {
		this->owner = _thread::current_thread;
		return true;
	}
	return false;
}

void mutex::_crit_unlock() {
	if (this->pending == nullptr) {
		this->owner = nullptr;
		return;
	}
	this->owner = this->pending;
	this->owner->_change_lst(&_thread::active_threads);
}

void mutex::unlock() {
	critical crit;
	this->_crit_unlock();
}