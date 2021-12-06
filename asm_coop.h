#ifndef _coop_h
#define _coop_h
extern "C" {
typedef void *(*ThreadFunc)(void *param);

void *asm_task_init(ThreadFunc func, void *data, void *stack, int stackLen);
}
#endif//_coop_h