// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotcmdv3.subsystems;

import static org.wpilib.hardware.pneumatic.DoubleSolenoid.Value.FORWARD;
import static org.wpilib.hardware.pneumatic.DoubleSolenoid.Value.REVERSE;

import org.wpilib.command3.Command;
import org.wpilib.command3.Mechanism;
import org.wpilib.examples.hatchbotcmdv3.Constants.HatchConstants;
import org.wpilib.hardware.pneumatic.DoubleSolenoid;
import org.wpilib.hardware.pneumatic.PneumaticsModuleType;

/** A hatch mechanism actuated by a single {@link org.wpilib.hardware.pneumatic.DoubleSolenoid}. */
public class HatchSubsystem extends Mechanism {
  private final DoubleSolenoid hatchSolenoid =
      new DoubleSolenoid(
          0,
          PneumaticsModuleType.CTRE_PCM,
          HatchConstants.kHatchSolenoidPorts[0],
          HatchConstants.kHatchSolenoidPorts[1]);

  /** Grabs the hatch. */
  public Command grabHatchCommand() {
    // implicitly require `this`
    return this.run(coro -> hatchSolenoid.set(FORWARD)).named("Grab Hatch");
  }

  /** Releases the hatch. */
  public Command releaseHatchCommand() {
    // implicitly require `this`
    return this.run(coro -> hatchSolenoid.set(REVERSE)).named("Release Hatch");
  }

  //  @Override
  //  public void initSendable(SendableBuilder builder) {
  //    super.initSendable(builder);
  //    // Publish the solenoid state to telemetry.
  //    builder.addBooleanProperty("extended", () -> hatchSolenoid.get() == kForward, null);
  //  }
}
