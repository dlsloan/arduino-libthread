#include <stdint.h>
#include <Arduino.h>

extern void *cpp_swap(void *sp);

extern "C" {

struct init_thread_stack {
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
uint32_t xPSR;
} __attribute__ ((packed));

void *asm_task_init(void *(*func)(void *), void *data, void *new_stack, int new_stack_size) {
	NVIC_SetPriority(PendSV_IRQn, 0xff);
	NVIC_EnableIRQ(PendSV_IRQn);
	struct init_thread_stack *stack = (struct init_thread_stack *)((char *)new_stack + new_stack_size - sizeof(*stack));
	stack->r0 = (uint32_t)data;
	stack->pc = (uint32_t)func & 0xfffffffe;
	stack->xPSR = (1 << 24);
	return stack;
}

void *__attribute__((noinline)) _advance(void *sp) {
	return cpp_swap(sp);
}

void __attribute__((naked)) __attribute__((noinline)) PendSV_Handler(void) {
	asm(
		"mov r0, r8;"
		"mov r1, r9;"
		"mov r2, r10;"
		"mov r3, r11;"
		"push {r0-r7};"
		// swap tasks
		"mov r0, sp;"
		"bl _advance;"
		"mov sp, r0;"
		// done swap tasks
		"pop {r0-r7};"
		"mov r11, r3;"
		"mov r10, r2;"
		"mov r9, r1;"
		"mov r8, r0;"
		// return from exception
		"ldr r0, =#0xfffffff9;"
		"bx r0;"
	);
}

}; // extern "C"