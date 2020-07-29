/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
  SendableRegistry::GetInstance().Add(sendable, name);
}
}  // namespace detail

void SendableCameraWrapper::InitSendable(SendableBuilder& builder) {
  builder.AddStringProperty(
      ".ShuffleboardURI", [this] { return m_uri; }, nullptr);
}
}  // namespace frc
