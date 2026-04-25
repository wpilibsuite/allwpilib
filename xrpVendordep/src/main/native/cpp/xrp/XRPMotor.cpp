// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/xrp/XRPMotor.hpp"

#include <map>
#include <set>
#include <string>

#include "wpi/system/Errors.hpp"

using namespace wpi::xrp;

std::map<int, std::string> XRPMotor::s_simDeviceMap = {
    {0, "motorL"}, {1, "motorR"}, {2, "motor3"}, {3, "motor4"}};

std::set<int> XRPMotor::s_registeredDevices = {};

void XRPMotor::CheckDeviceAllocation(int deviceNum) {
  if (s_simDeviceMap.count(deviceNum) == 0) {
    throw WPILIB_MakeError(wpi::err::ChannelIndexOutOfRange, "Channel {}",
                           deviceNum);
  }

  if (s_registeredDevices.count(deviceNum) > 0) {
    throw WPILIB_MakeError(wpi::err::ResourceAlreadyAllocated, "Channel {}",
                           deviceNum);
  }

  s_registeredDevices.insert(deviceNum);
}

XRPMotor::XRPMotor(int deviceNum) {
  CheckDeviceAllocation(deviceNum);

  m_deviceName = "XRPMotor:" + s_simDeviceMap[deviceNum];
  m_simDevice = hal::SimDevice(m_deviceName.c_str());

  if (m_simDevice) {
    m_simDevice.CreateBoolean("init", hal::SimDevice::Direction::OUTPUT, true);
    m_simInverted = m_simDevice.CreateBoolean(
        "inverted", hal::SimDevice::Direction::INPUT, false);
    m_simPower = m_simDevice.CreateDouble(
        "power", hal::SimDevice::Direction::OUTPUT, 0.0);
  }
}

void XRPMotor::SetPower(double power) {
  if (m_simPower) {
    bool invert = false;
    if (m_simInverted) {
      invert = m_simInverted.Get();
    }

    m_simPower.Set(invert ? -power : power);
  }
}

double XRPMotor::GetPower() const {
  if (m_simPower) {
    return m_simPower.Get();
  }

  return 0.0;
}

void XRPMotor::SetInverted(bool isInverted) {
  if (m_simInverted) {
    m_simInverted.Set(isInverted);
  }
}

bool XRPMotor::GetInverted() const {
  if (m_simInverted) {
    return m_simInverted.Get();
  }

  return false;
}

void XRPMotor::Disable() {
  SetPower(0.0);
}

void XRPMotor::StopMotor() {
  SetPower(0.0);
}

std::string XRPMotor::GetDescription() const {
  return m_deviceName;
}
