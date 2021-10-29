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

static void __attribute__((naked)) __attribute__((noinline)) _asm_task_init(void *func, void* data, void *stack, void (*start)(void *data)) {
asm(
//backup a few regs
"push {r6};"
"mov r6, sp;"
//switch to new stack and task entry lr
"mov sp, r2;"
//init r4-r7 (r0-r3 will be popped to r4-r7 in task entry)
"push {r3};"
"push {r0-r3};"
"push {r0-r4};"
//restore stack and return
"mov sp, r6;"
"pop {r6};"
"bx lr;"
);
}

void __attribute__((noinline)) asm_task_init(void *func, void* data, void **stack, int stackLen) {
	*stack += stackLen;
	_asm_task_init(func, data, *stack, _asm_task_start);
}

void __attribute__((naked)) __attribute__((noinline)) asm_task_swap(void **currentStack, void** nextStack) {
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
"mov r7, sp;"
"str r7, [r0];"
"ldr r7, [r1];"
"mov sp, r7;"
//Exit Task
"pop {r0-r4};"
"mov r8, r0;"
"mov r9, r1;"
"mov r10, r2;"
"mov r11, r3;"
"mov r12, r4;"
"mov lr, r5;"
"pop {r4-r7};"
"pop {pc};"
);
}

}; // extern "C"