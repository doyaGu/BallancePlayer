#ifndef PLAYER_RUNTIMESTARTUP_H
#define PLAYER_RUNTIMESTARTUP_H

#include "VxFPUControlWord.h"

inline void InitializePlayerArithmetic()
{
    // The original renderer selected PC24 before runtime managers were created.
    // SDL leaves the CRT precision unchanged; physics caches its energy damping
    // at initialization, so selecting the legacy word later is too late.
    VxSetBaseFPUControlWord();
}

#endif // PLAYER_RUNTIMESTARTUP_H
