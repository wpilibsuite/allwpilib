// Copyright (c) Sleipnir contributors

#pragma once

#include <utility>

namespace slp {

/// scope_exit is a general-purpose scope guard intended to call its exit
/// function when a scope is exited.
///
/// @tparam F Function type.
template <typename F>
class scope_exit {
 public:
  /// Constructs a scope_exit.
  ///
  /// @param f Function to call on scope exit.
  explicit scope_exit(F&& f) noexcept : m_f{std::forward<F>(f)} {}

  ~scope_exit() {
    if (m_active) {
      m_f();
    }
  }

  /// Move constructor.
  ///
  /// @param rhs scope_exit from which to move.
  scope_exit(scope_exit&& rhs) noexcept
      : m_f{std::move(rhs.m_f)}, m_active{rhs.m_active} {
    rhs.release();
  }

  scope_exit(const scope_exit&) = delete;
  scope_exit& operator=(const scope_exit&) = delete;

  /// Makes the scope_exit inactive.
  void release() noexcept { m_active = false; }

 private:
  F m_f;
  bool m_active = true;
};

}  // namespace slp
