// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/ErrorHandling.hpp"

#include <fmt/format.h>

#include "wpi/hal/Errors.h"
#include "wpi/hal/HAL.h"
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
HAL_Status MakeError(HAL_Status status, std::string_view value) {
  LastErrorStorage& lastError = GetThreadLastError();
  lastError.message = value;
  lastError.status = status;
  return HAL_USE_LAST_ERROR;
}

HAL_Status MakeErrorIndexOutOfRange(HAL_Status status, std::string_view message,
                                    int32_t minimum, int32_t maximum,
                                    int32_t requested) {
  return MakeError(
      status,
      fmt::format("{}\n Status: {}\n  Minimum: {} Maximum: {} Requested: {}",
                  message, status, minimum, maximum, requested));
}

HAL_Status MakeErrorPreviouslyAllocated(HAL_Status status,
                                        std::string_view message,
                                        int32_t channel,
                                        std::string_view previousAllocation) {
  return MakeError(status,
                   fmt::format("{} {} previously allocated.\n"
                               "Location of the previous allocation:\n{}\n"
                               "Location of the current allocation:",
                               message, channel, previousAllocation));
}
}  // namespace wpi::hal

extern "C" {
const char* HAL_GetLastError(HAL_Status* status) {
  if (*status == HAL_USE_LAST_ERROR) {
    LastErrorStorage& lastError = GetThreadLastError();
    *status = lastError.status;
    return lastError.message.c_str();
  }
  return HAL_GetErrorMessage(*status);
}
}  // extern "C"
