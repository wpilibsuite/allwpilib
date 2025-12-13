// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/format.h>

#include "wpi/hal/Errors.h"
#include "wpi/hal/HALBase.h"
#include "wpi/util/SmallString.hpp"

namespace {
struct LastErrorStorage {
  int32_t status;
  wpi::util::SmallString<512> message;
};
}  // namespace

static LastErrorStorage& GetThreadLastError() {
  thread_local LastErrorStorage lastError;
  return lastError;
}

namespace wpi::hal {
void SetLastError(int32_t* status, std::string_view value) {
  LastErrorStorage& lastError = GetThreadLastError();
  lastError.message = value;
  lastError.status = *status;
  *status = HAL_USE_LAST_ERROR;
}

void SetLastErrorIndexOutOfRange(int32_t* status, std::string_view message,
                                 int32_t minimum, int32_t maximum,
                                 int32_t requested) {
  SetLastError(
      status,
      fmt::format("{}\n Status: {}\n  Minimum: {} Maximum: {} Requested: {}",
                  message, *status, minimum, maximum, requested));
}

void SetLastErrorPreviouslyAllocated(int32_t* status, std::string_view message,
                                     int32_t channel,
                                     std::string_view previousAllocation) {
  wpi::hal::SetLastError(
      status, fmt::format("{} {} previously allocated.\n"
                          "Location of the previous allocation:\n{}\n"
                          "Location of the current allocation:",
                          message, channel, previousAllocation));
}
}  // namespace wpi::hal

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
