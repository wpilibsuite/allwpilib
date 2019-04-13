/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/SendableCameraWrapper.h"

#include <cscore_oo.h>
#include <wpi/DenseMap.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

namespace {
constexpr const char* kProtocol = "camera_server://";
wpi::DenseMap<int, std::unique_ptr<SendableCameraWrapper>> wrappers;
}  // namespace

SendableCameraWrapper& SendableCameraWrapper::Wrap(
    const cs::VideoSource& source) {
  return Wrap(source.GetHandle());
}

SendableCameraWrapper& SendableCameraWrapper::Wrap(CS_Source source) {
  auto& wrapper = wrappers[static_cast<int>(source)];
  if (!wrapper)
    wrapper = std::make_unique<SendableCameraWrapper>(source, private_init{});
  return *wrapper;
}

SendableCameraWrapper::SendableCameraWrapper(CS_Source source,
                                             const private_init&)
    : SendableBase(false), m_uri(kProtocol) {
  CS_Status status = 0;
  auto name = cs::GetSourceName(source, &status);
  SetName(name);
  m_uri += name;
}

void SendableCameraWrapper::InitSendable(SendableBuilder& builder) {
  builder.AddStringProperty(".ShuffleboardURI", [this] { return m_uri; },
                            nullptr);
}
