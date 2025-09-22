# Summary

This document describes how opmodes are implemented in command-based projects.

# Motivation

See [opmodes](opmodes.md) for the motivation for adding operator selectable opmodes to the core robot structure.  For command-based programs, the entire robot program is structured around command-based subsystems and commands tied to robot states and inputs, with everything constructed during initialization, so for more natural integration with the rest of the command-based framework, a different approach than the annotation-based approach used for periodic and linear opmodes is warranted.

# Background

A significant portion of FRC teams use the command-based framework, which builds on top of the periodic model with the concepts of subsystems and commands and a scheduler that provides cooperative multitasking. Subsystem objects are typically member variables of the `Robot` class (or a separate class called `RobotContainer`), and command objects are created and bound to joystick buttons or other triggers during construction.  In this code model, most behaviors are modeled as commands configured during robot construction, and very little is done explicitly iteratively.  Subsystems have a standardized `periodic()` method that is called by the command scheduler, which is most often used for telemetry.  In the provided template code, the command scheduler is set up to run in all modes because it is called by the `robotPeriodic()` method.  Triggers can be configured to run specific commands at the start of disabled/teleop/auto/test modes.  Operator selection of autonomous is still done via `SendableChooser`, with the `SendableChooser` value usually being a `Command` that is started when the auto mode is started.  Notably, this model shares both subsystems and command implementations between all operational modes–only the commands that are being run change; in general this has been seen as a benefit because it enables reuse (e.g. a "set elevator to height" command is useful in both autonomous and teleoperated modes, and a subsystem operates fundamentally the same in both modes).

The command-based framework is an approach and implementation for structuring robot programs with the key concepts being subsystems, commands, and triggers.  Commands require certain subsystems to run.  While multiple commands can be running simultaneously, only one command can be running that requires a particular subsystem, so subsystems effectively provide mutually exclusive behavior.  If a command is started that requires a subsystem that some other command also requires, the currently running command is canceled.  Triggers are used to start commands; triggers are boolean suppliers that can be combined with logical operations (e.g. and, or).  A command can be started based on a trigger value (true or false) or a change in trigger value (true to false or false to true).  A command scheduler (run periodically, including when the robot is disabled) handles trigger updating and command scheduling, and also runs `periodic()` on each subsystem.

