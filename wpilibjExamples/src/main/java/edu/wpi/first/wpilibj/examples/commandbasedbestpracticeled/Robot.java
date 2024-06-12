// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*
 * Example program that shows a variety of command based and programming "best practices."
 *
 * Includes five different techniques useful in Command-Based programming. In addition all of the
 * examples are written in a similar suggested style of handling commands and triggers with
 * suggested variable naming style and minimal scope.
 *  1. Goal-Oriented subsystem to feed setpoints to a free-running control calculation. (PID
 *     example)
 *  2. Use of historical data in addition to current state and events as input to a Finite State
 *     Machine. (Random, non-repeating colors)
 *  3. Example of splitting an apparent single resource into pieces for independent use.
 *     (Addressable LED strip)
 *  4. Minimal example of a robot subsystem. (Command triggered by an event)
 *  5. Use of "proxies" to disjoint composed groups such that a subsystem default command may run
 *     within a group instead of the normal behavior of being blocked until the group completes.
 *     (Simple subsystems running commands and default commands concurrently in groups)
 *
 * Because all but one demonstration uses an addressable LED strip as output (one has console
 * output) there is significant overlap and depth in demonstrating style of using the 2025 advanced
 * addressable LED classes and methods.
 * 
 * 
 * Demonstration output is on five sets of eight identical LEDs to show the program is operating;
 * operator input is Xbox controller. The sixth demonstration output is the terminal console
 * "prints."
 *
 * 1. LED set 1 usage Top LEDView subsystem default blue.
 *  Autonomous mode command brown fast blink.
 *  Non-autonomous display colors slowly around the color wheel initiated by pressing "X" button.
 *
 * 2. LED set 2 usage Main LEDView subsystem default cyan.
 *  Game Piece Intake Acquired subsystem signal intake game piece acquired magenta fast blink
 *  (simulate game piece intake acquired by pressing "B" button).
 *  Autonomous mode command light green (no requirement for Game Piece Intake Acquired).
 *
 * 3. LED set 3 usage EnableDisable LEDView subsystem.
 *  Enabled mode green slow blink; disabled mode red slow blink.
 *
 * 4. LED set 4 usage HistoryDemo LEDView subsystem.
 *  HistoryFSM subsystem displays random colors that don't repeat for awhile (time history).
 *  Periodic color changing initiated by pressing "Y" button then self perpetuating. Colors also
 *  change if the "Y" button is pressed. Runs in enabled mode.
 *
 * 5. LED set 5 usage AchieveHueGoal LEDView subsystem.
 *  AchieveHueGoal class-based controller runs continuously and responds to its goal setting
 *  subsystem. Colors on color wheel position show PID controller converging on a color selected
 *  by Xbox right trigger axis (press trigger axis a little to start; press "A" button to stop).
 *
 * 6. Console Terminal usage GroupDisjoint subsystem.
 *  Disjoint Sequential Group Demo console output initiated by entering teleop enable mode.
 *  Show that subsystem default command doesn't run within a group command unless the command with
 *  the subsystem requirement is disjointed from the group by using a Proxy structure or separated
 *  commands structure.
 *
 * All commands are interruptible. Some button presses are debounced.
 */

/*
 * Example program demonstrating:
 *
 * Splitting a common resource (string of LEDs into multiple separately used resources).
 * Configure button trigger.
 * Triggers.
 * Use of command parameters set at command creation time.
 * Use of command parameters set at dynamically at runtime (Suppliers).
 * Use of method reference.
 * Some commentary on composite commands and mode changes.
 * Command logging.
 * Configuring an autonomous command.
 * Use of Xbox controller to produce fake events.
 * Use of Xbox controller to trigger an event.
 * Use of public command factories in subsystems.
 * Overloading method parameter types.
 * No commands with the word Command in the name.
 * No triggers with the word Trigger in the name.
 * Supplier of dynamic LED pattern.
 * Static LED pattern.
 * Restrict Subsystem Default Command to none until set once at any time and then unchangeable.
 * Goal as a resource setting for a subsystem.
 * Default commands can either run or not run within a sequential group depending on how the group is defined using Proxy.
 * Commands run in sequence by triggering successive commands.
 * Use of Measure<Time>.
 */

