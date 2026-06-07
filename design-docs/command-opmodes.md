# Summary

This document describes how opmodes are implemented in command-based projects.

# Motivation

See [opmodes](opmodes.md) for the motivation for adding operator selectable opmodes to the core robot structure.  For command-based programs, the entire robot program is structured around command-based subsystems and commands tied to robot states and inputs, where everything can be constructed during initialization, so for more natural integration with the rest of the command-based framework, a different approach than the annotation-based approach used for periodic opmodes is warranted. It will also be possible for teams to mix command-based and non-command-based opmodes in the same project, using annotations.

# Background

A significant portion of FRC teams use the command-based framework, which builds on top of the periodic model with the concepts of subsystems and commands and a scheduler that provides cooperative multitasking. Subsystem objects are typically member variables of the `Robot` class (or a separate class called `RobotContainer`), and command objects are created and bound to joystick buttons or other triggers during construction.  In this code model, most behaviors are modeled as commands configured during robot construction, and very little is done explicitly iteratively.  Subsystems have a standardized `periodic()` method that is called by the command scheduler, which is most often used for telemetry.  In the provided template code, the command scheduler is set up to run in all modes because it is called by the `robotPeriodic()` method.  Triggers can be configured to run specific commands at the start of disabled/teleop/auto/test modes.  Operator selection of autonomous is still done via `SendableChooser`, with the `SendableChooser` value usually being a `Command` that is started when the auto mode is started.  Notably, this model shares both subsystems and command implementations between all operational modes–only the commands that are being run change; in general this has been seen as a benefit because it enables reuse (e.g. a "set elevator to height" command is useful in both autonomous and teleoperated modes, and a subsystem operates fundamentally the same in both modes).

The command-based framework is an approach and implementation for structuring robot programs with the key concepts being subsystems, commands, and triggers.  Commands require certain subsystems to run.  While multiple commands can be running simultaneously, only one command can be running that requires a particular subsystem, so subsystems effectively provide mutually exclusive behavior.  If a command is started that requires a subsystem that some other command also requires, the currently running command is canceled.  Triggers are used to start commands; triggers are boolean suppliers that can be combined with logical operations (e.g. and, or).  A command can be started based on a trigger value (true or false) or a change in trigger value (true to false or false to true).  A command scheduler (run periodically, including when the robot is disabled) handles trigger updating and command scheduling, and also runs `periodic()` on each subsystem.

