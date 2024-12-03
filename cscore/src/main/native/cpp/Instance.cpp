// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Instance.h"

#include <memory>
#include <string_view>
#include <utility>

#include <fmt/format.h>
#include <wpi/fs.h>
#include <wpi/print.h>

using namespace cs;

static void def_log_func(unsigned int level, const char* file,
                         unsigned int line, const char* msg) {
  if (level == 20) {
    wpi::print(stderr, "CS: {}\n", msg);
    return;
  }

  std::string_view levelmsg;
  if (level >= 50) {
    levelmsg = "CRITICAL";
  } else if (level >= 40) {
    levelmsg = "ERROR";
  } else if (level >= 30) {
    levelmsg = "WARNING";
  } else {
    return;
  }
  wpi::print(stderr, "CS: {}: {} ({}:{})\n", levelmsg, msg,
             // NOLINTNEXTLINE(build/include_what_you_use)
             fs::path{file}.filename().string(), line);
}

Instance::Instance()
    : telemetry(notifier),
      networkListener(logger, notifier),
      usbCameraListener(logger, notifier) {
  SetDefaultLogger();
}

Instance::~Instance() = default;

Instance& Instance::GetInstance() {
  static Instance* inst = new Instance;
  return *inst;
}

void Instance::Shutdown() {
  eventLoop.Stop();
  m_sinks.FreeAll();
  m_sources.FreeAll();
  networkListener.Stop();
  usbCameraListener.Stop();
  telemetry.Stop();
  notifier.Stop();
}

void Instance::SetDefaultLogger() {
  logger.SetLogger(def_log_func);
}

std::pair<CS_Source, std::shared_ptr<SourceData>> Instance::FindSource(
    const SourceImpl& source) {
  return m_sources.FindIf(
      [&](const SourceData& data) { return data.source.get() == &source; });
}

std::pair<CS_Sink, std::shared_ptr<SinkData>> Instance::FindSink(
    const SinkImpl& sink) {
  return m_sinks.FindIf(
      [&](const SinkData& data) { return data.sink.get() == &sink; });
}

CS_Source Instance::CreateSource(CS_SourceKind kind,
                                 std::shared_ptr<SourceImpl> source) {
  auto handle = m_sources.Allocate(kind, source);
  notifier.NotifySource(source->GetName(), handle, CS_SOURCE_CREATED);
  source->Start();
  return handle;
}

CS_Sink Instance::CreateSink(CS_SinkKind kind, std::shared_ptr<SinkImpl> sink) {
  auto handle = m_sinks.Allocate(kind, sink);
  notifier.NotifySink(sink->GetName(), handle, CS_SINK_CREATED);
  return handle;
}

void Instance::DestroySource(CS_Source handle) {
  if (auto data = m_sources.Free(handle)) {
    notifier.NotifySource(data->source->GetName(), handle, CS_SOURCE_DESTROYED);
  }
}

void Instance::DestroySink(CS_Sink handle) {
  if (auto data = m_sinks.Free(handle)) {
    if (auto source = data->sink->GetSource()) {
      source->Wakeup();
    }
    notifier.NotifySink(data->sink->GetName(), handle, CS_SINK_DESTROYED);
  }
}
