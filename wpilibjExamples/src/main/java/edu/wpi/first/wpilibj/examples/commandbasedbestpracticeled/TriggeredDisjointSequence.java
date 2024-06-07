// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled;

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.WrapperCommand;
import edu.wpi.first.wpilibj2.command.button.InternalButton;
import edu.wpi.first.wpilibj2.command.button.Trigger;

/**
 * A command group that runs a list of commands in sequence.
 * 
 * The end of each command triggers the start of the next command,
 * thus, each command runs relatively independently of the other commands.
 * 
 * Compares to using "proxies" but uses "triggers"
 * 
 * 
 * From ChiefDelphi poster @bovlb
 */

/*

Usage:

good example with best practice of using command factories
Command test = TriggeredDisjointSequence.sequence(
    mySubsystem.test1(),
    mySubsystem.test2(),
    mySubsystem.test3(),
    mySubsystem.test4(),
    mySubsystem.test1()); // fresh Command from its factory so it doesn't conflict with other one
test.schedule();

bad example of the pitfall of not using command factories
Command test = TriggeredDisjointSequence.sequence(
    test1,
    test2,
    test3,
    test4,
    // reusing command test1 (which is NOT a best practice) so it has to be
    // removed from the WrapperCommand group
    runOnce(()->CommandScheduler.getInstance().removeComposedCommand(test1)),
    test1);
test.schedule();

*/

/**
 * A command group that runs a list of commands in sequence.
 *
 * <p>Because each component command is individually composed, some rules for command compositions
 * apply: commands that are passed to it cannot be added to any other composition or scheduled
 * individually unless first released by
 * CommandScheduler.getInstance().removeComposedCommand(test1).
 *
 * <p>The difference with regular group compositions is this sequential group does not require at
 * all time all of the subsystems its components require.
 */
public final class TriggeredDisjointSequence extends WrapperCommand {
  private final InternalButton m_trigger;

  private TriggeredDisjointSequence(Command command) {
    super(command);
    m_trigger = new InternalButton();
  }

  @Override
  public void initialize() {
    m_trigger.setPressed(false); // reset in case this sequence is reused (maybe be sloppy use
    // of not restarting robot code and just changing modes and
    // returning to a previous mode but it's supported)
    m_command.initialize();
  }

  @Override
  public void end(boolean interrupted) {
    m_command.end(interrupted);
    m_trigger.setPressed(true); // indicate command ended and the next command is to be
    // triggered
  }

  private Trigger getTrigger() {
    return m_trigger;
  }

  /**
   * Run commands in a sequence with the end of a command triggering the next command.
   *
   * <p>Each command is added to an individual composition group (WrapperCommand) and thus is
   * restricted but the requirements of each component command are not required for the entire group
   * process since each wrapped command is run individually by being triggered from the previous
   * command.
   *
   * <p>Individual commands can be treated with .asProxy() as needed to break out of the wrapper
   * composition group.
   *
   * <p>Schedule the first command and all the rest trigger the successors.
   *
   * @param commands - list of commands to run sequentially
   * @return the first command to run by scheduling it and the remainder are automatically
   *     triggered.
   */
  public static Command sequence(Command... commands) {
    if (commands.length == 0) {
      return null;
    }

    if (commands.length == 1) {
      return commands[0];
    }

    // all but last command get the new trigger command (augmented) that triggers the next
    // command
    // all but first command triggered by the previous command
    // first doesn't have a previous and last doesn't have a next
    Command first = null;
    Trigger previousTrigger = null;
    int i = 0;

    for (Command command : commands) {
      int firstCommandIndex = 0;
      int lastCommandIndex = commands.length - 1;
      boolean atFirstcommand = i == firstCommandIndex;
      boolean atLastCommand = i == lastCommandIndex;

      TriggeredDisjointSequence augmented = null;

      if (!atLastCommand) {
        augmented = new TriggeredDisjointSequence(command); // augment it with a trigger
      }

      if (atFirstcommand) {
        first = augmented; // first command is triggered externally by the user
        // thus has no previous trigger to set
      } else if (atLastCommand) {
        previousTrigger.onTrue(command); // the last command is triggered by the previous
        // and won't be triggering the next command so no
        // augmentation
      } else {
        previousTrigger.onTrue(augmented); // not the first command and not the last command
        // the middle commands triggered by their previous command and augmented to trigger
        // the next command
      }

      if (!atLastCommand) { // now there is a previous command and it will trigger this
        // command
        previousTrigger = augmented.getTrigger();
      }

      ++i;
    }

    return first;
  }
}
