// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/SendableCameraWrapper.h"

#include <functional>
#include <memory>
#include <string>

#include <wpi/DenseMap.h>

#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

namespace frc {
namespace detail {
std::shared_ptr<SendableCameraWrapper>& GetSendableCameraWrapper(
    CS_Source source) {
  static wpi::DenseMap<int, std::shared_ptr<SendableCameraWrapper>> wrappers;
  return wrappers[static_cast<int>(source)];
}

void AddToSendableRegistry(frc::Sendable* sendable, std::string name) {
  SendableRegistry::Add(sendable, name);
}
}  // namespace detail

void SendableCameraWrapper::InitSendable(SendableBuilder& builder) {
  builder.AddStringProperty(
      ".ShuffleboardURI", [this] { return m_uri; }, nullptr);
}
}  // namespace frc
