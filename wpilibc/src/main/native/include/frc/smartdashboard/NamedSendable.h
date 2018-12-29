/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <wpi/deprecated.h>

#include "frc/smartdashboard/Sendable.h"

namespace frc {

/**
 * The interface for sendable objects that gives the sendable a default name in
 * the Smart Dashboard.
 * @deprecated Use Sendable directly instead
 */
class NamedSendable : public Sendable {
 public:
  WPI_DEPRECATED("use Sendable directly instead")
  NamedSendable() = default;

  void SetName(const wpi::Twine& name) override;
  std::string GetSubsystem() const override;
  void SetSubsystem(const wpi::Twine& subsystem) override;
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
