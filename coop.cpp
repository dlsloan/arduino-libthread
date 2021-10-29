#if defined(ARDUINO_ARCH_SAMD)
#include "coop_samd.cpp.h"
#elif defined(ARDUINO_ARCH_AVR)
#include "coop_arv.cpp.h"
#else
static_assert(false, "Unsupported target archatecture");
#endif