The 2025 version of the command-based framework provides a [`RobotModeTriggers`](https://github.wpilib.org/allwpilib/docs/release/java/edu/wpi/first/wpilibj2/command/button/RobotModeTriggers.html) class that provides triggers for each of the fixed robot modes (autonomous, teleop, test, and disabled) available in the 2025 FRC system.

In addition, third-party command-based frameworks inspired by WPILib's are also used by many FTC teams. While these frameworks are are developed by FTC students and mentors and are not officially supported by FIRST or the FTC SDK, they are widely used by FTC teams as they offer similar benefits to WPILib's command-based framework, most notably the ability to reuse commands between OpModes. The core concepts of command-based programming are the same, with users defining subsystems and commands, but the usage patterns are different. Teams create OpModes and register them with annotations, and each OpMode typically has its own set of triggers and commands, often registered in the OpMode's `start` method. Because command-based OpModes are registered with annotations just like standard Opmodes, it is possible (and somewhat common) to mix command-based and non-command-based OpModes in the same project.

# Design

Unlike the non-command-based approach, the command-based framework generally favors a design where commands can be used in all modes and explicit periodic code is discouraged.  In addition, the general approach for "modern" commands eschews classes, preferring a "fluent" method chained builder approach.  To support this, opmode registration for command-based is performed with function calls instead of separate annotated classes.

The design for multiple opmodes in the command-based framework extends the current `RobotModeTriggers` approach in two important ways:
- Instead of fixed modes, opmodes are explicitly created by the user
- The opmodes provide multiple triggers; this makes it possible to tie behaviors to DS selection/initialization as well as the enabled period

A unique `RobotBase` class is also provided that allows users to create and register opmodes in their `Robot` constructor, and provides factory functions for creating opmodes of different types (auto, teleop, test/utility).

## OpModeTriggers

The `OpModeTriggers` class provides triggers to enable users to tie into each section of a opmode's lifetime.  Triggers have the ability to be tied to specific commands or actions on both transitions (e.g. false to true, true to false) and while the condition is in a particular state.  The framework will ensure these triggers always start in false state, even if code is started while attached to the field, so that it's safe to attach an action to the false to true transition of these triggers.

```java
public class OpModeTriggers {
  // opmode is selected on DS (regardless of enabled or disabled)
  public Trigger loaded();

  // opmode is running, robot is enabled
  public Trigger enabled();

  // opmode is selected, robot is disabled
  public Trigger disabled();
}
```

## CommandOpModes

The `CommandOpModes` class provides static factory methods for creating `OpModeTriggers` objects. Each of these static factory methods register the opmode with `RobotState` and return a new `OpModeTriggers` object. Note that calling `RobotState.publishOpModes()` is still required to publish the opmodes to the Driver Station.

```java
public final class CommandOpModes {
  // static factory methods for creating OpModeTriggers objects
  public static OpModeTriggers createAutoOpMode(String name);
  public static OpModeTriggers createTeleopOpMode(String name);
  public static OpModeTriggers createUtilityOpMode(String name);

  // other overloads available for group, description, and colors
}
```

## CommandRobot (v3 Only)

The `CommandRobot` class extends `OpModeRobot` and provides static factory methods for creating `OpModeTriggers` objects. Using this class is the recommended way to register opmodes in a robot program that uses both command-based and periodic opmodes.

```java
public abstract class CommandRobot extends OpModeRobot {
  // these register the opmode and return a new OpModeTriggers object for it
  public OpModeTriggers createAutoOpMode(String name);
  public OpModeTriggers createTeleopOpMode(String name);
  public OpModeTriggers createUtilityOpMode(String name);

  // other overloads available for group, description, and colors
}
```

These static methods register opmodes with `RobotState` directly and return `OpModeTriggers` objects that can be used to define behaviors (though `RobotState.publishOpModes()` must be called to publish the opmodes to the Driver Station).

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

    // Publish opmodes to the Driver Station
    RobotState.publishOpModes();
  }

  private void addSimpleAuto() {
    // A simple autonomous opmode
    createAutoOpMode("Simple Auto").enabled().whileTrue(Autos.simpleAuto(this));
  }

  private void addPathAuto(String path) {
    // A complex autonomous opmode that loads a path when selected in the DS while still disabled
    OpModeTriggers opmode = createAutoOpMode(path, "Follow Path");
    opmode.loaded().onTrue(Commands.runOnce(() -> Paths.loadPath(path)));
    opmode.enabled().whileTrue(Autos.followPath(this, path));
  }

  private void addArcadeTeleop() {
    // A teleop opmode with joystick and button controls
    OpModeTriggers opmode = createTeleopOpMode("teleop");

    var driverController = new CommandXboxController(1);

    // Control the drive with split-stick arcade controls
    opmode.setDefaultCommand(
        drive,
        drive.arcadeDriveCommand(
            () -> -driverController.getLeftY(), () -> -driverController.getRightX()));

    // Deploy the intake with the X button
    opmode.enabled(driverController.x()).onTrue(intake.intakeCommand());
    // Retract the intake with the Y button
    opmode.enabled(driverController.y()).onTrue(intake.retractCommand());
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

# Commands v2 vs v3 OpMode Support

The opmode functionality described in this document is implemented differently across Commands framework versions:

## Commands v3 (Java Only)

Commands v3 provides full opmode integration through:

- **`CommandRobot` class**: Extends `OpModeRobot` to enable safe mixing of command-based and periodic opmodes
- **Automatic scope management**: Commands v3 has better scope control that prevents commands, triggers, and other framework objects from leaking between opmodes
- **Safe opmode transitions**: The framework automatically manages command lifecycle during opmode changes, ensuring clean teardown and initialization

This design allows teams to use both command-based opmodes (for behaviors that benefit from the command framework) and traditional periodic opmodes (for simple state machines or legacy code) within the same robot program without interference.

## Commands v2 (Java and C++)

Commands v2 provides a subset of opmode functionality through utility classes:

- **`CommandOpModes` utility class**: Static factory methods for creating `OpModeTriggers` objects
- **`OpModeTriggers` class**: Provides `loaded()`, `enabled()`, and `disabled()` triggers for command binding
- **No automatic scope isolation**: Commands and triggers created in one opmode may persist and interfere with other opmodes
- **Best for purely command-based programs**: Recommended when all robot behaviors are implemented as commands

The lack of automatic scope management in Commands v2 means teams should be cautious when mixing command-based and periodic approaches, as commands scheduled in one opmode may continue running when switching to a different opmode that doesn't expect them.

## Recommendation

- Use **Commands v3** when you need to mix command-based and periodic opmodes safely
- Use **Commands v2** for purely command-based robot programs where all behaviors are modeled as commands
- For C++ programs, Commands v2 is the only option (v3 is Java-only due to C++ coroutine limitations)

# Future Work

We also intend to support robot startup-time binding of default commands to OpModes. Currently, if a default command is set for a mechanism while an OpMode is running, that default command will be bound to that specific OpMode. However, there is no way to bind a default command to an OpMode that is not currently running. We intend to add the following methods to the `OpModeTriggers` class:

```java
public class OpModeTriggers {
  // existing methods

  // bind a default command to this specific OpMode
  public void setDefaultCommand(Mechanism mechanism, Command command);

  // unbind a default command from this OpMode
  public void removeDefaultCommand(Mechanism mechanism);
}
```
