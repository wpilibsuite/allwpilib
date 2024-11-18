// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.wpilibj.examples.hatchbottraditional.subsystems;

import static org.wpilib.wpilibj.DoubleSolenoid.Value.kForward;
import static org.wpilib.wpilibj.DoubleSolenoid.Value.kReverse;

import org.wpilib.util.sendable.SendableBuilder;
import org.wpilib.wpilibj.DoubleSolenoid;
import org.wpilib.wpilibj.PneumaticsModuleType;
import org.wpilib.wpilibj.examples.hatchbottraditional.Constants.HatchConstants;
import org.wpilib.wpilibj2.command.SubsystemBase;

/** A hatch mechanism actuated by a single {@link DoubleSolenoid}. */
public class HatchSubsystem extends SubsystemBase {
  private final DoubleSolenoid m_hatchSolenoid =
      new DoubleSolenoid(
          PneumaticsModuleType.CTREPCM,
          HatchConstants.kHatchSolenoidPorts[0],
          HatchConstants.kHatchSolenoidPorts[1]);

  /** Grabs the hatch. */
  public void grabHatch() {
    m_hatchSolenoid.set(kForward);
  }

  /** Releases the hatch. */
  public void releaseHatch() {
    m_hatchSolenoid.set(kReverse);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    super.initSendable(builder);
    // Publish the solenoid state to telemetry.
    builder.addBooleanProperty("extended", () -> m_hatchSolenoid.get() == kForward, null);
  }
}
