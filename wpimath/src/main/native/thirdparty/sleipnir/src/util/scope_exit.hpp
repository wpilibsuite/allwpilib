// Copyright (c) Sleipnir contributors

#pragma once

#include <utility>

namespace slp {

template <typename F>
class scope_exit {
 public:
  explicit scope_exit(F&& f) noexcept : m_f{std::forward<F>(f)} {}

  ~scope_exit() {
    if (m_active) {
      m_f();
    }
  }

  scope_exit(scope_exit&& rhs) noexcept
      : m_f{std::move(rhs.m_f)}, m_active{rhs.m_active} {
    rhs.release();
  }

  scope_exit(const scope_exit&) = delete;
  scope_exit& operator=(const scope_exit&) = delete;

  void release() noexcept { m_active = false; }

 private:
  F m_f;
  bool m_active = true;
};

}  // namespace slp
