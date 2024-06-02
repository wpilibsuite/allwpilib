// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*
 * Example program that shows a variety of command based and programming
 * "best practices."
 * 
 * Demonstration output is on five sets of eight identical LEDs to show the program is
 * operating; operator input is Xbox controller. The sixth demonstration
 * output is the terminal console "prints."
 * 
 * 1. LED set 1 usage
 *     Target Vision Acquired subsystem output Top LEDView subsystem (runs disabled, too)
 *      default blue
 *      target vision acquired orange (simulate target acquired by pressing "A" button)
 *     Autonomous mode command dark green (no requirement for Target Vision Acquired)
 *     Slowly around the color wheel (initiated by pressing "X" button) (no requirement
 *       for Target Vision Acquired)
 * 
 * 2. LED set 2 usage
 *     Game Piece Intake Acquired subsystem output Main LEDView subsystem
 *      default cyan
 *      intake game piece acquired magenta fast blink (simulate game piece intake
 *       acquired by pressing "B" button)
 *     Autonomous mode command light green (no requirement for Game Piece Intake Acquired)
 * 
 * 3. LED set 3 usage
 *     EnableDisable LEDView subsystem
 *      enabled mode green slow blink
 *      disabled mode red slow blink
 * 
 * 4. LED set 4 usage
 *     HistoryFSM subsystem HistoryDemo LEDView subsystem
 *      random colors that don't repeat for awhile (history) (initiated by pressing "Y"
 *       button then self perpetuating) (runs in enabled mode)
 * 
 * 5. LED set 5 usage
 *     AchieveHueGoal subsystem output AchieveHueGoal LEDView subsystem
 *      Subsystem based controller runs continuously and responds to a goal setting
 *       subsystem. Colors on color wheel position showing PID controller subsystem
 *       converging on a color selected by Xbox right trigger axis (press to start)
 * 
 * 6. Console Terminal usage
 *     Disjoint Sequential Group Demo console output initiated by teleop enable mode
 *      Show subsystem default command doesn't run within a group command unless the
 *      command with the subsystem requirement is disjointed from the group by using
 *      a Proxy structure. (runs in teleop mode)
 * 
 * All commands are interruptible.
 * Some button presses are debounced.
 */

/*
 * Example program demonstrating:
 * 
 * Splitting a common resource into two separately used resources (LEDs)
 * Configure button trigger
 * Triggers
 * Use of command parameters set at command creation time
 * Use of command parameters set at changeable at runtime (Suppliers)
 * Use of method reference
 * Inject TimedRobot.addPeriodic() into other classes
 * Some commentary on composite commands and mode changes
 * Command logging
 * Configuring an autonomous command
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
 * Default commands can either run or not run within a sequential group depending on how the group is defined using Proxy
 * Commands run in sequenced by triggering successive commands
 * Use of Measure<Time>
 */

/* Default Commands can be useful but they normally do not run within grouped
 * commands even if their associated subsystem is not active at all times within
 * the group.
 * 
 *  There are several possibilities to accommodate that restriction:
 *  1. do without default commands at any time
 *  2. do without the default command only within the group
 *  3. manually code the function of the default command within a group
 *  4. break groups into smaller groups and use Triggers to sequence multiple groups
 *  5. consider using Proxy branching out of the group restriction
 * 
 *   Using Triggers to sequence successive commands may help better organize the
 *   command flow and isolate some subsystem requirements so the default command can
 *   run. That’s okay and is preferred to using proxied commands.
 * 
 *   Usage of Proxies to hide the subsystem requirements from normal checks and
 *   thus allow the default command to activate could be useful but should be used
 *   extremely sparingly by an experienced user.
 * 
 *   The possibility of unintended consequences is very high if bypassing the
 *   normal checks of requirements. Usage should be limited to when you can easily
 *   understand what exactly you’re doing by invoking them.
 * 
 *   Judicious use of asProxy() on a group’s interior commands can often allow
 *   default commands to run correctly within groups. Incorrect application of Proxy
 *   results in an extremely difficult problem to debug.
 * 
 *   Slapping an asProxy() around a composed command isn’t sufficient. You have to
 *   proxy the inner commands, also or instead, and any new library commands to ease
 *   the use of Proxy aren’t recursive to inner layers.
 *
 *   After thoroughly understanding the structure of your groups extremely carefully
 *   add asProxy() to as few interior commands as possible to accomplish what you need.
 * 
 *   Known problems:
 *
 *      Proxies break the sanity check that warn of a subsystem running in parallel
 *      commands. There is no warning - just wrong results (a warning might be added to
 *      the new combined Proxy library functions but that helps only if you use those
 *      functions correctly and nothing says you have to).
 * 
 *      repeatedly() doesn’t repeat correctly due to a bug in the WPILib. (The repeat is
 *      different than the original proxied command).
 * 
 *      andThen() doesn’t work but that can be circumvented by using sequence().
 */

/*
 *  Uses of a Subsystem's Default Command that can cause unexpected results:
 * 
 *    Default Command normally does't run in a group of commands. Not using default commands
 *    can prevent that inconsistency but then you lose the benefits of default commands.
 * 
 *    Default Command can be set more than once but only the last one set is active. It
 *    might not be obvious which Default Command is being used. Never setting more than one
 *    default command can prevent that confusion.
 * 
 *    A default Command doesn't have to be set and can be removed after setting. Again it
 *    might not be obvious what default command is active. Consider mistake-proofing
 *    strategies to prevent confusion.
 */

/*
 * This code anticipates extensions to the WPILib addressable LED class which are included here.
 * 
 * This example program runs in real or simulated mode of the 2024 WPILib.
 * 
 * This is a refactor and extension of code donated by ChiefDelphi @illinar. It is intended
 * to demonstrate good programming based on @Oblarg's rules.
 * 
 * The use of the InternalTrigger to sequence command runs was donated by ChiefDelphi @bovlb
 * 
 * Any errors or confusions are the fault and responsibility of
 *  ChiefDelphi @SLAB-Mr.Thomas; github tom131313.
 */

package frc.robot;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

public class Robot extends TimedRobot {

  private RobotContainer m_robotContainer;
  private Command m_autonomousCommand;

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

  @Override
  public void teleopInit() { // commands running from another mode haven't been cancelled directly except the one below

    if (m_autonomousCommand != null) {
      m_autonomousCommand.cancel();
    }

    GroupDisjointTest.disjointedSequenceTestJob().schedule();
  }

  @Override
  public void teleopPeriodic() {
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
