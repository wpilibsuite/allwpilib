// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Tracer.h"

#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <units/time.h>
#include <wpi/SmallString.h>
#include <wpi/StringMap.h>
#include <wpi/deprecated.h>
#include <wpi/raw_ostream.h>

#include "frc/Errors.h"
#include "frc/RobotController.h"

using namespace frc;

std::atomic<bool> singleThreadedMode = false;
std::atomic<bool> anyTracesStarted = false;

void PostUnits() {
  auto table = nt::NetworkTableInstance::GetDefault().GetTable("/Tracer");
  table->GetEntry("TimingUnits").SetString("Milliseconds", 0);
}

Tracer::TracerState::TracerState() {
  if (singleThreadedMode && anyTracesStarted) {
    FRC_ReportWarning("Cannot start a new trace in single-threaded mode");
    m_disabled = true;
  }
  if (!anyTracesStarted) {
    PostUnits();
  }
  auto inst = nt::NetworkTableInstance::GetDefault();
  m_rootTable = inst.GetTable(fmt::format(
      "/Tracer/{}", std::hash<std::thread::id>{}(std::this_thread::get_id())));
}

Tracer::TracerState::TracerState(std::string_view name) {
  auto inst = nt::NetworkTableInstance::GetDefault();
  m_rootTable = inst.GetTable(fmt::format("/Tracer/{}", name));
}

std::string Tracer::TracerState::BuildStack() {
  std::string stack = "";
  for (size_t i = 0; i < m_traceStack.size(); ++i) {
    stack += m_traceStack[i];
    if (i < m_traceStack.size() - 1) {
      stack += "/";
    }
  }
  return stack;
}

std::string Tracer::TracerState::AppendTraceStack(std::string_view trace) {
  m_stackSize++;
  if (m_disabled) {
    return "";
  }
  m_traceStack.push_back(trace);
  return BuildStack();
}

std::string Tracer::TracerState::PopTraceStack() {
  if (m_stackSize > 0) {
    m_stackSize--;
  }
  if (m_disabled) {
    return "";
  }
  if (m_traceStack.empty() || m_cyclePoisoned) {
    m_cyclePoisoned = true;
    return "";
  }
  std::string stack = BuildStack();
  m_traceStack.pop_back();
  return stack;
}

void Tracer::TracerState::EndCycle() {
  if (m_cyclePoisoned && !m_disabled) {
    // Gives publishers empty times,
    // reporting no data is better than bad data
    for (auto&& [_, publisher] : m_publishers) {
      publisher.Set(0.0);
    }
    return;
  } else if (!m_disabled) {
    // Update times for all already existing publishers,
    // pop trace times for keys with existing publishers
    for (auto&& [key, publisher] : m_publishers) {
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
      auto topic = m_rootTable->GetDoubleTopic(traceTime.first);
      if (auto publisher = topic.Publish(); publisher) {
        publisher.Set(traceTime.second.value());
        m_publishers.emplace_back(traceTime.first, std::move(publisher));
      }
    }
  }

  // Clean up state
  m_traceTimes.clear();

  m_disabled = m_disableNextCycle;
}

void Tracer::TracerState::UpdateThreadName(std::string_view name) {
  if (m_publishers.empty()) {
    auto inst = nt::NetworkTableInstance::GetDefault();
    m_rootTable = inst.GetTable(fmt::format("/Tracer/{}", name));
  } else {
    FRC_ReportWarning(
        "Cannot update Tracer thread name after traces have been started");
  }
}

thread_local std::shared_ptr<Tracer::TracerState> threadLocalState =
    std::make_shared<Tracer::TracerState>();

void Tracer::StartTrace(std::string_view name) {
  anyTracesStarted = true;
  // Call `AppendTraceStack` even if disabled to keep `m_stackSize` in sync
  std::string stack = threadLocalState->AppendTraceStack(name);
  if (!threadLocalState->m_disabled) {
    auto now =
        units::millisecond_t{frc::RobotController::GetFPGATime() / 1000.0};
    threadLocalState->m_traceStartTimes.insert_or_assign(stack, now);
  }
}

void Tracer::EndTrace() {
  // Call `PopTraceStack` even if disabled to keep `m_stackSize` in sync
  std::string stack = threadLocalState->PopTraceStack();
  if (!threadLocalState->m_disabled) {
    if (stack.empty()) {
      FRC_ReportWarning("Trace ended without starting");
      return;
    }
    auto now =
        units::millisecond_t{frc::RobotController::GetFPGATime() / 1000.0};
    auto startTime = threadLocalState->m_traceStartTimes.find(stack);
    threadLocalState->m_traceTimes.insert_or_assign(stack,
                                                    now - startTime->second);
  }
  // If the stack is empty, the cycle is over.
  // Cycles can be ended even if the tracer is disabled
  if (threadLocalState->m_stackSize == 0) {
    threadLocalState->EndCycle();
  }
}

void Tracer::EnableSingleThreadedMode() {
  if (anyTracesStarted) {
    FRC_ReportWarning(
        "Cannot enable single-threaded mode after traces have been started");
  } else {
    singleThreadedMode = true;
    auto inst = nt::NetworkTableInstance::GetDefault();
    threadLocalState->m_rootTable = inst.GetTable("/Tracer");
  }
}

void Tracer::DisableTracingForCurrentThread() {
  threadLocalState->m_disableNextCycle = true;
}

void Tracer::EnableTracingForCurrentThread() {
  threadLocalState->m_disableNextCycle = false;
  if (threadLocalState->m_stackSize == 0) {
    threadLocalState->m_disabled = false;
  }
}

void Tracer::SetThreadName(std::string_view name) {
  threadLocalState->UpdateThreadName(name);
}

void Tracer::TraceFunc(std::string_view name, std::function<void()> runnable) {
  StartTrace(name);
  runnable();
  EndTrace();
}

template <typename T>
T Tracer::TraceFunc(std::string_view name, std::function<T()> supplier) {
  StartTrace(name);
  T result = supplier();
  EndTrace();
  return result;
}

Tracer::SubstitutiveTracer::SubstitutiveTracer(std::string_view name) {
  m_state = std::make_shared<TracerState>(name);
  m_currentlySubbedIn = false;
}

Tracer::SubstitutiveTracer::~SubstitutiveTracer() {
  if (m_currentlySubbedIn) {
    SubOut();
  }
}

void Tracer::SubstitutiveTracer::SubIn() {
  if (!m_currentlySubbedIn) {
    m_state.swap(threadLocalState);
    m_currentlySubbedIn = true;
  }
}

void Tracer::SubstitutiveTracer::SubOut() {
  if (m_currentlySubbedIn) {
    m_state.swap(threadLocalState);
    m_currentlySubbedIn = false;
  }
}

void Tracer::SubstitutiveTracer::SubWith(std::function<void()> runnable) {
  SubIn();
  runnable();
  SubOut();
}

void Tracer::ResetForTest() {
  threadLocalState = std::make_shared<TracerState>();
  anyTracesStarted = false;
  singleThreadedMode = false;
}

// DEPRECATED CLASS INSTANCE METHODS
WPI_IGNORE_DEPRECATED
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
          "\t{}: {:.6f}s\n", epoch.first,
          duration_cast<microseconds>(epoch.second).count() / 1.0e6);
    }
  }
}
WPI_UNIGNORE_DEPRECATED
