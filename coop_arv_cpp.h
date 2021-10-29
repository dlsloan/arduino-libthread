extern "C" {

static void __attribute__((noinline)) _task_fail() {
assert(false);
while(true);
}

static void __attribute__((naked)) __attribute__((noinline)) _asm_task_start() {
asm(
//task func param
"mov r0, r5;"
//call task
"blx r4;"
"bl _task_fail;"
);
}

void __attribute__((naked)) __attribute__((noinline)) asm_task_init(void *func, void* data, void **stack, int stackLen) {
asm(
//backup a few regs
"push{r5-r7};"
"mov r5, sp;"
"mov r6, lr;"
//read and advance stack addr
"ldr r7, [r2]"
"add r7, r3"
"str r7, [r2]"
//switch to new stack and task entry lr
"mov sp, r7;"
"mov lr, #_asm_task_start;"
//init r4... (r0... will be popped to r4... in task entry)
"push {r0-r8,lr};"
//restore stack and return
"mov sp, r5;"
"mov lr, r6;"
"pop{r5-r7};"
"bx lr;"
);
}

//todo verify arv m0 asm similarities
void __attribute__((naked)) __attribute__((noinline)) asm_task_swap(void **currentStack, void** nextStack) {
asm(
//Entry Task
"push {r4-r12,lr};"
//Switch stack
"str sp, [r0];"
"ldr sp, [r1]"
//Exit Task
"pop {r4-r12,pc}"
);
}

}; // extern "C"