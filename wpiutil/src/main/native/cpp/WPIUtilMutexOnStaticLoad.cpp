// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifdef _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
#undef _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
#endif

#include <mutex>

namespace {
static struct StaticLoadMutexTest {
  StaticLoadMutexTest() {
    std::mutex mutex;
    std::scoped_lock lock{mutex};
  }
} Test;
}  // namespace

namespace wpi::detail {
void PingStaticLoader() {}
}  // namespace wpi::detail
