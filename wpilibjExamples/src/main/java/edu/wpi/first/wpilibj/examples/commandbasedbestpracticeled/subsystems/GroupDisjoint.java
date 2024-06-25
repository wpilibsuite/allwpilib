// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled.subsystems;

/**
 * General purpose class to create test commands to "disjoint" methods designed to run commands by
 * proxy to allow default commands to run within composed group commands.
 */

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;

/**
 * Create tests for the "disjoint" methods used to run commands by "proxy" to allow default commands
 * to run within composed group commands.
 *
 * <p>The Command factory "testDuration" accepts parameters for the test number and the duration the
 * test command should run. (Note that is a "history" example.)
 *
 * <p>The default command is "built-in" so changes must be recoded in the class.
 */
public class GroupDisjoint extends SubsystemBase {
  /*
   * All Commands factories are "public."
   *
   * All other methods are "private" to prevent other classes from forgetting to add requirements of
   * these resources if creating commands from these methods.
   */

  private String m_resourceID;

  /**
   * @param resourceID resource (subsystem) ID
   */
  public GroupDisjoint(String resourceID) {
    this.m_resourceID = resourceID;
  }

  /**
   * Run before commands and triggers
   */
  public void runBeforeCommands() {}

  /**
   * Run after commands and triggers
   */
  public void runAfterCommands() {}

  // Command factories cannot be "static" since they require the subsystem
  // instance ("this"; implied in this example by the runOnce() command).

  /*
   * Public Commands
   */

  /**
   * Example of how to allow a default command to be set no more than once.
   *
   * @param def default command
   */
  @Override
  public void setDefaultCommand(Command def) {
    if (getDefaultCommand() != null) {
      throw new IllegalArgumentException("Default Command already set");
    }

    if (def != null) {
      super.setDefaultCommand(def);
    }
  }

  /** Set the default default command */
  public void setDefaultCommand() {
    setDefaultCommand(m_defaultCommand);
  }

  private final Command m_defaultCommand = run(() -> System.out.print(m_resourceID + "d"));
  // note that the Commands.print("testing " + testNumber) does not require a subsystem which
  // is needed for this test so System.out.print() was used more directly.

  /**
   * Command Factory for GroupDisjointTest subsystems
   *
   * Using this syntax is slightly easier to read than using "new TestDuration()".
   *
   * @param testNumber output this number and the resource (subsystem) ID
   * @param testDuration elapsed time to run execute() to produce output
   * @return command that puts out a "testNumber" for "testDuration" time
   */
  public Command testDuration(int testNumber, Measure<Time> testDuration) {
    return new TestDuration(testNumber, testDuration);
  }

  private class TestDuration extends Command {
    private final int m_testNumber;
    private final Measure<Time> m_testDuration;
    Timer m_endTime = new Timer(); // need a memory of time; put in the narrowest possible scope

    // wouldn't have to have quite so large of command template code if time history scope was put
    // in RobotContainer but then it's hard to manage since each subsystem instance would need a
    // manually coded time history variable in RobotContainer while here it's automatic

    private TestDuration(int testNumber, Measure<Time> testDuration) {
      this.m_testNumber = testNumber;
      this.m_testDuration = testDuration;
      addRequirements(GroupDisjoint.this); // requirements are too easy to forget
    }

    @Override
    public void initialize() {
      m_endTime.restart();
    }

    @Override
    public void execute() {
      System.out.print(m_resourceID + m_testNumber);
    }

    @Override
    public boolean isFinished() {
      return m_endTime.hasElapsed(m_testDuration.in(Seconds));
    }
  }
}
