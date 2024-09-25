// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/xrp/XRPMotor.h"

#include <frc/Errors.h>

#include <map>
#include <set>
#include <string>

using namespace frc;

std::map<int, std::string> XRPMotor::s_simDeviceMap = {
    {0, "motorL"}, {1, "motorR"}, {2, "motor3"}, {3, "motor4"}};

std::set<int> XRPMotor::s_registeredDevices = {};

void XRPMotor::CheckDeviceAllocation(int deviceNum) {
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

WPI_IGNORE_DEPRECATED

XRPMotor::XRPMotor(int deviceNum) {
  CheckDeviceAllocation(deviceNum);

  m_deviceName = "XRPMotor:" + s_simDeviceMap[deviceNum];
  m_simDevice = hal::SimDevice(m_deviceName.c_str());

  if (m_simDevice) {
    m_simDevice.CreateBoolean("init", hal::SimDevice::kOutput, true);
    m_simInverted =
        m_simDevice.CreateBoolean("inverted", hal::SimDevice::kInput, false);
    m_simSpeed =
        m_simDevice.CreateDouble("speed", hal::SimDevice::kOutput, 0.0);
  }
}

WPI_UNIGNORE_DEPRECATED

void XRPMotor::Set(double speed) {
  if (m_simSpeed) {
    bool invert = false;
    if (m_simInverted) {
      invert = m_simInverted.Get();
    }

    m_simSpeed.Set(invert ? -speed : speed);
  }
}

double XRPMotor::Get() const {
  if (m_simSpeed) {
    return m_simSpeed.Get();
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
  Set(0.0);
}

void XRPMotor::StopMotor() {
  Set(0.0);
}

std::string XRPMotor::GetDescription() const {
  return m_deviceName;
}
