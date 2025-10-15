// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/commands2/CommandPtr.hpp>
#include <wpi/commands2/SubsystemBase.hpp>
#include <wpi/hardware/pneumatic/DoubleSolenoid.hpp>
#include <wpi/hardware/pneumatic/PneumaticsControlModule.hpp>

#include "Constants.hpp"

class HatchSubsystem : public wpi::cmd::SubsystemBase {
 public:
  HatchSubsystem();

  // Subsystem methods go here.

  /**
   * Grabs the hatch.
   */
  wpi::cmd::CommandPtr GrabHatchCommand();

  /**
   * Releases the hatch.
   */
  wpi::cmd::CommandPtr ReleaseHatchCommand();

  void InitSendable(wpi::util::SendableBuilder& builder) override;

 private:
  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.
  wpi::DoubleSolenoid m_hatchSolenoid;
};
