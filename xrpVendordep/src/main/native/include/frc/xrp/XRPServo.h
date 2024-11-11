// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <map>
#include <set>
#include <string>

#include <hal/SimDevice.h>
#include <units/angle.h>

namespace frc {

/**
 * @ingroup xrp_api
 * @{
 */

/**
 * XRPServo.
 *
 * <p>A SimDevice based servo
 */
class XRPServo {
 public:
  /**
   * Constructs an XRPServo.
   *
   * @param deviceNum the servo channel
   */
  explicit XRPServo(int deviceNum);

  /**
   * Set the servo angle.
   *
   * @param angle Desired angle in radians
   */
  void SetAngle(units::radian_t angle);

  /**
   * Get the servo angle.
   *
   * @return Current servo angle in radians
   */
  units::radian_t GetAngle() const;

  /**
   * Set the servo position.
   *
   * @param position Desired position (Between 0.0 and 1.0)
   * @deprecated Use SetAngle() instead
   */
  [[deprecated("Use SetAngle() instead")]]
  void SetPosition(double position);

  /**
   * Get the servo position.
   *
   * @return Current servo position
   * @deprecated Use GetAngle() instead
   */
  [[deprecated("Use GetAngle() instead")]]
  double GetPosition() const;

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simPosition;

  std::string m_deviceName;

  static std::map<int, std::string> s_simDeviceMap;
  static std::set<int> s_registeredDevices;

  static void CheckDeviceAllocation(int deviceNum);
};

/** @} */

}  // namespace frc
