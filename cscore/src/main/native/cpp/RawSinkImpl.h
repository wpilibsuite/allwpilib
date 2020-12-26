// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_RAWSINKIMPL_H_
#define CSCORE_RAWSINKIMPL_H_

#include <stdint.h>

#include <atomic>
#include <functional>
#include <thread>

#include <wpi/Twine.h>
#include <wpi/condition_variable.h>

#include "Frame.h"
#include "SinkImpl.h"
#include "cscore_raw.h"

namespace cs {
class SourceImpl;

class RawSinkImpl : public SinkImpl {
 public:
  RawSinkImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
              Telemetry& telemetry);
  RawSinkImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
              Telemetry& telemetry,
              std::function<void(uint64_t time)> processFrame);
  ~RawSinkImpl() override;

  void Stop();

  uint64_t GrabFrame(CS_RawFrame& frame);
  uint64_t GrabFrame(CS_RawFrame& frame, double timeout);

 private:
  void ThreadMain();

  uint64_t GrabFrameImpl(CS_RawFrame& rawFrame, Frame& incomingFrame);

  std::atomic_bool m_active;  // set to false to terminate threads
  std::thread m_thread;
  std::function<void(uint64_t time)> m_processFrame;
};
}  // namespace cs

#endif  // CSCORE_RAWSINKIMPL_H_
