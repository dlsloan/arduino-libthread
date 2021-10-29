#if defined(ARDUINO_ARCH_SAMD)
#include "coop_samd.cpp.h"
#else
static_assert(false, "Unsupported target archatecture");
#endif