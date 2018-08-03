/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>

#include "MauTime.h"
#include "MauClockInternal.h"

static std::atomic<bool> programStarted{false};

static std::atomic<uint64_t> programStartTime{0};

namespace hal {
    namespace init {
        void InitializeMockHooks() {}
    }
}

void Mau_restartTiming() {
    programStartTime = mau::vmxGetTime();
}

int64_t Mau_getTime() {
    auto now = mau::vmxGetTime();
    auto currentTime = now - programStartTime;
    return currentTime;
}

double Mau_getTimestamp() { 
    return Mau_getTime() * 1.0e-6; 
}

void Mau_getProgramStarted() { 
    programStarted = true; 
}
