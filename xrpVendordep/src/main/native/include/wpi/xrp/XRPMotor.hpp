// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <set>
#include <string>

#include "wpi/hal/SimDevice.h"
#include "wpi/hardware/motor/MotorController.hpp"
#include "wpi/hardware/motor/MotorSafety.hpp"
#include "wpi/util/deprecated.hpp"

namespace wpi::xrp {

WPI_IGNORE_DEPRECATED

/**
 * @defgroup xrp_api XRP Hardware API
 * @{
 */

/**
 * XRPMotor.
 *
 * <p>A SimDevice based motor controller representing the motors on an XRP robot
 */
class XRPMotor : public wpi::MotorController, public wpi::MotorSafety {
 public:
  /**
   * Constructs an XRPMotor.
   *
   * @param deviceNum the motor channel
   */
  explicit XRPMotor(int deviceNum);

  void SetDutyCycle(double value) override;
  double GetDutyCycle() const override;

  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;

  void Disable() override;

  void StopMotor() override;
  std::string GetDescription() const override;

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simVelocity;
  hal::SimBoolean m_simInverted;

  std::string m_deviceName;

  static std::map<int, std::string> s_simDeviceMap;
  static std::set<int> s_registeredDevices;

  static void CheckDeviceAllocation(int deviceNum);
};

/** @} */

WPI_UNIGNORE_DEPRECATED

}  // namespace wpi::xrp
