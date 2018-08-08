#ifndef NATIVE_MAUTIME_H

#include <VMXTime.h>

#define NATIVE_MAUTIME_H

namespace mau {
    extern VMXTime* vmxTime;

    static uint64_t vmxGetTime() {
        return vmxTime->GetCurrentMicroseconds();
    }
}

#endif //NATIVE_MAUTIME_H
