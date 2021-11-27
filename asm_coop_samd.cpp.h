#include <stdint.h>
#include <Arduino.h>

extern "C" {
static void _asm_task_run(ThreadFunc func, void *data) {
	func(data);
	task_fail();
}

#define sp_push(sp, value) ({		\
	(sp) = (uint32_t *)(sp) - 1;	\
	*(uint32_t *)(sp) = (uint32_t)(value);	\
})


void asm_coop_init() {
	NVIC_SetPriority(PendSV_IRQn, ~0);
	NVIC_EnableIRQ(PendSV_IRQn);
}

void *asm_task_init(ThreadFunc func, void* data, void *stack, int stackLen) {
	//Simple wrapper for assembly function, converts the stack buffer to
	//stack pointer base and sets special task start function address.

	//In the samd (ARM Cortex M0) stacks work backwards from the end of a
	//buffer to the start and write 1 entry back, so we convert the stack
	//address to the next element past the end of the buffer an pass it to
	//the assembly init function.
	stack = stack + stackLen;

	//Initialize task stack with task run function entry point and arguments
	//r0 -> first param, r1 -> second param
	sp_push(stack, _asm_task_run);	//lr
	sp_push(stack, 0);	//r7
	sp_push(stack, 0);	//r6
	sp_push(stack, 0);	//r5
	sp_push(stack, 0);	//r4
	sp_push(stack, 0);	//r3
	sp_push(stack, 0);	//r2
	sp_push(stack, data);	//r1
	sp_push(stack, func);	//r0
	sp_push(stack, 0);	//r12
	sp_push(stack, 0);	//r11
	sp_push(stack, 0);	//r10
	sp_push(stack, 0);	//r9
	sp_push(stack, 0);	//r8
	sp_push(stack, 0);	//apsr
	return stack;
}

void __attribute__((naked)) __attribute__((noinline)) swap_tasks(void **csp, void* nsp)
{

}

void PendSV_Handler(void) {
	swap_tasks(_csp, _nsp);
}
/*
void __attribute__((naked)) __attribute__((noinline)) PendSV_Handler(void) {
	asm(
		"bx lr;"
	);

}
//*/

void __attribute__((naked)) __attribute__((noinline)) asm_task_swap(void **currentStack, void* nextStack) {
	//C to asm translation (params0-3 -> r0-r3)
	//  currentStack -> r0
	//  nextStack -> r1
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	asm(
		//-- Current Task --
		//    Store current task exec location
		"push {lr};"
		//    Backup current task cpu state
		"push {r0-r7};"
		"mov r2, r8;"
		"mov r3, r9;"
		"mov r4, r10;"
		"mov r5, r11;"
		"mov r6, r12;"
		"push {r2-r6};"
		"mrs r3, apsr;"
		"push {r3};"
		//-- Switch Tasks --
		//    Write current task stack pointer
		"mov r3, sp;"
		"str r3, [r0];"
		//    Switch to next task stack
		"mov sp, r1;"
		//-- Next Task --
		//    Restore cpu state of next task
		"pop {r3};"
		"msr apsr, r0;"
		"pop {r1-r5};"
		"mov r8, r1;"
		"mov r9, r2;"
		"mov r10, r3;"
		"mov r11, r4;"
		"mov r12, r5;"
		"pop {r0-r7};"
		//    Load next task's exec location and resume execution
		"pop {pc};"
	);
}

}; // extern "C"