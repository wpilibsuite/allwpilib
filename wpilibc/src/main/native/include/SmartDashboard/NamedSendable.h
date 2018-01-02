/*----------------------------------------------------------------------------*/
/* Copyright (c) 2012-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <string>

#include <support/deprecated.h>

#include "SmartDashboard/Sendable.h"

namespace frc {

/**
 * The interface for sendable objects that gives the sendable a default name in
 * the Smart Dashboard.
 * @deprecated Use Sendable directly instead
 */
class WPI_DEPRECATED("use Sendable directly instead") NamedSendable
    : public Sendable {
 public:
  void SetName(const llvm::Twine& name) override;
  std::string GetSubsystem() const override;
  void SetSubsystem(const llvm::Twine& subsystem) override;
  void InitSendable(SendableBuilder& builder) override;
};

}  // namespace frc
