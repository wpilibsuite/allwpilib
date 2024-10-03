// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <hal/cpp/fpga_clock.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTable.h>
#include <units/time.h>
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
   * This is not to be used by user code.
   *
   * This manages the state of the Tracer for a single thread.
   */
  class TracerState {
   public:
    TracerState();
    explicit TracerState(std::string_view name);

    std::string BuildStack();
    std::string AppendTraceStack(std::string_view trace);
    std::string PopTraceStack();
    void EndCycle();
    void UpdateThreadName(std::string_view name);

    // The network table that all data is published to
    std::shared_ptr<nt::NetworkTable> m_rootTable;
    // The stack of trace frames, every startTrace will add to this stack
    // and every endTrace will remove from this stack
    std::vector<std::string_view> m_traceStack;
    // A map of trace names to the time they took to execute
    wpi::StringMap<units::millisecond_t> m_traceTimes;
    // A map of trace names to the time they started
    wpi::StringMap<units::millisecond_t> m_traceStartTimes;
    // A collection of all publishers that have been created,
    // this makes updating the times of publishers much easier and faster
    std::vector<std::pair<std::string_view, nt::DoublePublisher>> m_publishers;
    // If the cycle is poisoned, it will warn the user
    // and not publish any data
    bool m_cyclePoisoned = false;
    // If the tracer is disabled, it will not publish any data
    // or do any string manipulation
    bool m_disabled = false;
    // If the tracer should be disabled next cycle
    // and every cycle after that until this flag is set to false.
    // Disabling is done this way to prevent disabling/enabling
    // in the middle of a cycle
    bool m_disableNextCycle = false;
    // stack size is used to keep track of stack size
    // even when disabled, calling `EndCycle` is important when
    // disabled or not to update the disabled state in a safe manner
    uint32_t m_stackSize = 0;
  };

  /**
   * Starts a trace,
   * should be called at the beginning of a function that's not being called by
   * user code.
   * Should be paired with `Tracer.endTrace()` at the end of the function.
   *
   * @param name the name of the trace, should be unique to the function.
   */
  static void StartTrace(std::string_view name);

  /**
   * Ends a trace, should only be called at the end of a function that's not
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
   *
   * <p>Being disabled prevents the `Tracer` from publishing any values to
   * NetworkTables. This will cause all values to appear as if they're frozen at
   * the value they were at when this function was called.
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
   * A RAII class for starting and ending traces.
   */
  class ScopedTraceHandle {
   public:
    explicit ScopedTraceHandle(std::string_view name) { StartTrace(name); }
    ~ScopedTraceHandle() { EndTrace(); }
  };

  /**
   * Starts a trace,
   * should be called at the beginning of a function that's not being called by
   * user code.
   * This is a RAII version of `Tracer.startTrace(string)`,
   * and will automatically call `Tracer.endTrace()` when the scope ends and
   * the instance gets destroyed.
   *
   * @param name the name of the trace, should be unique to the function.
   */
  static ScopedTraceHandle StartScopedTrace(std::string_view name) {
    return ScopedTraceHandle{name};
  }

  /**
   * A class to help with substituting the current tracer state with a new one.
   *
   * This can be used to act as if code is running on another thread even
   * if it's running synchronously on the same thread.
   */
  class SubstitutiveTracer {
   public:
    explicit SubstitutiveTracer(std::string_view name);
    ~SubstitutiveTracer();

    /**
     * Substitutes the current tracer state with the new one.
     */
    void SubIn();
    /**
     * Substitutes the current tracer state with the original one.
     */
    void SubOut();
    /**
     * Runs the given runnable with the new tracer state.
     *
     * @param runnable the runnable to run.
     */
    void SubWith(std::function<void()> runnable);

   private:
    std::shared_ptr<TracerState> m_state;
    bool m_currentlySubbedIn;
  };

  /**
   * Resets the internal state of the Tracer.
   * This should only be used for testing purposes.
   */
  static void ResetForTest();

  // DEPRECATED CLASS INSTANCE METHODS

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
