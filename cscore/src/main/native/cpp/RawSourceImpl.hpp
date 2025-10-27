// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_RAWSOURCEIMPL_H_
#define CSCORE_RAWSOURCEIMPL_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "ConfigurableSourceImpl.hpp"
#include "SourceImpl.hpp"
#include "wpi/cs/cscore_raw.h"

namespace cs {

class RawSourceImpl : public ConfigurableSourceImpl {
 public:
  RawSourceImpl(std::string_view name, wpi::Logger& logger, Notifier& notifier,
                Telemetry& telemetry, const VideoMode& mode);
  ~RawSourceImpl() override;

  // Raw-specific functions
  void PutFrame(const WPI_RawFrame& image);

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace cs

#endif  // CSCORE_RAWSOURCEIMPL_H_
