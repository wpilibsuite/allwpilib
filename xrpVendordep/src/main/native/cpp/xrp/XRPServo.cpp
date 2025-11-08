// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/xrp/XRPServo.hpp"

#include <map>
#include <numbers>
#include <set>
#include <string>

#include "wpi/system/Errors.hpp"
#include "wpi/units/angle.hpp"

using namespace wpi::xrp;

std::map<int, std::string> XRPServo::s_simDeviceMap = {
    {4, "servo1"}, {5, "servo2"}, {6, "servo3"}, {7, "servo4"}};

std::set<int> XRPServo::s_registeredDevices = {};

void XRPServo::CheckDeviceAllocation(int deviceNum) {
  if (s_simDeviceMap.count(deviceNum) == 0) {
    throw FRC_MakeError(wpi::err::ChannelIndexOutOfRange, "Channel {}",
                        deviceNum);
  }

  if (s_registeredDevices.count(deviceNum) > 0) {
    throw FRC_MakeError(wpi::err::ResourceAlreadyAllocated, "Channel {}",
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

void XRPServo::SetAngle(wpi::units::radian_t angle) {
  angle = std::clamp<wpi::units::radian_t>(angle, 0_deg, 180_deg);
  double pos = angle.value() / std::numbers::pi;

  if (m_simPosition) {
    m_simPosition.Set(pos);
  }
}

wpi::units::radian_t XRPServo::GetAngle() const {
  if (m_simPosition) {
    return wpi::units::radian_t{m_simPosition.Get() * std::numbers::pi};
  }

  return 90_deg;
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
