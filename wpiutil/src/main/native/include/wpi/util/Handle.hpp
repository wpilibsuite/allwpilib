// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <concepts>

#include "wpi/util/Handle.h"

namespace wpi::util {
/**
 * A move-only C++ wrapper around a WPI handle.
 * Will free the handle if FreeFunction is provided
 */
template <typename CType, auto FreeFunction = nullptr,
          auto CInvalid = WPI_INVALID_HANDLE>
  requires std::same_as<decltype(FreeFunction), std::nullptr_t> ||
           std::invocable<decltype(FreeFunction), CType>
class Handle {
 public:
  Handle() = default;
  /*implicit*/ Handle(CType val) : m_handle(val) {}  // NOLINT

  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;

  Handle(Handle&& rhs) : m_handle(rhs.m_handle) {
    rhs.m_handle = static_cast<CType>(CInvalid);
  }

  Handle& operator=(Handle&& rhs) {
    if (this != &rhs) {
      if constexpr (std::invocable<decltype(FreeFunction), CType>) {
        if (m_handle != static_cast<CType>(CInvalid)) {
          FreeFunction(m_handle);
        }
      }
    }
    m_handle = rhs.m_handle;
    rhs.m_handle = static_cast<CType>(CInvalid);
    return *this;
  }

  ~Handle() {
    if constexpr (std::invocable<decltype(FreeFunction), CType>) {
      if (m_handle != static_cast<CType>(CInvalid)) {
        FreeFunction(m_handle);
      }
    }
  }

  operator CType() const { return m_handle; }  // NOLINT

 private:
  CType m_handle = static_cast<CType>(CInvalid);
};

}  // namespace wpi::util
