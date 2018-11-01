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

  UnlimitedHandleResource<Handle, SourceData, Handle::kSource> sources;
  UnlimitedHandleResource<Handle, SinkData, Handle::kSink> sinks;

  wpi::Logger logger;
  Notifier notifier;
  Telemetry telemetry;
  NetworkListener network_listener;
  wpi::EventLoopRunner event_loop;

  std::pair<CS_Sink, std::shared_ptr<SinkData>> FindSink(const SinkImpl& sink);
  std::pair<CS_Source, std::shared_ptr<SourceData>> FindSource(
      const SourceImpl& source);

  void SetDefaultLogger();

 private:
  Instance();
};

}  // namespace cs

#endif  // CSCORE_INSTANCE_H_
