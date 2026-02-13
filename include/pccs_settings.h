#ifndef PCCS_SETTINGS_H
#define PCCS_SETTINGS_H

#if defined(arm) || defined(__ARM_ARCH)
// Check for ARMv4T, which is what the ARM7TDMI implements
#if defined(ARM_ARCH_4T) || (defined(__ARM_ARCH) && (__ARM_ARCH == 4))
#define ON_GBA true
#endif
#endif

#ifndef ON_GBA
#define ON_GBA false
#endif

#if ON_GBA
#define USE_CPP_RAND false
#else
#define USE_CPP_RAND true
#endif

#define ACCESS_POKEDEX false

#endif

/* TODO:
- Get nickname function (set nickname function?)
- Test cases (ugh)
*/