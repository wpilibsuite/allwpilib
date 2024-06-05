// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

/*
 * Example subsystem that acquires a target (simulated fake boolean by pressing "A" button). A
 * signal is displayed to indicate the status of the Target Vision (acquired target or not).
 */

import frc.robot.Color;
import frc.robot.LEDPattern;
import frc.robot.subsystems.RobotSignals.LEDView;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;

public class TargetVision extends SubsystemBase {

  private final LEDView m_RobotSignals;
  private final CommandXboxController m_OperatorController;

  public final Trigger m_TargetAcquired = new Trigger(this::canSeeTarget);

  /**
   * 
   * @param robotSignals
   * @param operatorController
   */
  public TargetVision(LEDView robotSignals,
      CommandXboxController operatorController) {

    this.m_RobotSignals = robotSignals;
    this.m_OperatorController = operatorController;
    m_TargetAcquired.whileTrue(targetIsAcquired());
  }

  /**
   * 
   * @return command to set the signal indicating target acquired
   */
  public Command targetIsAcquired() {

    LEDPattern targetAcquiredSignal = LEDPattern.solid(Color.kOrange);
    return

    m_RobotSignals.setSignal(targetAcquiredSignal) // this command locks the robotSignals.Top
                                                 // subsystem only
        .andThen(Commands.idle(this).withTimeout(0.)) // command created in this subsystem will lock
                                                      // this subsystem also
        /* composite */
        .withName("LedVisionTargetInSight").ignoringDisable(true); // ignore disable true must be
                                        // here on the composite; on the first command doesn't do anything
  }

  /**
   * 
   * @return status of fake event source for target acquired
   */
  private boolean canSeeTarget() {

    return m_OperatorController.getHID().getAButton();
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
