// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <type_traits>
#include <utility>

#include "wpi/drivers/motor/A301Error.hpp"

namespace wpi {

/** A value read from an A301 periodic status frame. */
template <typename T>
class A301StatusSignal {
 public:
  /** Constructs an A301 status signal. */
  A301StatusSignal(T value, int32_t status, uint64_t timestamp)
      : m_value{std::move(value)},
        m_status{status},
        m_timestamp{timestamp} {}

  /** Returns the most recently received value. */
  T Get() const { return m_value; }

  /** Returns the value when valid, or the supplied default value otherwise. */
  T Get(T defaultValue) const {
    return IsValid() ? m_value : std::move(defaultValue);
  }

  /** Returns whether the value was read without an error. */
  bool IsValid() const { return GetError() == A301Error::kOk; }

  /** Returns the REVLib-compatible error associated with the read. */
  A301Error GetError() const {
    return detail::A301ErrorFromHalStatus(m_status);
  }

  /** Returns the underlying HAL status. */
  int32_t GetStatus() const { return m_status; }

  /** Returns the CAN frame timestamp in microseconds. */
  uint64_t GetTimestamp() const { return m_timestamp; }

  /** Maps the value while retaining its status and timestamp. */
  template <typename F>
  auto Map(F&& mapper) const
      -> A301StatusSignal<std::invoke_result_t<F, const T&>> {
    using U = std::invoke_result_t<F, const T&>;
    return A301StatusSignal<U>{std::invoke(std::forward<F>(mapper), m_value),
                               m_status, m_timestamp};
  }

 private:
  T m_value{};
  int32_t m_status{};
  uint64_t m_timestamp{};
};

}  // namespace wpi
