// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <atomic>
#include <string_view>

#include "ConfigurableSourceImpl.hpp"
#include "SourceImpl.hpp"

namespace wpi::cs {

class RawSourceImpl : public ConfigurableSourceImpl {
 public:
  RawSourceImpl(std::string_view name, wpi::util::Logger& logger,
                Notifier& notifier, Telemetry& telemetry,
                const VideoMode& mode);
  ~RawSourceImpl() override;

  // Raw-specific functions
  void PutFrame(const WPI_RawFrame& image);

 private:
  std::atomic_bool m_connected{true};
};

}  // namespace wpi::cs
