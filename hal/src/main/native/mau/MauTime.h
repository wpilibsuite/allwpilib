#ifndef NATIVE_MAUTIME_H

#include "MauInternal.h"

#define NATIVE_MAUTIME_H

namespace mau {
    static uint64_t vmxGetTime() {
        return vmxTime->GetCurrentMicroseconds();
    }
}

#endif //NATIVE_MAUTIME_H
