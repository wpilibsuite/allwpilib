// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

/*
 * Example subsystem that acquires a game piece (simulated fake boolean by pressing "B" button). A
 * signal is displayed to indicate the status of the Intake (acquired game piece or not).
 */

import static edu.wpi.first.units.Units.Seconds;

import frc.robot.Color;
import frc.robot.LEDPattern;
import frc.robot.subsystems.RobotSignals.LEDView;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class Intake extends SubsystemBase {

  private final LEDView robotSignals;
  private final CommandXboxController operatorController;

  public final Trigger gamePieceAcquired =
      new Trigger(this::hasGamePieceAcquired);

  public Intake(LEDView robotSignals,
      CommandXboxController operatorController) {

    this.robotSignals = robotSignals;
    this.operatorController = operatorController;
    gamePieceAcquired.whileTrue(gamePieceIsAcquired());
  }

  /**
   * 
   * @return command to set the signal indicating game piece acquired
   */
  public Command gamePieceIsAcquired() {

    LEDPattern gamePieceAcquiredSignal =
        LEDPattern.solid(Color.kMagenta).blink(Seconds.of(0.2));
    return robotSignals.setSignal(gamePieceAcquiredSignal) // this command locks the
                                                           // robotSignals.Main subsystem only
        .ignoringDisable(true)
        .withName("MainGamePieceAcquiredSignal");
  }

  /**
   * 
   * @return status of fake event source for game piece acquired
   */
  private boolean hasGamePieceAcquired() {

    return operatorController.getHID().getBButton();
  }

  /**
   * Example of how to disallow default command
   * 
   * @param def default command (not used)
   */
  @Override
  public void setDefaultCommand(Command def) {

    throw new IllegalArgumentException("Default Command not allowed");
  }

  /**
   * Run before commands and triggers
   */
  public void beforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void afterCommands() {}
}
