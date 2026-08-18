//===- IOSandbox.h ----------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_IOSANDBOX_H
#define WPIUTIL_WPI_IOSANDBOX_H

#if defined(LLVM_ENABLE_IO_SANDBOX) && LLVM_ENABLE_IO_SANDBOX

#include "wpi/util/Compiler.hpp"
#include "wpi/util/ErrorHandling.hpp"
#include "wpi/util/SaveAndRestore.hpp"

namespace wpi::util::sys::sandbox {
inline LLVM_THREAD_LOCAL bool Enabled = false;
struct ScopedSetting {
  SaveAndRestore<bool> Impl;
};
inline ScopedSetting scopedEnable() { return {{Enabled, true}}; }
inline ScopedSetting scopedDisable() { return {{Enabled, false}}; }
inline void violationIfEnabled() {
  if (Enabled)
    reportFatalInternalError("IO sandbox violation");
}
} // namespace wpi::util::sys::sandbox

#else

namespace wpi::util::sys::sandbox {
struct [[maybe_unused]] ScopedSetting {};
inline ScopedSetting scopedEnable() { return {}; }
inline ScopedSetting scopedDisable() { return {}; }
inline void violationIfEnabled() {}
} // namespace wpi::util::sys::sandbox

#endif

#endif
