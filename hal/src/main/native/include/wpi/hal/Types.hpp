// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/Types.h"

namespace wpi::hal {
/**
 * A move-only C++ wrapper around a HAL handle.
 * Will free the handle if FreeFunction is provided
 */
template <typename CType, void (*FreeFunction)(CType) = nullptr,
          int32_t CInvalid = HAL_kInvalidHandle>
class Handle {
 public:
  Handle() = default;
  /*implicit*/ Handle(CType val) : m_handle(val) {}  // NOLINT

  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;

  Handle(Handle&& rhs) : m_handle(rhs.m_handle) { rhs.m_handle = CInvalid; }

  Handle& operator=(Handle&& rhs) {
    if (this != &rhs) {
// FIXME: GCC gives the false positive "the address of <GetDefault> will never
// be NULL" because it doesn't realize the default template parameter can make
// GetDefault nullptr. Fixed in GCC 13.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=94554
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105885
#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
#endif  // __GNUC__
      if constexpr (FreeFunction != nullptr) {
#if __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__
        if (m_handle != CInvalid) {
          FreeFunction(m_handle);
        }
      }
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = CInvalid;
    return *this;
  }

  ~Handle() {
// FIXME: GCC gives the false positive "the address of <GetDefault> will never
// be NULL" because it doesn't realize the default template parameter can make
// GetDefault nullptr. Fixed in GCC 13.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=94554
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105885
#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
#endif  // __GNUC__
    if constexpr (FreeFunction != nullptr) {
#if __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__
      if (m_handle != CInvalid) {
        FreeFunction(m_handle);
      }
    }
  }

  operator CType() const { return m_handle; }  // NOLINT

 private:
  CType m_handle = CInvalid;
};

}  // namespace wpi::hal
