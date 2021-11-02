#include <assert.h>

extern "C" {

static void __attribute__ ((used)) __attribute__((noinline)) task_fail() {
	//This code exists to protect against a run-away task from executing
	//random pieces of memory as code. It should never be reached.

	//assert(false) may alert programmers of the bug if debugging is enabled
	//and asserts are setup to be reported
	assert(false);

	//Infinite loop keeps proc here on death and out of random code.
	while(true);
}

static void __attribute__((naked)) __attribute__((noinline)) _asm_task_start() {
	//This is a special function that knows it will only be called immediatly after
	//the first task switch to a new task. since task init sets the entery for r5 to
	//the tasks data, we copy the data value from r5 to r0 (C param0) before calling
	//the task function
	asm(
		//task func param
		"mov r0, r5;"
		//call task
		"blx r4;"
		"bl task_fail;"
	);
}

static void* __attribute__((naked)) __attribute__((noinline)) _asm_task_init(void *func, void* data, void *stack, void (*start)()) {
	//C to asm translation (params0-3 -> r0-r3)
	//  func -> r0
	//  data -> r1
	//  stack -> r2
	//  start -> r3
	//return uses what ever is in r0
	//  bx lr; <- return r0
	asm(
		//backup r6 for local use
		"push {r6};"
		"mov r6, sp;"
		//switch to new stack
		"mov sp, r2;"
		//push start address onto task location entry
		"push {r3};"
		//init r4-r7 (r0-r3 will be popped to r4-r7 in task entry)
		"push {r0-r3};"
		//5 dummy values to pad the stack to the right size
		"push {r0-r4};"
		//return modified task stack
		"mov r0, sp;"
		//restore current stack
		"mov sp, r6;"
		//restore r6
		"pop {r6};"
		"bx lr;"
	);
}

void *__attribute__((noinline)) asm_task_init(void *func, void* data, void *stack, int stackLen) {
	//Simple wrapper for assembly function, converts the stack buffer to
	//stack pointer base and sets special task start function address.

	//In the samd (ARM Cortex M0) stacks work backwards from the end of a
	//buffer to the start and write 1 entry back, so we convert the stack
	//address to the next element past the end of the buffer an pass it to
	//the assembly init function.

	//The _asm_task_start is a constant which should in theory be in the
	//assembly, but I'm not sure how to do that >.< so it goes here for now
	//as a parameter passed to the assembly.
	return _asm_task_init(func, data, stack + stackLen, &_asm_task_start);
}

void __attribute__((naked)) __attribute__((noinline)) asm_task_swap(void **currentStack, void* nextStack) {
	//C to asm translation (params0-3 -> r0-r3)
	//  currentStack -> r0
	//  nextStack -> r1
	asm(
		//-- Current Task --
		//    Store current task exec location
		"push {lr};"
		//    Backup current task cpu state
		"push {r4-r7};"
		"mov r2, r8;"
		"mov r3, r9;"
		"mov r4, r10;"
		"mov r5, r11;"
		"mov r6, r12;"
		"push {r2-r6};"
		//-- Switch Tasks --
		//    Write current task stack pointer
		"mov r3, sp;"
		"str r3, [r0];"
		//    Switch to next task stack
		"mov sp, r1;"
		//-- Next Task --
		//    Restore cpu state of next task
		"pop {r1-r5};"
		"mov r8, r1;"
		"mov r9, r2;"
		"mov r10, r3;"
		"mov r11, r4;"
		"mov r12, r5;"
		"pop {r4-r7};"
		//    Load next task's exec location and resume execution
		"pop {r1};"
		"mov lr, r1;"
		"bx lr;"
	);
}

}; // extern "C"