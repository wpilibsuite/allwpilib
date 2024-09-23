// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Tracer.h"

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include "networktables/DoubleTopic.h"
#include <wpi/StringMap.h>
#include <units/time.h>
#include "frc/RobotController.h"

#include "frc/Errors.h"

using namespace frc;

class TracerState {
 public:
  TracerState() {
    if (singleThreadedMode && anyTracesStarted) {
      FRC_ReportWarning("Cannot start a new trace in single-threaded mode");
      m_disabled = true;
    }
    anyTracesStarted = true;
    auto inst = nt::NetworkTableInstance::GetDefault();
    m_rootTable = inst.GetTable(fmt::format("/Tracer/{}", std::hash<std::thread::id>{}(std::this_thread::get_id())));
  }

  std::string BuildStack() {
    std::string stack = "";
    for (size_t i = 0; i < m_traceStack.size(); ++i) {
      stack += m_traceStack[i];
      if (i < m_traceStack.size() - 1) {
        stack += "/";
      }
    }
    return stack;
  }

  std::string AppendTraceStack(std::string_view trace) {
    if (m_disabled) {
      m_stackSize++;
      return "";
    }
    m_traceStack.push_back(trace);
    return BuildStack();
  }

  std::string PopTraceStack() {
    if (m_disabled) {
      m_stackSize > 0 ? m_stackSize-- : m_stackSize;
      return "";
    }
    if (m_traceStack.empty() || m_cyclePoisened) {
      m_cyclePoisened = true;
      return "";
    }
    std::string stack = BuildStack();
    m_traceStack.pop_back();
    return stack;
  }

  void EndCycle() {
    if (m_disabled != m_disableNextCycle || m_cyclePoisened) {
      // Gives publishers empty times,
      // reporting no data is better than bad data
      for (auto&& [key, publisher] : m_publisherHeap) {
        publisher.Set(0.0);
      }
      return;
    } else {
      // Update times for all already existing publishers
      for (auto&& [key, publisher] : m_publisherHeap) {
        if (auto time = m_traceTimes.find(key); time != m_traceTimes.end()) {
          publisher.Set(time->second.value());
          m_traceTimes.erase(time);
        } else {
          publisher.Set(0.0);
        }
      }

      // Create publishers for all new entries, add them to the heap
      // and set their times
      for (auto&& traceTime : m_traceTimes) {
        auto topic = m_rootTable->GetDoubleTopic(traceTime.first());
        topic.SetCached(false);
        if (auto publisher = topic.Publish(); publisher) {
          publisher.Set(traceTime.second.value());
          m_publisherHeap.insert(
              std::make_pair(traceTime.first(), std::move(publisher))
          );
        }
      }
    }

    // Clean up state
    m_traceTimes.clear();

    m_disabled = m_disableNextCycle;
  }

  void UpdateThreadName(std::string_view name) {
    if (m_publisherHeap.empty()) {
      auto inst = nt::NetworkTableInstance::GetDefault();
      m_rootTable = inst.GetTable(fmt::format("/Tracer/{}", name));
    } else {
      FRC_ReportWarning("Cannot update Tracer thread name while traces are still active");
    }
  }

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
  wpi::StringMap<nt::DoublePublisher> m_publisherHeap;
  // If the cycle is poisened, it will warn the user
  // and not publish any data
  bool m_cyclePoisened = false;
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

static std::atomic<bool> singleThreadedMode = false;
static std::atomic<bool> anyTracesStarted = false;
static thread_local TracerState threadLocalState = TracerState();


static void StartTrace(std::string_view name) {
  // Call `AppendTraceStack` even if disabled to keep `m_stackSize` in sync
  std::string stack = threadLocalState.AppendTraceStack(name);
  if (!threadLocalState.m_disabled) {
      auto now = units::millisecond_t{frc::RobotController::GetFPGATime() / 1000.0};
    threadLocalState.m_traceStartTimes.insert_or_assign(stack, now);
  }
}

static void EndTrace() {
  // Call `PopTraceStack` even if disabled to keep `m_stackSize` in sync
  std::string stack = threadLocalState.PopTraceStack();
  if (!threadLocalState.m_disabled) {
    if (stack.empty()) {
      threadLocalState.m_cyclePoisened = true;
      FRC_ReportWarning("Trace ended without starting");
      return;
    }
    auto now = units::millisecond_t{frc::RobotController::GetFPGATime() / 1000.0};
    auto startTime = threadLocalState.m_traceStartTimes.find(stack);
    threadLocalState.m_traceTimes.insert_or_assign(stack, now - startTime->second);
  }
  // If the stack is empty, the cycle is over.
  // Cycles can be ended even if the tracer is disabled
  if (threadLocalState.m_stackSize == 0) {
    threadLocalState.EndCycle();
  }
}

static void EnableSingleThreadedMode() {
  if (anyTracesStarted) {
    FRC_ReportWarning("Cannot enable single-threaded mode after traces have been started");
  } else {
    singleThreadedMode = true;
    auto inst = nt::NetworkTableInstance::GetDefault();
    threadLocalState.m_rootTable = inst.GetTable("/Tracer");
  }
}

static void DisableTracingForCurrentThread() {
  threadLocalState.m_disableNextCycle = true;
}

static void EnableTracingForCurrentThread() {
  threadLocalState.m_disableNextCycle = false;
}

static void TraceFunc(std::string_view name, std::function<void()> runnable) {
  StartTrace(name);
  runnable();
  EndTrace();
}

template <typename T>
static T TraceFunc(std::string_view name, std::function<T()> supplier) {
  StartTrace(name);
  T result = supplier();
  EndTrace();
  return result;
}



// DEPRECATED CLASS INSTANCE METHODS
Tracer::Tracer() {
  ResetTimer();
}

void Tracer::ResetTimer() {
  m_startTime = hal::fpga_clock::now();
}

void Tracer::ClearEpochs() {
  ResetTimer();
  m_epochs.clear();
}

void Tracer::AddEpoch(std::string_view epochName) {
  auto currentTime = hal::fpga_clock::now();
  m_epochs[epochName] = currentTime - m_startTime;
  m_startTime = currentTime;
}

void Tracer::PrintEpochs() {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream os(buf);
  PrintEpochs(os);
  if (!buf.empty()) {
    FRC_ReportWarning("{}", buf.c_str());
  }
}

void Tracer::PrintEpochs(wpi::raw_ostream& os) {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;

  auto now = hal::fpga_clock::now();
  if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
    m_lastEpochsPrintTime = now;
    for (const auto& epoch : m_epochs) {
      os << fmt::format(
          "\t{}: {:.6f}s\n", epoch.getKey(),
          duration_cast<microseconds>(epoch.getValue()).count() / 1.0e6);
    }
  }
}