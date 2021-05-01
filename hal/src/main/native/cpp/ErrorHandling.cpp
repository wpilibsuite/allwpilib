// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/SmallString.h>
#include <wpi/Twine.h>

#include "hal/Errors.h"
#include "hal/HALBase.h"

namespace {
struct LastErrorStorage {
  int32_t status;
  wpi::SmallString<512> message;
};
}  // namespace

static LastErrorStorage& GetThreadLastError() {
  thread_local LastErrorStorage lastError;
  return lastError;
}

namespace hal {
void SetLastError(int32_t* status, const wpi::Twine& value) {
  LastErrorStorage& lastError = GetThreadLastError();
  lastError.message.clear();
  value.toVector(lastError.message);
  lastError.status = *status;
  *status = HAL_USE_LAST_ERROR;
}

void SetLastErrorIndexOutOfRange(int32_t* status, const wpi::Twine& message,
                                 int32_t minimum, int32_t maximum,
                                 int32_t requested) {
  SetLastError(status, message + "\n Status: " + wpi::Twine(*status) +
                           "\n  Minimum: " + wpi::Twine(minimum) +
                           " Maximum: " + wpi::Twine(maximum) +
                           " Reequested: " + wpi::Twine(requested));
}

void SetLastErrorPreviouslyAllocated(int32_t* status, const wpi::Twine& message,
                                     int32_t channel,
                                     const wpi::Twine& previousAllocation) {
  hal::SetLastError(
      status,
      message + " " + wpi::Twine(channel) +
          " previously allocated.\nLocation of the previous allocation:\n" +
          previousAllocation + "\nLocation of the current allocation:");
}
}  // namespace hal

extern "C" {
const char* HAL_GetLastError(int32_t* status) {
  if (*status == HAL_USE_LAST_ERROR) {
    LastErrorStorage& lastError = GetThreadLastError();
    *status = lastError.status;
    return lastError.message.c_str();
  }
  return HAL_GetErrorMessage(*status);
}
}  // extern "C"
