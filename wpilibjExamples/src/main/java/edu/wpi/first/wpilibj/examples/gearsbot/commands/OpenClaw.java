/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj2.command.WaitCommand;

import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;

/**
 * Opens the claw for one second. Real robots should use sensors, stalling motors is BAD!
 */
public class OpenClaw extends WaitCommand {
  private final Claw m_claw;

  /**
   * Creates a new OpenClaw command.
   *
   * @param claw The claw to use
   */
  public OpenClaw(Claw claw) {
    super(1);
    m_claw = claw;
    addRequirements(m_claw);
  }

  // Called just before this Command runs the first time
  @Override
  public void initialize() {
    m_claw.open();
    super.initialize();
  }

  // Called once after isFinished returns true
  @Override
  public void end(boolean interrupted) {
    m_claw.stop();
  }
}
