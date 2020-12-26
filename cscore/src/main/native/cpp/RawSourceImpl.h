// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_RAWSOURCEIMPL_H_
#define CSCORE_RAWSOURCEIMPL_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <wpi/ArrayRef.h>
#include <wpi/Twine.h>

#include "ConfigurableSourceImpl.h"
#include "SourceImpl.h"
#include "cscore_raw.h"

namespace cs {

class RawSourceImpl : public ConfigurableSourceImpl {
 public:
  RawSourceImpl(const wpi::Twine& name, wpi::Logger& logger, Notifier& notifier,
                Telemetry& telemetry, const VideoMode& mode);
  ~RawSourceImpl() override;

  // Raw-specific functions
  void PutFrame(const CS_RawFrame& image);

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace cs

#endif  // CSCORE_RAWSOURCEIMPL_H_
