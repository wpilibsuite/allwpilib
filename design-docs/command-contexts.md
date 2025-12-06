# Summary

This document describes how command contexts are implemented in command-based projects.

# Motivation

[Command opmodes](opmodes-commandbased.md) will significantly increase the number of command bindings that need to be restricted to particular conditions.  In addition, it should be a conscious decision to make bindings that are active regardless of opmode- Unknowingly doing so can be a major safety risk.

This documents proposes a solution to both easily specify restrictions for command bindings and require such restrictions.

# Background

Both FTC and FRC historically have had ways to automatically schedule commands in response to conditions such as driver inputs, but due to the different robot program setups, the methods and behaviors are different.  To provide context, a brief summary of these behaviors (as of 2025) is below:
- In FRC, state naturally persists throughout the entire robot program, so command bindings last for the entire robot program execution.  (They are created when the robot program starts and last until the robot program ends.)
- In FTC, opmodes should be as self contained as possible to avoid buggy behavior, so bindings are local to their opmode.  (They are created when the opmode starts and destroyed when the opmode ends)

# Design

Core concepts:
- A `BooleanSupplier` represents a condition.
- A `Context` describes when a set of `Trigger`s should be active.
- A `Trigger` represents a condition to use for command bindings.
- A `Trigger`’s *context condition* is the condition of the `Context` associated with the `Trigger`.
- A `Trigger`’s *base condition* is the original condition used for the `Trigger`, without the context condition applied.

Relevant library classes:
- `BooleanSupplier` (described above) is provided by the Java Standard Environment.
- `Context` is described above.  Although it contains a boolean condition, it should only be used for making `Trigger`s with that context condition, so it does _not_ implement `BooleanSupplier`.
- `Trigger` is described above.  It is made by a `Context` from a `BooleanSupplier` and implements `BooleanSupplier`.
- `Conditions` provides static utility methods for performing logical operations on `BooleanSupplier`s.
- `CommandOpModes` is part of [command opmodes](opmodes-commandbased.md), and provides some common `Context` instances.
- `CommandOpMode` is part of [command opmodes](opmodes-commandbased.md) and inherits from `Context`.
- `CommandGenericHID` is the base class of all command controller classes (described below).
- Command controller classes (such as `CommandXboxController`) are used to create `Trigger`s associated with a controller's inputs.

Robot code outline:
- Controllers are created with particular `Context`s to restrict the controller’s `Trigger` bindings to particular opmodes, optionally imposing additional conditions on the `Context`s.
- Subsystems provide `BooleanSupplier`s to indicate state. (e.g., arm at angle)
- Static methods in `Conditions` perform any necessary logic operations on the `BooleanSupplier`s.
- `Context`s are used to create `Trigger`s from `BooleanSupplier`s to make bindings for any automatic actions.

## Context

The `Context` class describes when a set of `Trigger`s should be active.

To avoid inadvertently making bindings that are active in more opmodes than expected, `Context`s can only be made from opmodes, or from an opmode type such as all opmodes or all teleop opmodes.

To prevent making `Context`s which can be true regardless of opmode (aside from `CommandOpModes.all` and `Context`s built from it), the only ways to create a `Context` are from the `CommandOpModes` static `Context`s, a `CommandOpMode`, a logical OR of existing `Context`s, and a logical AND of a `Context` with an arbitrary condition.  (Note that allowing a logical OR of a `Context` with an arbitrary condition would defeat this goal.)

```java
public class Context {
  // used by CommandOpMode and the internals of CommandOpModes
  protected Context(BooleanSupplier);

  // creates a Trigger with the given base condition
  public Trigger trigger(BooleanSupplier);
  public Trigger trigger(EventLoop, BooleanSupplier);

  public Context and(BooleanSupplier);
  public Context or(Context);
}
```

Open question: Should we add `Context.trigger()` and `Context.trigger(EventLoop)` to create `Trigger`s whose context condition is the context and whose base condition is the context being active?  (Note that a constant true base condition would not have any rising or falling edges to trigger Commands.)

## CommandOpModes

