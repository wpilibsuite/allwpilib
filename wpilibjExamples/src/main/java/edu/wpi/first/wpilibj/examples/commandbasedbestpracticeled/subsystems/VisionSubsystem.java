// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class VisionSubsystem extends SubsystemBase {
  /**
   * Indicates that vision sees target.
   */
  public final Trigger targetAcquired = new Trigger(this::canSeeTarget);

  private boolean canSeeTarget() {
    return false;
  }
}
