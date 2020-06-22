/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_INSTANCE_H_
#define CSCORE_INSTANCE_H_

#include <atomic>
#include <memory>
#include <utility>

#include "cscore_cpp.h"

namespace wpi {
class EventLoopRunner;
class Logger;
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace cs {

class FramePool;
class Notifier;
class Telemetry;
class SinkImpl;
class SourceImpl;

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

  wpi::Logger& GetLogger();
  Notifier& GetNotifier();
  Telemetry& GetTelemetry();
  wpi::EventLoopRunner& GetEventLoop();
  FramePool& GetFramePool();

  std::pair<CS_Sink, std::shared_ptr<SinkData>> FindSink(const SinkImpl& sink);
  std::pair<CS_Source, std::shared_ptr<SourceData>> FindSource(
      const SourceImpl& source);

  void SetDefaultLogger();

  void StartNetworkListener(bool immediateNotify);

  std::shared_ptr<SourceData> GetSource(CS_Source handle);
  std::shared_ptr<SinkData> GetSink(CS_Sink handle);

  CS_Source CreateSource(CS_SourceKind kind,
                         std::shared_ptr<SourceImpl> source);

  CS_Sink CreateSink(CS_SinkKind kind, std::shared_ptr<SinkImpl> sink);

  void DestroySource(CS_Source handle);
  void DestroySink(CS_Sink handle);

  wpi::ArrayRef<CS_Source> EnumerateSourceHandles(
      wpi::SmallVectorImpl<CS_Source>& vec);

  wpi::ArrayRef<CS_Sink> EnumerateSinkHandles(
      wpi::SmallVectorImpl<CS_Sink>& vec);

  wpi::ArrayRef<CS_Sink> EnumerateSourceSinks(
      CS_Source source, wpi::SmallVectorImpl<CS_Sink>& vec);

 private:
  Instance();

  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace cs

#endif  // CSCORE_INSTANCE_H_