The 2025 version of the command-based framework provides a [`RobotModeTriggers`](https://github.wpilib.org/allwpilib/docs/release/java/edu/wpi/first/wpilibj2/command/button/RobotModeTriggers.html) class that provides triggers for each of the fixed robot modes (autonomous, teleop, test, and disabled) available in the 2025 FRC system.

# Design

Unlike the non-command-based approach, the command-based framework generally favors a design where commands can be used in all modes and explicit periodic code is discouraged.  In addition, the general approach for "modern" commands eschews classes, preferring a "fluent" method chained builder approach.  To support this, opmode registration for command-based is performed with function calls instead of separate annotated classes.

The design for multiple opmodes in the command-based framework extends the current `RobotModeTriggers` approach in two important ways:
- Instead of fixed modes, opmodes are explicitly created by the user
- The opmodes provide multiple triggers; this makes it possible to tie behaviors to DS selection/initialization as well as the enabled period

A unique `RobotBase` class is also provided that always runs the command scheduler periodically in all modes, including disabled.  This means it is not possible to mix command-based opmodes and non-command-based opmodes; all opmodes in a command-based project must be command-based opmodes.

## CommandOpModes

The `CommandOpModes` class provides factory functions for creating opmodes.

```java
public final class CommandOpModes {
  // these register the opmode and return a new CommandOpMode object for it
  // (also includes default versions so group and description are optional)
  //
  // Calling twice with the same name is a runtime error (exception is thrown)
  public static CommandOpMode autonomous(String name, String group, String description) {...}
  public static CommandOpMode teleoperated(String name, String group, String description) {...}
  public static CommandOpMode test(String name, String group, String description) {...}
}
```

## CommandOpMode

The `CommandOpMode` class provides triggers to enable users to tie into each section of a opmode's lifetime.  Triggers have the ability to be tied to specific commands or actions on both transitions (e.g. false to true, true to false) and while the condition is in a particular state.  The framework will ensure these triggers always start in false state, even if code is started while attached to the field, so that it's safe to attach an action to the false to true transition of these triggers.

```java
public class CommandOpMode {
  // opmode is selected on DS (regardless of enabled or disabled)
  public final Trigger selected;

  // opmode is selected, robot is disabled
  public final Trigger disabled;

  // opmode is running, robot is enabled
  public final Trigger running;
}
```

## CommandRobot

The `CommandRobot` class is the base class for the user's command-based `Robot` class.  It also implements the private library machinery for robot startup and robot execution, and provides factory functions for creating opmodes.

```java
public abstract class CommandRobot {
  public void beforeScheduler() {
    // this code is called periodically in all robot modes of operation before the scheduler is run
  }

  public void afterScheduler() {
    // this code is called periodically in all robot modes of operation after the scheduler is run
  }

  // exposed so users may override it if desired, but generally shouldn't be necessary to do so
  public void robotPeriodic() {
    beforeScheduler();
    CommandScheduler.run();
    afterScheduler();
  }
}
```

## Subsystem enhancements

The `Subsystem` interface adds an overload of `setDefaultCommand` to support creating per-opmode default commands, and similarly a new `removeDefaultCommand` overload.  The non-`CommandOpMode` overloads are applied in all opmodes where a per-opmode default has not been set.

```java
class Subsystem {
  // Sets a default command for this subsystem active only in the given opmode.
  default void setDefaultCommand(CommandOpMode opmode, Command defaultCommand);

  // Sets a default command for this subsystem active in all opmodes where there's no per-opmode default command.
  default void setDefaultCommand(Command defaultCommand);

  // Removes the default command for the given opmode.  No effect if none set.
  default void removeDefaultCommand(CommandOpMode opmode);

  // Removes the default command for all opmodes where there's no per-opmode default command.
  default void removeDefaultCommand();
}
```

## Java Robot Code Examples

The template/example code for command-based Java includes the following:
- A Robot class with subsystems and constructor that sets up a command-based teleop opmode, a couple of auto opmodes, and a test opmode

Robot:

```java
public class Robot extends CommandRobot {
  // actuators (Subsystem derived)
  public final Drive drive = new Drive();
  public final Intake intake = new Intake();
  public final Storage storage = new Storage();

  // sensors (not Subsystem derived)
  public final Vision vision = new Vision();

  public Robot() {
    // Automatically disable and retract the intake whenever the ball storage is full.
    storage.hasCargo.onTrue(intake.retractCommand());

    // Create auto opmodes
    addSimpleAuto();
    addPathAuto("drive and turn");

    // Create teleop opmodes
    addArcadeTeleop();
  }

  private void addSimpleAuto() {
    // A simple autonomous opmode
    CommandOpModes.autonomous("Simple Auto").running.whileTrue(Autos.simpleAuto(this));
  }

  private void addPathAuto(String path) {
    // A complex autonomous opmode that loads a path when selected in the DS while still disabled
    CommandOpMode opmode = CommandOpModes.autonomous(path, "Follow Path");
    opmode.selected.onTrue(Commands.runOnce(() -> Paths.loadPath(path)));
    opmode.running.whileTrue(Autos.followPath(this, path));
  }

  private void addArcadeTeleop() {
    // A teleop opmode with joystick and button controls
    CommandOpMode opmode = CommandOpModes.teleoperated("teleop");

    var driverController = new CommandXboxController(1);

    // Control the drive with split-stick arcade controls
    m_drive.setDefaultCommand(
        opmode,
        drive.arcadeDriveCommand(
            () -> -driverController.getLeftY(), () -> -driverController.getRightX()));

    // Deploy the intake with the X button
    opmode.running.and(driverController.x()).onTrue(intake.intakeCommand());
    // Retract the intake with the Y button
    opmode.running.and(driverController.y()).onTrue(intake.retractCommand());
  }

  @Override
  public void beforeScheduler() {
    // this code is called periodically in all robot modes of operation before the scheduler is run

    // make sure we process vision inputs ahead of scheduled commands
    vision.process();
  }

  @Override
  public void afterScheduler() {
    // this code is called periodically in all robot modes of operation after the scheduler is run

    // output telemetry from all subsystems
    Telemetry.log("drive", drive);
    Telemetry.log("intake", intake);
    Telemetry.log("storage", storage);
  }
}
```

Autos:

```java
public class Autos {
  public static Command simpleAuto(Robot robot) {...}
  public static Command followPath(Robot robot, String path) {...}
}
```

# Drawbacks

Command-based opmodes and non-command-based opmodes cannot be intermixed in a single project.  The benefit of this is cleaner integration with the rest of the command-based framework, including proper handling of periodic subsystem behaviors in disabled mode, which otherwise might be a common error for teams.  In addition, command-based subsystems would need to expose significant non-command-based behaviors to make them usable in non-command-based opmodes, which would likely create potential for additional issues in writing normal command-based code (where subsystem implementation details should be black-boxed from commands).

# Alternatives

Make command-based opmodes interoperate with non-command opmodes by using the same base class and only having the scheduler run if one of those opmodes is active.

# Trades

- Binding teleop joysticks is very verbose if different behavior is desired in different teleop opmodes.  Maybe add to CommandOpMode a separate event loop that's only active in that opmode?  At the minimum, it may make sense to add CommandOpMode overloads to joysticks so users could write `driverController.x(teleop)` instead of `teleop.running.and(driverController.x())`?
- Is the `beforeScheduler()` / `afterScheduler()` split beneficial?  Or should we just have `robotPeriodic()` showing the `CommandScheduler.run()` code in it like we do in 2025?
- Related: should we remove the built-in `periodic()` from the `Subsystem` interface?  Split it into before/after?

# Unresolved Questions