In addition to providing the factories described in [command opmodes](opmodes-commandbased.md), the `CommandOpModes` class provides provides static `Context` instances for types of opmodes.  Note that the number of opmodes of a particular type is unbounded\!

```java
public final class CommandOpModes {
  // always true
  public static final Context all;

  // true when the given robot mode is selected, regardless of enabled/disabled state
  public static final Context allAuto;
  public static final Context allTeleop;
  public static final Context allTest;

  // the autonomous(), teleoperated(), and test() CommandOpMode factories still exist
}
```

Open question: Are allAuto, allTeleop, and allTest worth defining for users?
Open question: Should allTeleop be renamed to allTeleoperated to match the CommandOpMode factory?

## CommandOpMode

In addition to providing the `Trigger`s described in [command opmodes](opmodes-commandbased.md), the `CommandOpMode` class extends `Context`, allowing any opmode to be used wherever a context is expected.  The condition of the `Context` is when the opmode is selected (regardless of enabled/disabled status).

Note that the `selected` trigger must have a base condition that has rising and falling edges.

```java
public class CommandOpMode extends Context {
  public final Trigger selected = trigger(...is selected...);
  public final Trigger disabled = selected.and(RobotState::isDisabled);
  public final Trigger enabled = selected.and(RobotState::isEnabled);
}
```

## Conditions

To help with operations on `BooleanSupplier`s, a `Conditions` class provides static utility methods.

Previously, these operations would be done by creating a `Trigger` and using the instance methods on that class.  However, using a `Trigger` for conditions not meant to trigger commands is not proper usage, especially with the new requirement that `Trigger`s always have an associated `Context`.  Defining a `Condition` functional interface with the appropriate utility methods was considered, but was decided against due to the confusion it would cause about when to use it instead of `Context`, `Trigger`, or `BooleanEvent`.

```java
public class Conditions {
  public static BooleanSupplier and(BooleanSupplier, BooleanSupplier);
  public static BooleanSupplier or(BooleanSupplier, BooleanSupplier);
  public static BooleanSupplier negate(BooleanSupplier);
  public static BooleanSupplier debounce(BooleanSupplier, double);
  public static BooleanSupplier debounce(BooleanSupplier, double, Debouncer.DebounceType);
}
```

## Trigger

A `Trigger` is a combination of a *context condition* and a *base condition* that can make command bindings.

Every `Trigger` is associated with a `Context`.  A `Trigger` with an inactive `Context` does not evaluate its base condition and does not activate any bindings.

Not evaluating the base condition is important for the controller classes, where evaluating the base condition leads to warnings if the controller is not connected.  Not evaluating the base condition when the context condition is false is a natural way to suppress those warnings when the controller is not used.

Previously, `Trigger`s would sometimes be used to indicate subsystem state.  Such usage is no longer appropriate because `Trigger`s should only be used to trigger commands.  Furthermore, `Trigger`s can only be made by `Context`s, but associating a `Context` with subsystem is inappropriate.  Instead, use `BooleanSupplier`s and the static utility methods provided by `Conditions`.  (Note that a static import allows simple usage as `and(cond1, cond2)` without needings `Conditions.` at the beginning.)

```java
public class Trigger implements BooleanSupplier {
  // package private for use by Context
  Trigger(Context, BooleanSupplier);
  Trigger(Context, EventLoop, BooleanSupplier);

  @Override
  public boolean getAsBoolean();

  public Trigger and(BooleanSupplier);
  public Trigger or(Trigger);
  public Trigger negate();
  public Trigger debounce(double);
  public Trigger debounce(double, Debouncer.DebounceType);

  // below are the existing methods to create command bindings
  public Trigger onChange(Command);
  public Trigger onTrue(Command);
  public Trigger onFalse(Command);
  public Trigger whileTrue(Command);
  public Trigger whileFalse(Command);
  public Trigger toggleOnTrue(Command);
  public Trigger toggleOnFalse(Command);
}
```

