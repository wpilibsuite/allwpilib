// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "wpi/util/Logger.hpp"

namespace wpi::cs {

class Notifier;

class UsbCameraListener {
 public:
  UsbCameraListener(wpi::util::Logger& logger, Notifier& notifier);
  ~UsbCameraListener();

  void Start();
  void Stop();

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace wpi::cs
