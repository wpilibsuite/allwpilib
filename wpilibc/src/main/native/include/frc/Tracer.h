// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <functional>
#include <string_view>

#include <hal/cpp/fpga_clock.h>
#include <wpi/StringMap.h>

namespace wpi {
class raw_ostream;
}  // namespace wpi

namespace frc {
/**
 * A class for keeping track of how much time it takes for different parts of
 * code to execute. This is done with epochs, that are added to calls to
 * AddEpoch() and can be printed with a call to PrintEpochs().
 *
 * Epochs are a way to partition the time elapsed so that when overruns occur,
 * one can determine which parts of an operation consumed the most time.
 */
class Tracer {
 public:
  /**
   * Starts a trace,
   * should be called at the beginning of a function thats not being called by
   * user code.
   * Should be paired with `Tracer.endTrace()` at the end of the function.
   *
   * Best used in periodic functions in Subsystems and Robot.java.
   *
   * @param name the name of the trace, should be unique to the function.
   */
  static void StartTrace(std::string_view name);

  /**
   * Ends a trace, should only be called at the end of a function thats not
   * being called by user code. If a `Tracer.startTrace(string)` is not paired
   * with a `Tracer.endTrace()` there will be timing issues and dropped timing
   * data.
   */
  static void EndTrace();

  /**
   * Enables single threaded mode for the Tracer.
   * This will cause traces on different threads to throw an exception.
   * This will shorten the path of traced data in NetworkTables by not including
   * the thread name.
   *
   * @warning This will do nothing if called after a trace has been started.
   */
  static void EnableSingleThreadedMode();

  /**
   * Disables any tracing for the current thread.
   * This will cause all `Tracer.startTrace(string)`, `Tracer.endTrace()`
   * and `Tracer.TraceFunc(function)` to do nothing.
   */
  static void DisableTracingForCurrentThread();

  /**
   * Enables tracing for the current thread.
   * This will only need to be called if
   * `Tracer.DisableTracingForCurrentThread()` has been called.
   */
  static void EnableTracingForCurrentThread();

  /**
   * Sets the name for the current thread to be used in the trace.
   *
   * @param name the name of the thread.
   */
  static void SetThreadName(std::string_view name);

  /**
   * Traces a function, should be used in place of
   * `Tracer.StartTrace(string)` and `Tracer.EndTrace()`
   * for functions called by user code like `CommandScheduler.run()` and
   * other expensive functions.
   *
   * @param name     the name of the trace, should be unique to the function.
   * @param runnable the function to trace.
   */
  static void TraceFunc(std::string_view name, std::function<void()> runnable);

  /**
   * Traces a function, should be used in place of
   * `Tracer.startTrace(string)` and `Tracer.endTrace()`
   * for functions called by user code like `CommandScheduler.run()` and
   * other expensive functions.
   *
   * @param name     the name of the trace, should be unique to the function.
   * @param supplier the function to trace.
   */
  template <typename T>
  static T TraceFunc(std::string_view name, std::function<T()> supplier);

  /**
   * Constructs a `Tracer` compatible with the 2024 `Tracer`.
   *
   * @deprecated This constructor is only for compatibility with the 2024 Tracer
   * and will be removed in 2025. Use the static methods in Tracer instead.
   */
  [[deprecated(
      "This constructor is only for compatibility with the 2024 Tracer and "
      "will be removed in 2025. Use the static methods in Tracer instead.")]]
  Tracer();

  /**
   * Restarts the epoch timer.
   *
   * @deprecated This method is only for compatibility with the 2024 Tracer and
   * will be removed in 2025. Use the static methods in Tracer instead.
   */
  [[deprecated(
      "This method is only for compatibility with the 2024 Tracer and will be "
      "removed in 2025. Use the static methods in Tracer instead.")]]
  void ResetTimer();

  /**
   * Clears all epochs.
   *
   * @deprecated This method is only for compatibility with the 2024 Tracer and
   * will be removed in 2025. Use the static methods in Tracer instead.
   */
  [[deprecated(
      "This method is only for compatibility with the 2024 Tracer and will be "
      "removed in 2025. Use the static methods in Tracer instead.")]]
  void ClearEpochs();

  /**
   * Adds time since last epoch to the list printed by PrintEpochs().
   *
   * Epochs are a way to partition the time elapsed so that when overruns occur,
   * one can determine which parts of an operation consumed the most time.
   *
   * @param epochName The name to associate with the epoch.
   *
   * @deprecated This method is only for compatibility with the 2024 Tracer and
   * will be removed in 2025. Use the static methods in Tracer instead.
   */
  [[deprecated(
      "This method is only for compatibility with the 2024 Tracer and will be "
      "removed in 2025. Use the static methods in Tracer instead.")]]
  void AddEpoch(std::string_view epochName);

  /**
   * Prints list of epochs added so far and their times to the DriverStation.
   *
   * @deprecated This method is only for compatibility with the 2024 Tracer and
   * will be removed in 2025. Use the static methods in Tracer instead.
   */
  [[deprecated(
      "This method is only for compatibility with the 2024 Tracer and will be "
      "removed in 2025. Use the static methods in Tracer instead.")]]
  void PrintEpochs();

  /**
   * Prints list of epochs added so far and their times to a stream.
   *
   * @param os output stream
   *
   * @deprecated This method is only for compatibility with the 2024 Tracer and
   * will be removed in 2025. Use the static methods in Tracer instead.
   */
  [[deprecated(
      "This method is only for compatibility with the 2024 Tracer and will be "
      "removed in 2025. Use the static methods in Tracer instead.")]]
  void PrintEpochs(wpi::raw_ostream& os);

 private:
  static constexpr std::chrono::milliseconds kMinPrintPeriod{1000};

  hal::fpga_clock::time_point m_startTime;
  hal::fpga_clock::time_point m_lastEpochsPrintTime = hal::fpga_clock::epoch();

  wpi::StringMap<std::chrono::nanoseconds> m_epochs;
};

}  // namespace frc
