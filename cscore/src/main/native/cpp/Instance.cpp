/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Instance.h"

#include <wpi/EventLoopRunner.h>
#include <wpi/Logger.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>

#include "Log.h"
#include "NetworkListener.h"
#include "Notifier.h"
#include "SinkImpl.h"
#include "SourceImpl.h"
#include "Telemetry.h"
#include "UnlimitedHandleResource.h"

using namespace cs;

static void def_log_func(unsigned int level, const char* file,
                         unsigned int line, const char* msg) {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream oss(buf);
  if (level == 20) {
    oss << "CS: " << msg << '\n';
    wpi::errs() << oss.str();
    return;
  }

  wpi::StringRef levelmsg;
  if (level >= 50)
    levelmsg = "CRITICAL: ";
  else if (level >= 40)
    levelmsg = "ERROR: ";
  else if (level >= 30)
    levelmsg = "WARNING: ";
  else
    return;
  oss << "CS: " << levelmsg << msg << " (" << wpi::sys::path::filename(file)
      << ':' << line << ")\n";
  wpi::errs() << oss.str();
}

class Instance::Impl {
 public:
  Impl() : telemetry(notifier), networkListener(logger, notifier) {}

  wpi::Logger logger;
  Notifier notifier;
  Telemetry telemetry;
  NetworkListener networkListener;
  UnlimitedHandleResource<Handle, SourceData, Handle::kSource> sources;
  UnlimitedHandleResource<Handle, SinkData, Handle::kSink> sinks;
  wpi::EventLoopRunner eventLoop;
};

Instance::Instance() : m_impl(new Impl) { SetDefaultLogger(); }

Instance::~Instance() {}

Instance& Instance::GetInstance() {
  static Instance* inst = new Instance;
  return *inst;
}

void Instance::Shutdown() {
  m_impl->eventLoop.Stop();
  m_impl->sinks.FreeAll();
  m_impl->sources.FreeAll();
  m_impl->networkListener.Stop();
  m_impl->telemetry.Stop();
  m_impl->notifier.Stop();
}

wpi::Logger& Instance::GetLogger() { return m_impl->logger; }

Notifier& Instance::GetNotifier() { return m_impl->notifier; }

Telemetry& Instance::GetTelemetry() { return m_impl->telemetry; }

wpi::EventLoopRunner& Instance::GetEventLoop() { return m_impl->eventLoop; }

void Instance::SetDefaultLogger() { m_impl->logger.SetLogger(def_log_func); }

void Instance::StartNetworkListener(bool immediateNotify) {
  m_impl->networkListener.Start();
  if (immediateNotify) m_impl->notifier.NotifyNetworkInterfacesChanged();
}

std::pair<CS_Source, std::shared_ptr<SourceData>> Instance::FindSource(
    const SourceImpl& source) {
  return m_impl->sources.FindIf(
      [&](const SourceData& data) { return data.source.get() == &source; });
}

std::pair<CS_Sink, std::shared_ptr<SinkData>> Instance::FindSink(
    const SinkImpl& sink) {
  return m_impl->sinks.FindIf(
      [&](const SinkData& data) { return data.sink.get() == &sink; });
}

std::shared_ptr<SourceData> Instance::GetSource(CS_Source handle) {
  return m_impl->sources.Get(handle);
}

std::shared_ptr<SinkData> Instance::GetSink(CS_Sink handle) {
  return m_impl->sinks.Get(handle);
}

CS_Source Instance::CreateSource(CS_SourceKind kind,
                                 std::shared_ptr<SourceImpl> source) {
  auto handle = m_impl->sources.Allocate(kind, source);
  m_impl->notifier.NotifySource(source->GetName(), handle, CS_SOURCE_CREATED);
  source->recordTelemetry.connect(
      [this, handle](CS_TelemetryKind kind, int64_t quantity) {
        m_impl->telemetry.Record(handle, kind, quantity);
      });
  source->Start();
  return handle;
}

CS_Sink Instance::CreateSink(CS_SinkKind kind, std::shared_ptr<SinkImpl> sink) {
  auto handle = m_impl->sinks.Allocate(kind, sink);
  m_impl->notifier.NotifySink(sink->GetName(), handle, CS_SINK_CREATED);
  return handle;
}

void Instance::DestroySource(CS_Source handle) {
  if (auto data = m_impl->sources.Free(handle))
    m_impl->notifier.NotifySource(data->source->GetName(), handle,
                                  CS_SOURCE_DESTROYED);
}

void Instance::DestroySink(CS_Sink handle) {
  if (auto data = m_impl->sinks.Free(handle))
    m_impl->notifier.NotifySink(data->sink->GetName(), handle,
                                CS_SINK_DESTROYED);
}

wpi::ArrayRef<CS_Source> Instance::EnumerateSourceHandles(
    wpi::SmallVectorImpl<CS_Source>& vec) {
  return m_impl->sources.GetAll(vec);
}

wpi::ArrayRef<CS_Sink> Instance::EnumerateSinkHandles(
    wpi::SmallVectorImpl<CS_Sink>& vec) {
  return m_impl->sinks.GetAll(vec);
}

wpi::ArrayRef<CS_Sink> Instance::EnumerateSourceSinks(
    CS_Source source, wpi::SmallVectorImpl<CS_Sink>& vec) {
  vec.clear();
  m_impl->sinks.ForEach([&](CS_Sink sinkHandle, const SinkData& data) {
    if (source == data.sourceHandle.load()) vec.push_back(sinkHandle);
  });
  return vec;
}
