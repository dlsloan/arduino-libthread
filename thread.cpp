#include <Arduino.h>
#include <thread.h>

_thread *_thread::active = nullptr;
_thread *_thread::idle = nullptr;

_thread main_thread;

_thread::_thread() : running(true) {
	if (_thread::active == nullptr) {
		_thread::active = this;
		prev = this;
		next = this;
	} else {
		prev = _thread::active;
		next = _thread::active->next;
		next->prev = this;
		prev->next = this;
	}
}

//void yield() {
//	_thread::swap();
//}

void _thread::swap() {
	__sync_synchronize();
	if (_thread::active->next != _thread::active) {
		_thread *current = _thread::active;
		_thread::active = _thread::active->next;
		__sync_synchronize();
		asm_task_swap(&current->sp, _thread::active->sp);
	}
}

void _thread::join() {
	while (running)
		yield();
}