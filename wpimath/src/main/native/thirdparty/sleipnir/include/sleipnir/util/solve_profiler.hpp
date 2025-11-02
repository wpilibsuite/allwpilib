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
class SolveProfiler {
 public:
  /**
   * Constructs a SolveProfiler.
   *
   * @param name Name of measurement to show in diagnostics.
   */
  explicit SolveProfiler(std::string_view name) : m_name{name} {}

  /**
   * Tell the profiler to start measuring solve time.
   */
  void start() {
#ifndef SLEIPNIR_DISABLE_DIAGNOSTICS
    m_current_solve_start_time = std::chrono::steady_clock::now();
#endif
  }

  /**
   * Tell the profiler to stop measuring solve time, increment the number of
   * averages, and incorporate the latest measurement into the average.
   */
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

  /**
   * Returns name of measurement to show in diagnostics.
   *
   * @return Name of measurement to show in diagnostics.
   */
  std::string_view name() const { return m_name; }

  /**
   * Returns the number of solves.
   *
   * @return The number of solves.
   */
  int num_solves() const { return m_num_solves; }

  /**
   * Returns the most recent solve duration in seconds.
   *
   * @return The most recent solve duration in seconds.
   */
  const std::chrono::duration<double>& current_duration() const {
    return m_current_solve_duration;
  }

  /**
   * Returns the average solve duration in seconds.
   *
   * @return The average solve duration in seconds.
   */
  const std::chrono::duration<double>& average_duration() const {
    return m_average_solve_duration;
  }

  /**
   * Returns the sum of all solve durations in seconds.
   *
   * @return The sum of all solve durations in seconds.
   */
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

}  // namespace slp
