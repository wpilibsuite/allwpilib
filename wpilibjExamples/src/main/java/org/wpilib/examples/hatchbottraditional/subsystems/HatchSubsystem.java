// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbottraditional.subsystems;

import static org.wpilib.hardware.pneumatic.DoubleSolenoid.Value.FORWARD;
import static org.wpilib.hardware.pneumatic.DoubleSolenoid.Value.REVERSE;

import module wpilib;
import module wpilib.command2;

import org.wpilib.examples.hatchbottraditional.Constants.HatchConstants;

/** A hatch mechanism actuated by a single {@link DoubleSolenoid}. */
public class HatchSubsystem extends SubsystemBase {
  private final DoubleSolenoid hatchSolenoid =
      new DoubleSolenoid(
          CANPort.CAN_S0,
          PneumaticsModuleType.CTRE_PCM,
          HatchConstants.HATCH_SOLENOID_PORTS[0],
          HatchConstants.HATCH_SOLENOID_PORTS[1]);

  /** Grabs the hatch. */
  public void grabHatch() {
    hatchSolenoid.set(FORWARD);
  }

  /** Releases the hatch. */
  public void releaseHatch() {
    hatchSolenoid.set(REVERSE);
  }

  @Override
  public void logTo(TelemetryTable table) {
    super.logTo(table);
    // Publish the solenoid state to telemetry.
    table.log("extended", hatchSolenoid.get() == FORWARD);
  }
}
