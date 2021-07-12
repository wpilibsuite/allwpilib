// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Processes.h"

#include <unistd.h>

#include <cstdlib>

#include <fmt/format.h>

#include "hal/Errors.h"

namespace {
class UidSetter {
 public:
  explicit UidSetter(uid_t uid) {
    m_uid = getuid();
    if (setuid(uid) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "setuid(0) failed");
    }
  }

  ~UidSetter() noexcept(false) {
    if (setuid(m_uid) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "Restoring uid failed");
    }
  }

 private:
  uid_t m_uid;
};
}  // namespace

extern "C" {

HAL_Bool HAL_SetProcessPriority(const char* process, HAL_Bool realTime,
                                int32_t priority, int32_t* status) {
  UidSetter uidSetter{0};

  if (!realTime) {
    // Set scheduling policy to SCHED_OTHER
    if (std::system(
            fmt::format("pgrep '{}' | xargs chrt -o", process).c_str()) != 0) {
      *status = HAL_THREAD_PRIORITY_ERROR;
      return false;
    }
  } else {
    // Set scheduling policy to SCHED_FIFO with the given priority
    if (std::system(
            fmt::format("pgrep '{}' | xargs chrt -f -p {}", process, priority)
                .c_str()) != 0) {
      *status = HAL_THREAD_PRIORITY_ERROR;
      return false;
    }
  }

  *status = 0;
  return true;
}

}  // extern "C"
