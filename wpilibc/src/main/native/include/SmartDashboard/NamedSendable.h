/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include "SmartDashboard/Sendable.h"

namespace frc {

/**
 * The interface for sendable objects that gives the sendable a default name in
 * the Smart Dashboard
 *
 */
class NamedSendable : public Sendable {
 public:
  /**
   * @return the name of the subtable of SmartDashboard that the Sendable object
   *         will use
   */
  virtual std::string GetName() const = 0;
};

}  // namespace frc
