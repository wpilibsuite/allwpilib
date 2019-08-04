/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_INSTANCE_H_
#define CSCORE_INSTANCE_H_

#include <memory>
#include <utility>

#include <wpi/EventLoopRunner.h>
#include <wpi/Logger.h>

#include "Log.h"
#include "NetworkListener.h"
#include "Notifier.h"
#include "SinkImpl.h"
#include "SourceImpl.h"
#include "Telemetry.h"
#include "UnlimitedHandleResource.h"

namespace cs {

struct SourceData {
  SourceData(CS_SourceKind kind_, std::shared_ptr<SourceImpl> source_)
      : kind{kind_}, refCount{0}, source{source_} {}

  CS_SourceKind kind;
  std::atomic_int refCount;
  std::shared_ptr<SourceImpl> source;
};

struct SinkData {
  explicit SinkData(CS_SinkKind kind_, std::shared_ptr<SinkImpl> sink_)
      : kind{kind_}, refCount{0}, sourceHandle{0}, sink{sink_} {}

  CS_SinkKind kind;
  std::atomic_int refCount;
  std::atomic<CS_Source> sourceHandle;
  std::shared_ptr<SinkImpl> sink;
};

class Instance {
 public:
  Instance(const Instance&) = delete;
  Instance& operator=(const Instance&) = delete;
  ~Instance();

  static Instance& GetInstance();

  void Shutdown();

  wpi::Logger logger;
  Notifier notifier;
  Telemetry telemetry;
  NetworkListener networkListener;

 private:
  UnlimitedHandleResource<Handle, SourceData, Handle::kSource> m_sources;
  UnlimitedHandleResource<Handle, SinkData, Handle::kSink> m_sinks;

 public:
  wpi::EventLoopRunner eventLoop;

  std::pair<CS_Sink, std::shared_ptr<SinkData>> FindSink(const SinkImpl& sink);
  std::pair<CS_Source, std::shared_ptr<SourceData>> FindSource(
      const SourceImpl& source);

  void SetDefaultLogger();

  std::shared_ptr<SourceData> GetSource(CS_Source handle) {
    return m_sources.Get(handle);
  }

  std::shared_ptr<SinkData> GetSink(CS_Sink handle) {
    return m_sinks.Get(handle);
  }

  CS_Source CreateSource(CS_SourceKind kind,
                         std::shared_ptr<SourceImpl> source);

  CS_Sink CreateSink(CS_SinkKind kind, std::shared_ptr<SinkImpl> sink);

  void DestroySource(CS_Source handle);
  void DestroySink(CS_Sink handle);

  wpi::ArrayRef<CS_Source> EnumerateSourceHandles(
      wpi::SmallVectorImpl<CS_Source>& vec) {
    return m_sources.GetAll(vec);
  }

  wpi::ArrayRef<CS_Sink> EnumerateSinkHandles(
      wpi::SmallVectorImpl<CS_Sink>& vec) {
    return m_sinks.GetAll(vec);
  }

  wpi::ArrayRef<CS_Sink> EnumerateSourceSinks(
      CS_Source source, wpi::SmallVectorImpl<CS_Sink>& vec) {
    vec.clear();
    m_sinks.ForEach([&](CS_Sink sinkHandle, const SinkData& data) {
      if (source == data.sourceHandle.load()) vec.push_back(sinkHandle);
    });
    return vec;
  }

 private:
  Instance();
};

}  // namespace cs

#endif  // CSCORE_INSTANCE_H_
