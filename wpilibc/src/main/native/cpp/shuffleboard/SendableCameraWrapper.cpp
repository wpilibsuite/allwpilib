/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/shuffleboard/SendableCameraWrapper.h"

#include <cscore_oo.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <ntcore.h>
#include <wpi/ArrayRef.h>
#include <wpi/DenseMap.h>
#include <wpi/StringMap.h>

#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

namespace {
constexpr const char* kProtocol = "camera_server://";
wpi::StringMap<std::unique_ptr<SendableCameraWrapper>> wrappers;
}  // namespace

SendableCameraWrapper& SendableCameraWrapper::Wrap(
    const cs::VideoSource& source) {
  auto name = source.GetName();
  auto& wrapper = wrappers[name];
  if (!wrapper)
    wrapper = std::make_unique<SendableCameraWrapper>(name, private_init{});
  return *wrapper;
}

SendableCameraWrapper& SendableCameraWrapper::Wrap(CS_Source source) {
  CS_Status status = 0;
  auto name = cs::GetSourceName(source, &status);
  auto& wrapper = wrappers[name];
  if (!wrapper)
    wrapper = std::make_unique<SendableCameraWrapper>(name, private_init{});
  return *wrapper;
}

SendableCameraWrapper& SendableCameraWrapper::Wrap(
    std::string cameraName, wpi::ArrayRef<std::string> cameraUrls) {
  auto& wrapper = wrappers[cameraName];
  if (!wrapper) {
    wrapper =
        std::make_unique<SendableCameraWrapper>(cameraName, private_init{});
    std::string streams = "/CameraPublisher/" + cameraName + "/streams";
    nt::NetworkTableInstance::GetDefault().GetEntry(streams).SetStringArray(
        cameraUrls);
  }
  return *wrapper;
}

SendableCameraWrapper::SendableCameraWrapper(std::string name,
                                             const private_init&)
    : SendableBase(false), m_uri(kProtocol) {
  SetName(name);
  m_uri += name;
}

void SendableCameraWrapper::InitSendable(SendableBuilder& builder) {
  builder.AddStringProperty(".ShuffleboardURI", [this] { return m_uri; },
                            nullptr);
}
