// Copyright (c) Sleipnir contributors

#pragma once

#include <chrono>
#include <string>
#include <string_view>

namespace slp {

/**
 * Records the number of profiler measurements (start/stop pairs) and the
 * average duration between each start and stop call.
 */
class SetupProfiler {
 public:
  /**
   * Constructs a SetupProfiler.
   *
   * @param name Name of measurement to show in diagnostics.
   */
  explicit SetupProfiler(std::string_view name) : m_name{name} {}

  /**
   * Tell the profiler to start measuring setup time.
   */
  void start() {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    m_setup_start_time = std::chrono::steady_clock::now();
#endif
  }

  /**
   * Tell the profiler to stop measuring setup time.
   */
  void stop() {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    m_setup_stop_time = std::chrono::steady_clock::now();
    m_setup_duration = m_setup_stop_time - m_setup_start_time;
#endif
  }

  /**
   * Returns name of measurement to show in diagnostics.
   *
   * @return Name of measurement to show in diagnostics.
   */
  std::string_view name() const { return m_name; }

  /**
   * Returns the setup duration in milliseconds as a double.
   *
   * @return The setup duration in milliseconds as a double.
   */
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

}  // namespace slp