/*
 * Default Commands can be useful but they normally do not run within grouped commands even if their
 * associated subsystem is not active at all times within the group.
 *
 * There are several possibilities to accommodate that restriction:
 *  1. do without default commands at any time.
 *  2. do without the default command only within the group.
 *  3. manually code the function of the default command within a group.
 *  4. break groups into smaller groups and use Triggers to sequence multiple groups.
 *  5. use Proxy branching out of the group restriction.
 *
 * Using Triggers to sequence successive commands may help better organize the command flow and
 * isolate some subsystem requirements so the default command can run. That’s okay and is preferred
 * to using proxy commands.
 *
 * Usage of Proxies to hide the subsystem requirements from normal checks and thus allow the
 * default command to activate could be useful but should be used extremely sparingly by an
 * experienced user.
 *
 * The possibility of unintended consequences is very high if bypassing the normal checks of
 * requirements. Usage should be limited to when you can easily understand what exactly you’re
 * doing by invoking them.
 *
 * Judicious use of asProxy() on a group’s interior commands can often allow default commands to
 * run correctly within groups. Incorrect application of Proxy results in an extremely difficult
 * problem to debug.
 *
 * Slapping an asProxy() around a composed command isn’t sufficient. You have to use proxy the
 * inner commands, also or instead, and any new library commands to ease the use of Proxy aren’t
 * recursive to inner layers.
 *
 * After thoroughly understanding the structure of your groups extremely carefully add asProxy() to
 * as few interior commands as possible to accomplish what you need.
 *
 * Known problems:
 *
 * Proxies break the sanity check that warn of a subsystem running in parallel commands. There is no
 * warning - just wrong results (a warning might be added to the new combined Proxy library
 * functions but that helps only if you use those functions correctly and nothing says you have to).
 *
 * repeatedly() doesn’t repeat correctly due to a bug in the WPILib. (The repeat is different than
 * the original proxy command).
 *
 * andThen() doesn’t work but that can be circumvented by using sequence().
 */

/*
 * Uses of a Subsystem's Default Command that can cause unexpected results:
 *
 * Default Command normally does't run in a group of commands. Not using default commands can
 * prevent that inconsistency but then you lose the benefits of default commands.
 *
 * Default Command can be set more than once but only the last one set is active. It might not be
 * obvious which Default Command is being used. Never setting more than one default command can
 * prevent that confusion.
 *
 * A default Command doesn't have to be set and can be removed after setting. Again it might not be
 * obvious what default command is active. Consider mistake-proofing strategies to prevent
 * confusion.
 */

/*
 * This example program runs in real or simulated mode of the 2024 WPILib.
 *
 * This is a refactor and extension of code donated by ChiefDelphi @illinar. It is intended to
 * demonstrate good programming based on @Oblarg's rules.
 *
 * The use of the InternalTrigger to sequence command runs was donated by ChiefDelphi @bovlb
 *
 * Any errors or confusions are the fault and responsibility of ChiefDelphi @SLAB-Mr.Thomas; github
 * tom131313.
 */

package edu.wpi.first.wpilibj.examples.commandbasedbestpracticeled;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

public class Robot extends TimedRobot {
  private RobotContainer m_robotContainer;
  private Command m_autonomousSignal;
  private Command m_disjointedSequenceTests;

  @Override
  public void robotInit() {
    m_robotContainer = new RobotContainer();
  }

  @Override
  public void robotPeriodic() {
    // get a consistent set of all inputs
    m_robotContainer.runBeforeCommands();
    // check all the triggers and run all the scheduled commands
    CommandScheduler.getInstance().run();
    // write outputs such as logging, dashboards, indicators, and goal-oriented subsystem periodic
    m_robotContainer.runAfterCommands();
  }

  @Override
  public void disabledInit() {} // Commands running from another mode haven't been cancelled.

  @Override
  public void disabledPeriodic() {}

  @Override
  public void disabledExit() {}

  @Override
  public void autonomousInit() {
    // Commands running from another mode haven't been cancelled directly but may be interrupted by
    // this command.
    m_autonomousSignal = m_robotContainer.setAutonomousSignal();

    if (m_autonomousSignal != null) {
      m_autonomousSignal.schedule();
    }
  }

  @Override
  public void autonomousPeriodic() {}

  @Override
  public void autonomousExit() {
    if (m_autonomousSignal != null) {
      m_autonomousSignal.cancel();
    }
  }

  @Override
  public void teleopInit() {
    // Commands running from another mode haven't been cancelled directly except the one below.

    if (m_autonomousSignal != null) {
      m_autonomousSignal.cancel();
    }

    m_disjointedSequenceTests = m_robotContainer.getDisjointedSequenceTest();

    if (m_disjointedSequenceTests != null) {
      m_disjointedSequenceTests.schedule();
    }
  }

  @Override
  public void teleopPeriodic() {}

  @Override
  public void teleopExit() {}

  @Override
  public void testInit() {
    // Running commands are all cancelled. Default Commands will activate especially the disjoint
    // test defaults if the disjoint test was prematurely aborted leaving the defaults in place.
    // That behavior can be changed with more logic if need be - no need for this simple demo.
    CommandScheduler.getInstance().cancelAll();
  }

  @Override
  public void testPeriodic() {}

  @Override
  public void testExit() {}
}
