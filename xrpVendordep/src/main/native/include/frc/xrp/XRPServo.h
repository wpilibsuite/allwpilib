// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <set>
#include <string>

#include <hal/SimDevice.h>

namespace frc {

class XRPServo {
 public:
  explicit XRPServo(int deviceNum);

  void SetAngle(double angleDegrees);
  double GetAngle() const;

  void SetPosition(double position);
  double GetPosition() const;

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;

  std::string m_deviceName;

  static std::map<int, std::string> s_simDeviceMap;
  static std::set<int> s_registeredDevices;

  static void CheckDeviceAllocation(int deviceNum);
};

}  // namespace frc
