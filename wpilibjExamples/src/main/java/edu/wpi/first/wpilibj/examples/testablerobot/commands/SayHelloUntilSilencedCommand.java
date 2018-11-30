/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.testablerobot.commands;

import edu.wpi.first.wpilibj.examples.testablerobot.subsystems.SilenceableHelloWorldSubsystem;

/**
 * An example command to flash an LED until sensing tells us otherwise.
 */
public class SayHelloUntilSilencedCommand extends SayHelloCommand {
  // Our parent has a reference to this subsystem already. You may ask yourself
  // "why is this guy doing this again?...just make the parent's version protected".
  // The answer is there is really no good reason for state to cross boundaries, in this case.
  // You have to come up with really good reasons to leak state and this is not
  // one of them. Leaky state opens up more opportunities for bugs.
  // Just hold on to the reference here again and use it.
  private final SilenceableHelloWorldSubsystem m_silenceableHelloWorldSubsystem;

  /**
   * Constructor to take in our required subsystem as a dependency, and must
   * exist because we are extending SayHelloCommand. Descendent must have a constructor.
   * 
   * @param silenceableHelloWorldSubsystem   Our subsystem to act upon
   */
  public SayHelloUntilSilencedCommand(
      SilenceableHelloWorldSubsystem silenceableHelloWorldSubsystem) {
    // Must call super class constructor to set instance variables properly
    super(silenceableHelloWorldSubsystem);
    // Hold on to our subsystem for futue use
    this.m_silenceableHelloWorldSubsystem = silenceableHelloWorldSubsystem;
  }

  /**
   * Determine whether or not our time for saying hello has come to an end.
   */
  @Override
  protected boolean isFinished() {
    // Completely override this method because we don't care what the parent is doing in this case.
    // This new behavior is what we are replacing.
    return m_silenceableHelloWorldSubsystem.beSilent();
  }
}
