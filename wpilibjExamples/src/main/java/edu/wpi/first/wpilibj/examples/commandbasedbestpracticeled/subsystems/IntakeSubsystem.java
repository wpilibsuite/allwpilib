// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class IntakeSubsystem extends SubsystemBase {
  /**
   * Indicates that intake has successfully acquired a game piece.
   */
  public final Trigger gamePieceAcquired = new Trigger(this::hasGamePieceAcquired);

  private boolean hasGamePieceAcquired() {
    return false;
  }
}
