#ifndef _coop_h
#define _coop_h
extern "C" {

typedef void (*ThreadFunc)(void *param);

void asm_coop_init();
void *asm_task_init(ThreadFunc func, void *data, void *stack, int stackLen);
void asm_task_swap(void **currentStack, void* nextStack);
}
#endif//_coop_h