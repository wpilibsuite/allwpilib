// Copyright (c) Sleipnir contributors

#pragma once

#include <chrono>

#include "sleipnir/util/SymbolExports.hpp"

namespace sleipnir {

/**
 * Records the number of profiler measurements (start/stop pairs) and the
 * average duration between each start and stop call.
 */
class SLEIPNIR_DLLEXPORT Profiler {
 public:
  /**
   * Tell the profiler to start measuring setup time.
   */
  void StartSetup() { m_setupStartTime = std::chrono::system_clock::now(); }

  /**
   * Tell the profiler to stop measuring setup time.
   */
  void StopSetup() {
    m_setupDuration = std::chrono::system_clock::now() - m_setupStartTime;
  }

  /**
   * Tell the profiler to start measuring solve time.
   */
  void StartSolve() { m_solveStartTime = std::chrono::system_clock::now(); }

  /**
   * Tell the profiler to stop measuring solve time, increment the number of
   * averages, and incorporate the latest measurement into the average.
   */
  void StopSolve() {
    auto now = std::chrono::system_clock::now();
    ++m_solveMeasurements;
    m_averageSolveDuration =
        (m_solveMeasurements - 1.0) / m_solveMeasurements *
            m_averageSolveDuration +
        1.0 / m_solveMeasurements * (now - m_solveStartTime);
  }

  /**
   * The setup duration in milliseconds as a double.
   */
  double SetupDuration() const {
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;
    return duration_cast<nanoseconds>(m_setupDuration).count() / 1e6;
  }

  /**
   * The number of solve measurements taken.
   */
  int SolveMeasurements() const { return m_solveMeasurements; }

  /**
   * The average solve duration in milliseconds as a double.
   */
  double AverageSolveDuration() const {
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;
    return duration_cast<nanoseconds>(m_averageSolveDuration).count() / 1e6;
  }

 private:
  std::chrono::system_clock::time_point m_setupStartTime;
  std::chrono::duration<double> m_setupDuration{0.0};

  int m_solveMeasurements = 0;
  std::chrono::duration<double> m_averageSolveDuration{0.0};
  std::chrono::system_clock::time_point m_solveStartTime;
};

}  // namespace sleipnir
