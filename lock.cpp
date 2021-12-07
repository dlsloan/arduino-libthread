#include <lock.h>
#include <mutex.h>

lock::lock(mutex *mtx) : mtx(mtx) {
	mtx->lock();
}

lock::~lock() {
	mtx->unlock();
}