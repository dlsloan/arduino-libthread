#include "asm_coop.h"

#include <assert.h>

#if defined(ARDUINO_ARCH_SAMD)
#include "asm_coop_samd.cpp.h"
#else
static_assert(false, "Unsupported target archatecture");
#endif
