/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_RAWSINKIMPL_H_
#define CSCORE_RAWSINKIMPL_H_

#include <stdint.h>

#include <wpi/Twine.h>

#include "Frame.h"
#include "SinkImpl.h"
#include "cscore_raw.h"

namespace cs {
class SourceImpl;

class RawSinkImpl : public SinkImpl {
 public:
  RawSinkImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier);
  RawSinkImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
              std::function<void(uint64_t time)> processFrame);
  ~RawSinkImpl() override;

  void Stop();

  uint64_t GrabFrame(CS_RawFrame& frame);
  uint64_t GrabFrame(CS_RawFrame& frame, double timeout);

 private:
  uint64_t GrabFrameImpl(CS_RawFrame& rawFrame, Frame& incomingFrame);
};
}  // namespace cs

#endif  // CSCORE_RAWSINKIMPL_H_
