// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace wpi {

class GenericHID;

/** Interface for device wrappers backed by a GenericHID. */
class HIDDevice {
 public:
  virtual ~HIDDevice() = default;

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  virtual GenericHID& GetGenericHID() = 0;

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  virtual const GenericHID& GetGenericHID() const = 0;
};

}  // namespace wpi
