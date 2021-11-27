#include "asm_coop.h"

#include <assert.h>

static void task_fail() {
	//This code exists to protect against a run-away task from executing
	//random pieces of memory as code. It should never be reached.

	//assert(false) may alert programmers of the bug if debugging is enabled
	//and asserts are setup to be reported
	assert(false);

	//Infinite loop keeps proc here on death and out of random code.
	while(true);
}

#if defined(ARDUINO_ARCH_SAMD)
#include "asm_coop_samd.cpp.h"
#else
static_assert(false, "Unsupported target archatecture");
#endif
