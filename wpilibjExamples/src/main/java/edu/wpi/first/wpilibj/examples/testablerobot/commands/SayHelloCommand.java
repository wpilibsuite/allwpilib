/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.commands;

import java.util.concurrent.TimeUnit;

import edu.wpi.first.wpilibj.command.Command;

import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.HelloWorldSubsystem;

/**
 * An example command to flash an LED.
 */
public class SayHelloCommand extends Command {

  private final HelloWorldSubsystem m_helloWorldSubsystem;
  private long m_lastTime;

  /**
   * Our constructor takes in all the external dependencies, meaning subsystems,
   * that this class uses to do its work. This makes the class testable.
   * 
   * @param helloWorldSubsystem   The subsystem that the command will manipulate.
   */
  public SayHelloCommand(HelloWorldSubsystem helloWorldSubsystem) {
    requires(helloWorldSubsystem);
    this.m_helloWorldSubsystem = helloWorldSubsystem;
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    // Reset our counter when starting up
    m_lastTime = 0;
  }

  // Called repeatedly when this Command is scheduled to run
  @Override
  protected void execute() {
    // Flash the LED by setting up a timing check that looks at the time elapsed
    // from the last time we flipped the LED state.
    // You could (should) use PWM, but that would testing illustration less interesting.
    // And by the way, don't use Thread.sleep() in the these command execute methods!
    long now = System.nanoTime();
    if (now > (m_lastTime + TimeUnit.MILLISECONDS.toNanos(200))) {
      m_helloWorldSubsystem.toggleLED();
      m_lastTime = now;
    }
  }

  // Make this return true when this Command no longer needs to run execute()
  @Override
  protected boolean isFinished() {
    // In this example, the command itself knows not whether it should ever end, so it doesn't.
    return false;
  }

  // Called once after isFinished returns true
  @Override
  protected void end() {
    // If end is ever called, make sure we leave the LED in a known state.
    m_helloWorldSubsystem.turnOffLED();
  }

  // Called when another command which requires one or more of the same
  // subsystems is scheduled to run
  @Override
  protected void interrupted() {
    // This is the usual ending place for this command since the scheduler will
    // wind up canceling this command.
    end();
  }
}
