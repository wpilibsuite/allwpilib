#ifndef NATIVE_MAUTIMEINTERNAL_H

#include "MockData/MockHooks.h"
#include "MauInternal.h"
#include <stdint.h>

#define NATIVE_MAUTIMEINTERNAL_H

void Mau_restartTiming();

int64_t Mau_getTime();

double Mau_getTimestamp();

void Mau_getProgramStarted();


#endif //NATIVE_MAUTIMEINTERNAL_H