Missing detail: How should we resolve combining `Trigger`s with different contexts (or event loops)?
Missing detail: What should `getAsBoolean()` return when the context condition is false?
Missing detail: Should `onFalse()` schedule the command if the context condition has a falling edge while the base condition is true?
Missing detail: Should `whileTrue()` cancel the command if the context condition has a falling edge while the base condition is true?

Open question: Should we allow `Trigger or(BooleanSupplier)`?  Note that only the base condition would be OR'd, and the context condition would remain the same.  This may be confusing, and may have weird interactions with a) how we resolve combining `Trigger`s with different contexts and b) the return value of `getAsBoolean()` when the context condition is false.

## Command controllers

The command controller classes (e.g., `CommandXboxController`) have a mandatory context parameter for their constructors.  Otherwise, they have the same overload set as before.

```java
public class CommandGenericHID {
  // this replaces public CommandGenericHID(int);
  // otherwise, the API is the same, with the Context being used to create the Triggers
  public CommandGenericHID(int, Context);
}

public class CommandXboxController {
  // this replaces public CommandXboxController(int);
  // otherwise, the API is the same, with the Context being used to create the Triggers
  public CommandXboxController(int, Context);
}
```

## Sharing bindings across opmodes

There are two recommended structures for bindings that are common to multiple opmodes:
- Define a method with an opmode parameter that uses that parameter for all contexts, and invoke/call that method for each opmode with the shared bindings.
- Separate from opmode-specific bindings, make the bindings with the context being a combination of all opmodes with the shared bindings.

## Java robot code example

```java
public class Robot extends CommandRobot {
  // actuators (derived from Subsystem)
  public final Drive drive = new Drive();
  public final Intake intake = new Intake();
  public final Storage storage = new Storage();

  public Robot() {
    // Automatically disable and retract the intake whenever the ball storage is full in any opmode
    CommandOpModes().any.trigger(storage.hasCargo).onTrue(intake.retractCommand());

    // Create auto opmodes
    addSimpleAuto(CommandOpModes.autonomous("Simple Auto"));
    addPathAuto("drive and turn");

    // Create teleop opmodes
    var arcadeTeleop = CommandOpModes.teleoperated("Arcade Drive");
    addArcadeDriveBindings(arcadeTeleop);
    addIntakeBindings(arcadeTeleop);

    var tankTeleop = CommandOpModes.teleoperated("Tank Drive");
    addTankDriveBindings(tankTeleop);
    addIntakeBindings(tankTeleop);
  }

  private void addSimpleAuto(CommandOpMode opmode) {
    // A simple autonomous opmode
    opmode.running.whileTrue(Autos.simpleAuto(this));
  }

  private void addPathAuto(String path) {
    // A complex autonomous opmode that loads a path when selected in the DS while still disabled
    CommandOpMode opmode = CommandOpModes.autonomous(path, "Follow Path");
    opmode.selected.onTrue(Commands.runOnce(() -> Paths.loadPath(path)));
    opmode.running.whileTrue(Autos.followPath(this, path));
  }

  private void addIntakeBindings(CommandOpMode opmode) {
    var codriverController = new CommandXboxController(1, opmode);

    // Deploy the intake with the X button
    codriverController.x().onTrue(intake.intakeCommand());
    // Retract the intake with the Y button
    codriverController.y().onTrue(intake.retractCommand());
  }

  private void addArcadeDriveBindings(CommandOpMode opmode) {
    var driverController = new CommandXboxController(0, opmode);

    // Control the drive with split-stick arcade controls
    drive.setDefaultCommand(
        opmode,
        drive.arcadeDriveCommand(
            () -> -driverController.getLeftY(), () -> -driverController.getRightX()));
  }

  private void addTankDriveBindings(CommandOpMode opmode) {
    var driverController = new CommandXboxController(0, opmode);

    // Control the drive with split-stick tank controls
    drive.setDefaultCommand(
        opmode,
        drive.tankDriveCommand(
            () -> -driverController.getLeftY(), () -> -driverController.getRightY()));
  }
}

public class Autos {
  public static Command simpleAuto(Robot robot) {...}
  public static Command followPath(Robot robot, String path) {...}
}
```
