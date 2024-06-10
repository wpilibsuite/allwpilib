// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Example subsystem that acquires a game piece (simulated fake boolean by pressing "B" button). A
 * signal is displayed to indicate the status of the Intake (acquired game piece or not).
 */

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class Intake extends SubsystemBase {
  private final LEDView m_robotSignals;
  private final CommandXboxController m_operatorController;

  private final Trigger m_gamePieceAcquired = new Trigger(this::hasGamePieceAcquired);

  /**
   * @param robotSignals Signal Subsystem
   * @param operatorController Source of fake target of game piece acquired event
   */
  public Intake(LEDView robotSignals, CommandXboxController operatorController) {
    this.m_robotSignals = robotSignals;
    this.m_operatorController = operatorController;
    m_gamePieceAcquired.whileTrue(gamePieceIsAcquired()); // Trigger's "this" is null until here.
  }

  /**
   * Signals that a game piece has been acquired
   * 
   * @return command to set the signal indicating game piece acquired
   */
  private Command gamePieceIsAcquired() {
    LEDPattern gamePieceAcquiredSignal = LEDPattern.solid(Color.kMagenta).blink(Seconds.of(0.2));
    return m_robotSignals
        .setSignal(gamePieceAcquiredSignal) // this command locks the robotSignals.Main subsystem
        .ignoringDisable(true)
        .withName("MainGamePieceAcquiredSignal");
  }

  /**
   * Determine if a game piece has been acquired.
   * 
   * @return status of fake event source for game piece acquired
   */
  private boolean hasGamePieceAcquired() {
    return m_operatorController.getHID().getBButton();
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
