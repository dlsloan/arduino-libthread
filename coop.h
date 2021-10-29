#ifndef _coop_h
#define _coop_h
extern "C" {

typedef void (*TaskFunc)(void *param);

void *asm_task_init(TaskFunc func, void *data, void *stack, int stackLen);
void *asm_task_swap(void *nextStack);

}
#endif//_coop_h