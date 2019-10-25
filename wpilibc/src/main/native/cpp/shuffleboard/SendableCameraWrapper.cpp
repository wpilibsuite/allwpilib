/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <functional>
#include <memory>
#include <string>

#include <wpi/DenseMap.h>

#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

namespace frc {
class SendableCameraWrapper;
namespace detail {
std::shared_ptr<SendableCameraWrapper>& GetSendableCameraWrapper(int source) {
  static wpi::DenseMap<int, std::shared_ptr<SendableCameraWrapper>> wrappers;
  return wrappers[source];
}

void CreateSendableCameraWrapperSendable(std::function<std::string()> urlGetter,
                                         SendableBuilder& builder) {
  builder.AddStringProperty(".ShuffleboardURI", std::move(urlGetter), nullptr);
}

void AddToSendableRegistry(frc::Sendable* sendable, std::string name) {
  SendableRegistry::GetInstance().Add(sendable, name);
}
}  // namespace detail
}  // namespace frc
