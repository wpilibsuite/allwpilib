// Copyright (c) Sleipnir contributors

#pragma once

#include <chrono>
#include <concepts>
#include <string>
#include <string_view>
#include <utility>

namespace slp {

/// Records the number of profiler measurements (start/stop pairs) and the
/// average duration between each start and stop call.
class SetupProfiler {
 public:
  /// Constructs a SetupProfiler.
  ///
  /// @param name Name of measurement to show in diagnostics.
  explicit SetupProfiler(std::string_view name) : m_name{name} {}

  /// Starts setup time measurement.
  void start() {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    m_setup_start_time = std::chrono::steady_clock::now();
#endif
  }

  /// Stops setup time measurement.
  void stop() {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    m_setup_stop_time = std::chrono::steady_clock::now();
    m_setup_duration = m_setup_stop_time - m_setup_start_time;
#endif
  }

  /// Returns name of measurement to show in diagnostics.
  ///
  /// @return Name of measurement to show in diagnostics.
  std::string_view name() const { return m_name; }

  /// Returns the setup duration in milliseconds as a double.
  ///
  /// @return The setup duration in milliseconds as a double.
  const std::chrono::duration<double>& duration() const {
    return m_setup_duration;
  }

 private:
  /// Name of measurement to show in diagnostics.
  std::string m_name;

  std::chrono::steady_clock::time_point m_setup_start_time;
  std::chrono::steady_clock::time_point m_setup_stop_time;
  std::chrono::duration<double> m_setup_duration{0.0};
};

/// Records the number of profiler measurements (start/stop pairs) and the
/// average duration between each start and stop call.
class SolveProfiler {
 public:
  /// Constructs a SolveProfiler.
  ///
  /// @param name Name of measurement to show in diagnostics.
  explicit SolveProfiler(std::string_view name) : m_name{name} {}

  /// Starts solve time measurement.
  void start() {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    m_current_solve_start_time = std::chrono::steady_clock::now();
#endif
  }

  /// Stops solve time measurement, increments the number of averages, and
  /// incorporates the latest measurement into the average.
  void stop() {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    m_current_solve_stop_time = std::chrono::steady_clock::now();
    m_current_solve_duration =
        m_current_solve_stop_time - m_current_solve_start_time;
    m_total_solve_duration += m_current_solve_duration;

    ++m_num_solves;
    m_average_solve_duration =
        (m_num_solves - 1.0) / m_num_solves * m_average_solve_duration +
        1.0 / m_num_solves * m_current_solve_duration;
#endif
  }

  /// Returns name of measurement to show in diagnostics.
  ///
  /// @return Name of measurement to show in diagnostics.
  std::string_view name() const { return m_name; }

  /// Returns the number of solves.
  ///
  /// @return The number of solves.
  int num_solves() const { return m_num_solves; }

  /// Returns the most recent solve duration in seconds.
  ///
  /// @return The most recent solve duration in seconds.
  const std::chrono::duration<double>& current_duration() const {
    return m_current_solve_duration;
  }

  /// Returns the average solve duration in seconds.
  ///
  /// @return The average solve duration in seconds.
  const std::chrono::duration<double>& average_duration() const {
    return m_average_solve_duration;
  }

  /// Returns the sum of all solve durations in seconds.
  ///
  /// @return The sum of all solve durations in seconds.
  const std::chrono::duration<double>& total_duration() const {
    return m_total_solve_duration;
  }

 private:
  /// Name of measurement to show in diagnostics.
  std::string m_name;

  std::chrono::steady_clock::time_point m_current_solve_start_time;
  std::chrono::steady_clock::time_point m_current_solve_stop_time;
  std::chrono::duration<double> m_current_solve_duration{0.0};
  std::chrono::duration<double> m_total_solve_duration{0.0};

  int m_num_solves = 0;
  std::chrono::duration<double> m_average_solve_duration{0.0};
};

/// Starts a profiler in the constructor and stops it in the destructor.
template <typename Profiler>
  requires std::same_as<Profiler, SetupProfiler> ||
           std::same_as<Profiler, SolveProfiler>
class ScopedProfiler {
 public:
  /// Starts a profiler.
  ///
  /// @param profiler The profiler.
  explicit ScopedProfiler(Profiler& profiler) noexcept : m_profiler{&profiler} {
    m_profiler->start();
  }

  /// Stops a profiler.
  ~ScopedProfiler() {
    if (m_active) {
      m_profiler->stop();
    }
  }

  /// Move constructor.
  ///
  /// @param rhs The other ScopedProfiler.
  ScopedProfiler(ScopedProfiler&& rhs) noexcept
      : m_profiler{std::move(rhs.m_profiler)}, m_active{rhs.m_active} {
    rhs.m_active = false;
  }

  ScopedProfiler(const ScopedProfiler&) = delete;
  ScopedProfiler& operator=(const ScopedProfiler&) = delete;

  /// Stops the profiler.
  ///
  /// If this is called, the destructor is a no-op.
  void stop() {
    if (m_active) {
      m_profiler->stop();
      m_active = false;
    }
  }

  /// Returns the most recent solve duration in milliseconds as a double.
  ///
  /// @return The most recent solve duration in milliseconds as a double.
  const std::chrono::duration<double>& current_duration() const {
    return m_profiler->current_duration();
  }

 private:
  Profiler* m_profiler;
  bool m_active = true;
};

}  // namespace slp
