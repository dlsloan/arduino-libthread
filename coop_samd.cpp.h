#include <assert.h>

extern "C" {

static void __attribute__ ((used)) __attribute__((noinline)) task_fail() {
	assert(false);
	while(true);
}

static void __attribute__((naked)) __attribute__((noinline)) _asm_task_start(void *data) {
asm(
	//task func param
	"mov r0, r5;"
	//call task
	"blx r4;"
	"bl task_fail;"
);
}

static void* __attribute__((naked)) __attribute__((noinline)) _asm_task_init(void *func, void* data, void *stack, void (*start)(void *data)) {
asm(
	//backup a few regs
	"push {r6};"
	"mov r6, sp;"
	//switch to new stack and task entry lr
	"mov sp, r2;"
	//init r4-r7 (r0-r3 will be popped to r4-r7 in task entry)
	"push {r3};"
	"push {r3};"
	"push {r0-r3};"
	"push {r0-r4};"
	//restore stack and return
	"mov r0, sp;"
	"mov sp, r6;"
	"pop {r6};"
	"bx lr;"
);
}

void *__attribute__((noinline)) asm_task_init(void *func, void* data, void *stack, int stackLen) {
	return _asm_task_init(func, data, stack + stackLen, &_asm_task_start);
}

void *__attribute__((naked)) __attribute__((noinline)) asm_task_swap(void* nextStack) {
asm(
	//Entry Task
	"push {lr};"
	"push {r4-r7};"
	"mov r2, r8;"
	"mov r3, r9;"
	"mov r4, r10;"
	"mov r5, r11;"
	"mov r6, r12;"
	"push {r2-r6};"
	//Switch stack
	"mov r1, sp;"
	"mov sp, r0;"
	"mov r0, r1;"
	//Exit Task
	"pop {r1-r5};"
	"mov r8, r1;"
	"mov r9, r2;"
	"mov r10, r3;"
	"mov r11, r4;"
	"mov r12, r5;"
	"pop {r4-r7};"
	"pop {r1};"
	"mov lr, r1;"
	"bx lr;"
);
}

}; // extern "C"