// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Errors.h"
#include "hal/HALBase.h"
#include "wpi/Twine.h"
#include "wpi/SmallString.h"

namespace {
struct LastErrorStorage {
    int32_t status;
    wpi::SmallString<512> message;
};
}

static LastErrorStorage& GetThreadLastError() {
    thread_local LastErrorStorage lastError;
    return lastError;
}

namespace hal {
void SetLastError(int32_t status, const wpi::Twine& value) {
    LastErrorStorage& lastError = GetThreadLastError();
    lastError.message.clear();
    value.toVector(lastError.message);
    lastError.status = status;
}
} // namespace hal

extern "C" {
const char* HAL_GetLastError(int32_t* status) {
    if (*status == HAL_USE_LAST_ERROR) {
        LastErrorStorage& lastError = GetThreadLastError();
        *status = lastError.status;
        return lastError.message.c_str();
    }
    return HAL_GetErrorMessage(*status);
}
}
