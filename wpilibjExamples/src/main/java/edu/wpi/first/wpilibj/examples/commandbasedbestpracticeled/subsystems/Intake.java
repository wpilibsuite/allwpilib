// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Example subsystem that acquires a game piece (simulated fake boolean by pressing "B" button). A
 * signal is displayed to indicate the status of the Intake (acquired game piece or not).
 */

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.wpilibj2.command.Commands.parallel;

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

public class Intake extends SubsystemBase {
  private final LEDView m_robotSignals;

  /**
   * @param robotSignals Signal Subsystem
   */
  public Intake(LEDView robotSignals) {
    this.m_robotSignals = robotSignals;
  }

  /**
   * Signals that a game piece has been acquired
   * 
   * @return command to set the signal indicating game piece acquired
   */
  public Command gamePieceIsAcquired() {
    LEDPattern gamePieceAcquiredSignal = LEDPattern.solid(Color.kMagenta).blink(Seconds.of(0.2));
    return
      parallel(
          m_robotSignals.setSignal(gamePieceAcquiredSignal) // this command locks the
          .ignoringDisable(true)                            //  robotSignals subsystem
          .withName("MainGamePieceAcquiredSignal"),

          runOnce(() -> {}) // for an example this command locks the Intake subsystem for the group
                            // since there is no asProxy
                            // the none() command is similar but has no requirements
      ).withName("Game Piece Acquired Parallel");
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
  public void runBeforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void runAfterCommands() {}
}
