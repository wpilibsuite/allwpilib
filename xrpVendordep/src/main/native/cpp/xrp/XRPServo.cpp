// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/xrp/XRPServo.h"

#include <frc/Errors.h>

using namespace frc;

std::map<int, std::string> XRPServo::s_simDeviceMap = {{4, "servo1"},
                                                       {5, "servo2"}};

std::set<int> XRPServo::s_registeredDevices = {};

void XRPServo::CheckDeviceAllocation(int deviceNum) {
  if (s_simDeviceMap.count(deviceNum) == 0) {
    throw FRC_MakeError(frc::err::ChannelIndexOutOfRange, "Channel {}",
                        deviceNum);
  }

  if (s_registeredDevices.count(deviceNum) > 0) {
    throw FRC_MakeError(frc::err::ResourceAlreadyAllocated, "Channel {}",
                        deviceNum);
  }

  s_registeredDevices.insert(deviceNum);
}

XRPServo::XRPServo(int deviceNum) {
  CheckDeviceAllocation(deviceNum);

  m_deviceName = "XRPServo:" + s_simDeviceMap[deviceNum];
  m_simDevice = hal::SimDevice(m_deviceName.c_str());

  if (m_simDevice) {
    m_simDevice.CreateBoolean("init", hal::SimDevice::kOutput, true);
    m_simPosition =
        m_simDevice.CreateDouble("position", hal::SimDevice::kOutput, 0.5);
  }
}

void XRPServo::SetAngle(double angleDegrees) {
  if (angleDegrees < 0.0) {
    angleDegrees = 0.0;
  }

  if (angleDegrees > 180.0) {
    angleDegrees = 180.0;
  }

  double pos = (angleDegrees / 180.0);

  if (m_simPosition) {
    m_simPosition.Set(pos);
  }
}

double XRPServo::GetAngle() const {
  if (m_simPosition) {
    return m_simPosition.Get() * 180.0;
  }

  return 90.0;
}

void XRPServo::SetPosition(double pos) {
  if (pos < 0.0) {
    pos = 0.0;
  }

  if (pos > 1.0) {
    pos = 1.0;
  }

  if (m_simPosition) {
    m_simPosition.Set(pos);
  }
}

double XRPServo::GetPosition() const {
  if (m_simPosition) {
    return m_simPosition.Get();
  }

  return 0.5;
}
