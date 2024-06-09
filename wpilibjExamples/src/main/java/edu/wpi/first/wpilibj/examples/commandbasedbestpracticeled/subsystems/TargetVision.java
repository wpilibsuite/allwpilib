// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/*
 * Example subsystem that acquires a target (simulated fake boolean by pressing "A" button). A
 * signal is displayed to indicate the status of the Target Vision (acquired target or not).
 */

import edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems.RobotSignals.LEDView;
import edu.wpi.first.wpilibj.LEDPattern;
import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class TargetVision extends SubsystemBase {
  private final LEDView m_robotSignals;
  private final CommandXboxController m_operatorController;

  private final Trigger m_targetAcquired = new Trigger(this::canSeeTarget);

  /**
   * @param robotSignals Signal Subsystem
   * @param operatorController Source of fake target acquired event
   */
  public TargetVision(LEDView robotSignals, CommandXboxController operatorController) {
    this.m_robotSignals = robotSignals;
    this.m_operatorController = operatorController;
    m_targetAcquired.whileTrue(targetIsAcquired()); // Trigger's "this" is null until here.
  }

  /**
   * Signals that a target has been acquired.
   * 
   * @return command to set the signal indicating target acquired
   */
  private Command targetIsAcquired() {
    LEDPattern targetAcquiredSignal = LEDPattern.solid(Color.kOrange);
    return m_robotSignals
        .setSignal(targetAcquiredSignal) // this command locks the specific injected m_robotSignals
        // subsystem only
        .andThen(Commands.idle(this).withTimeout(0.)) // command created in this subsystem will
        // lock this subsystem also
        /* composite */
        .withName("LedVisionTargetInSight")
        .ignoringDisable(true); // ignore disable true must be here on the composite; on the first
    // command doesn't do anything
  }

  /**
   * Determine if a target has been acquired.
   * 
   * @return status of fake event source for target acquired
   */
  private boolean canSeeTarget() {
    return m_operatorController.getHID().getAButton();
  }

  /**
   * Example of how to disallow default command
   *
   * @param def default command
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
