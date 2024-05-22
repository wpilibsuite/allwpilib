// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Example program that shows a variety of command based and programming
 * "best practices."
 * 
 * Demonstration output is on five sets of ten LEDs to show the program is
 * operating; operator input is Xbox controller. The sixth demonstration
 * output is the console "prints."
 * 
 * 1.  Target Vision Acquired subsystem output Top LEDView subsystem (runs disabled, too)
 *     default blue;
 *     autonomous mode command dark green (no requirement for Target Vision Acquired);
 *     target vision acquired orange (simulate target acquired by pressing "A" button);
 *     slowly around the color wheel (initiated by pressing "X" button) (no requirement
 *       for Target Vision Acquired)
 * 2. Game Piece Intake Acquired subsystem output Main LEDView subsystem
 *     default cyan;
 *     autonomous mode command light green (no requirement for Game Piece Intake Acquired);
 *     intake game piece acquired magenta fast blink (simulate game piece intake
 *       acquired by pressing "B" button)
 * 3. EnableDisable LEDView subsystem
 *     enabled mode green slow blink
 *     disabled mode red slow blink
 * 4. HistoryFSM subsystem HistoryDemo LEDView subsystem
 *     random colors that don't repeat for awhile (history) (initiated by pressing "Y"
 *       button then self perpetuating)
 * 5. AchieveHueGoal subsystem output AchieveHueGoal LEDView subsystem
 *     Subsystem based controller runs continuously and responds to a goal setting
 *       subsystem. Colors on color wheel position showing PID controller subsystem
 *       converging on a color selected by Xbox right trigger axis
 * 6. Ungrouped Sequential Group Demo console output initiated by teleop enable mode
 *     Show default command runs after a subsystem requirement completes in an ungrouped
 *       sequence but not a normal sequence
 * 
 * There are some variations with commands that may run disabled or not (most can).
 * All commands are interruptible.
 * Some button presses are debounced.
 * _____________________________________________________________________________________
 * 
 * Sample program to demonstrate loose coupling of Commands in a sequential grouping.
 *
 * A standard sequence locks all of the requirements of all of the subsystems used in
 * the group for the duration of the sequential execution. Default commands do not run
 * until the entire sequence completes.
 * 
 * A loosely connected sequence created by the RobotContainer.looseSequence() sequential
 * group demonstrates that each command runs independently and when a command ends its
 * subsystems default command runs.
 * 
 * Running the same sequence normally with Commands.sequence demonstrates that the
 * subsystem requirements are maintained for the entire group execution duration and
 * the default command is not activated unto the sequence group ends.
 * 
 */

/**
 * Example program demonstrating:
 * 
 * Splitting a common resource into two separately used resources (LEDs)
 * Configure button trigger
 * Triggers
 * Use of command parameters set at command creation time
 * Use of command parameters set at changable at runtime (Suppliers)
 * Use of method reference
 * Inject TimedRobot.addPeriodic() into other classes
 * Some commentary on composite commands and mode changes
 * Command logging
 * Configuring an autonomous commnad
 * Use of Xbox controller to produce fake events
 * Use of Xbox controller to trigger an event
 * Use of public command factories in subsystems
 * Use of private non-Command methods to prevent other classes from forgetting to lock a subsystem
 * Change TimeRobot loop speed
 * Change LED update rate different from the TimedRobot loop speed
 * Overloading method parameter types
 * No commands with the word Command in the name
 * No triggers with the word Trigger in the name
 * Supplier of dynamic LED pattern
 * Static LED pattern
 * Restrict Subsystem Default Command to none until set once at any time and then unchangeable
 * Goal setting subsystem for a resource
 * Triggers available for other systems to use
 * Default commands can either run or not run within a sequential group depending on how the group is defined
 * 
 * This code anticipates extensions to the WPILib addressable LED class which are included here.
 * This example program runs in real or simulated mode of the 2024 WPILib.
 * 
 * This is a refactor and extension of code donated by ChiefDelphi @illinar. It is intended
 * to demonstrate good programming based on @Oblarg's rules.
 * Any errors or confusions are the fault and responsibility of ChiefDelphi @SLAB-Mr.Thomas; github tom131313.
 * 
 */
package frc.robot;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

public class Robot extends TimedRobot {

  private RobotContainer m_robotContainer;
  private Command m_autonomousCommand;

  // Robot() {
  //   super(1.); // slow things down to human speed
  // }

  @Override
  public void robotInit() {

    m_robotContainer = new RobotContainer();
  }

  @Override
  public void robotPeriodic() {

    m_robotContainer.beforeCommands();    // get a consistent set of all inputs
    CommandScheduler.getInstance().run(); // check all the triggers and run all the scheduled commands
    m_robotContainer.afterCommands();     // write outputs such as logging, dashboards and indicators
  }

  @Override
  public void disabledInit() {} // commands running from another mode haven't been cancelled

  @Override
  public void disabledPeriodic() {}

  @Override
  public void disabledExit() {}

  @Override
  public void autonomousInit() {

    // commands running from another mode haven't been cancelled directly but may be interrupted by this command
    m_autonomousCommand = m_robotContainer.getAutonomousCommand();

    if (m_autonomousCommand != null) {
      m_autonomousCommand.schedule();
    }
  }

  @Override
  public void autonomousPeriodic() {}

  @Override
  public void autonomousExit() {

       if (m_autonomousCommand != null) {
      m_autonomousCommand.cancel();
    }
  }

  private int count;

  @Override
  public void teleopInit() { // commands running from another mode haven't been cancelled directly except the one below

    if (m_autonomousCommand != null) {
      m_autonomousCommand.cancel();
    }

    count = 0;
  }

  @Override
  public void teleopPeriodic() {
    
    // demonstrate method to run a loosely grouped sequence of commands such
    // that their requirements are not required for the entire group process
    // and the default command will run.
    count++;
    if (count == 400) m_robotContainer.testUngroupedSequence.schedule();
    if (count == 500) m_robotContainer.testSequence.schedule();
    if (count == 600) m_robotContainer.unregisterGroupedUngroupedTest();
  }

  @Override
  public void teleopExit() {}

  @Override
  public void testInit() {

    CommandScheduler.getInstance().cancelAll();
  }

  @Override
  public void testPeriodic() {}

  @Override
  public void testExit() {}
}
