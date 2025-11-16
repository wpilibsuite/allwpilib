// Copyright (c) Sleipnir contributors

#pragma once

#include <concepts>
#include <utility>

#include "sleipnir/util/setup_profiler.hpp"
#include "sleipnir/util/solve_profiler.hpp"

namespace slp {

/**
 * Starts a profiler in the constructor and stops it in the destructor.
 */
template <typename Profiler>
  requires std::same_as<Profiler, SetupProfiler> ||
           std::same_as<Profiler, SolveProfiler>
class ScopedProfiler {
 public:
  /**
   * Starts a profiler.
   *
   * @param profiler The profiler.
   */
  explicit ScopedProfiler(Profiler& profiler) noexcept : m_profiler{&profiler} {
    m_profiler->start();
  }

  /**
   * Stops a profiler.
   */
  ~ScopedProfiler() {
    if (m_active) {
      m_profiler->stop();
    }
  }

  /**
   * Move constructor.
   *
   * @param rhs The other ScopedProfiler.
   */
  ScopedProfiler(ScopedProfiler&& rhs) noexcept
      : m_profiler{std::move(rhs.m_profiler)}, m_active{rhs.m_active} {
    rhs.m_active = false;
  }

  ScopedProfiler(const ScopedProfiler&) = delete;
  ScopedProfiler& operator=(const ScopedProfiler&) = delete;

  /**
   * Stops the profiler.
   *
   * If this is called, the destructor is a no-op.
   */
  void stop() {
    if (m_active) {
      m_profiler->stop();
      m_active = false;
    }
  }

  /**
   * Returns the most recent solve duration in milliseconds as a double.
   *
   * @return The most recent solve duration in milliseconds as a double.
   */
  const std::chrono::duration<double>& current_duration() const {
    return m_profiler->current_duration();
  }

 private:
  Profiler* m_profiler;
  bool m_active = true;
};

}  // namespace slp
