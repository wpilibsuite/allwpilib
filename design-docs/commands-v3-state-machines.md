# State Machines in WPILib Commands Version 3

- See [Commands v3](commands-v3.md) for details on the commands framework

## Problem Statement {#problem-statement}

Coroutines are a powerful way to express low- to high-complexity behaviors. However, they become unwieldy at
representing highly complex behaviors where phases may be repeated or skipped to at any point in the sequence. State
machines excel at this by providing ways to transition from any arbitrary state to any other arbitrary state, flattening
the declarative structure of a coroutine into a linear sequence of states and transitions.

Example: consider a FRC game like 2022 Rapid React or 2017 Steamworks. The robot has a drivetrain, a hopper to store
balls, a turret to aim at a goal, and a flywheel shooter to launch balls at the goal. We want an autonomous mode to
drive to a known position on the field for optimal scoring, then aim at the goal, fire balls until the hopper is empty,
and finally play an LED animation to indicate the end of the autonomous sequence. If the robot is moved away from the
scoring location, the scoring portion of the sequence should stop and the robot should move back into position, and then
resume the scoring sequence.

```java
public Command autoWithStateMachine() {
  // Declare the state machine
  StateMachine stateMachine = new StateMachine("Auto With State Machine");

  // Define states
  State getInPosition = stateMachine.addState(drivetrain.driveToScoringLocation());
  State aiming = stateMachine.addState(turret.aimAtGoal());
  State scoring = stateMachine.addState(shooter.fireOnce());
  State celebrating = stateMachine.addState(leds.celebrate());

  // Set the initial state. Neglecting this will cause a runtime exception when the state machine starts.
  stateMachine.setInitialState(getInPosition);

  // Switch to aiming when we reach the scoring location.
  getInPosition.switchTo(aiming).whenComplete();
  // Set the swerve wheels in an X shape after reaching the scoring location to resist being pushed away.
  getInPosition.onExit(() -> Scheduler.getDefault().fork(drivetrain.setX()));

  // Then start scoring once the turret is aimed at the goal.
  aiming.switchTo(scoring).when(turret::aimedAtGoal);

  // Loop the scoring state as long as the hopper has a ball.
  scoring.switchTo(scoring).whenCompleteAnd(() -> hopper.hasBall());

  // Automatically interrupt any part of the aiming or scoring sequence if
  // the robot is moved away from the scoring location and move back into position.
  stateMachine.switchFromAny(aiming, scoring).to(getInPosition).when(atScoringLocation.negate());

  // Start celebrating once the final ball has been scored.
  scoring.switchTo(celebrating).whenCompleteAnd(() -> !hopper.hasBall());

  return stateMachine;
}
```

```java
Command autoWithCoroutines() {
  return Command.noRequirements().executing(coroutine -> {
    // Automatically score while the robot is in scoring position.
    // This will be canceled if the robot is bumped away from the scoring location.
    atScoringLocation.whileTrue(
        turret.aimAtGoal()
            .andThen(shooter.fireOnce().repeatWhile(hopper::hasBall))
            .andThen(leds.celebrate())
            .withAutomaticName()
    );
    // Move back into scoring position if the robot is bumped away from the scoring location.
    atScoringLocation.onFalse(drivetrain.driveToScoringLocation());

    coroutine.await(drivetrain.driveToScoringLocation());

    // Park to allow the triggered commands to run in the background.
    // We assume the command will be canceled at the end of the autonomous period.
    coroutine.park();
  }).named("Auto With Coroutines");
}
```

## Implementation Details {#implementation-details}

### Overview

The public entry point is `org.wpilib.commands3.StateMachine` which implements `Command`.

Each state machine is named; `name()` returns the provided name for telemetry and debugging. State machine names are
specified in the constructor; there is no dedicated builder like `NeedsNameBuilderStage.named(...)` for regular
commands.

`requirements()` returns an empty set; the machine itself does not own any Mechanism. The commands that back states may
have requirements, which will be inherited by the state machine while those states are active, just like a normal
command with nested children.

### Constructing a state machine

State machines are created with `new StateMachine(String)`. The name cannot be null. The `StateMachine` class is final
and cannot be subclassed; v1-style group creation that does setup in a subclass' constructor is not supported:

```java
// Not allowed
class CustomStateMachine extends StateMachine {
  // ...
}
```

States are defined with `addState(Command)`. Users need to manually wire states together using transition builders after
defining the states. A `State` object wraps the underlying command and is responsible for tracking the possible
transitions out of that state.

State machines have no initial state, which must be set explicitly:

```java
StateMachine stateMachine = new StateMachine("Example");
State initialState = stateMachine.addState(...);
stateMachine.setInitialState(initialState);
```

`setInitialState` throws an `NullPointerException` if given a null input. It may be called multiple times to override
the initial state before running.

`setInitialState` and all transitions require that both states belong to the same `StateMachine`
object; otherwise an `IllegalArgumentException` is thrown.

### State Machine Loop

A state machine is a regular command that manages the state lifecycle in its `run()` method. The lifecycle is managed by
a loop, where in each iteration the current state's command is scheduled, and then enters an inner loop that continues
to yield as long as the command is running, similar to `Coroutine.waitUntil`. However, the inner loop also checks for
state transitions before calling `yield()`; if a transition is determined to be active, the command is canceled and the
state moves to the transition's target state. The main loop is then restarted with the new state.

If a state's command finishes without triggering any transitions, the state machine checks for state completions. If
a completion is found, the state machine immediately moves to the completion's target state, and (to prevent a potential
infinite loop) conditionally inserts a `yield()` at the very end of the loop in case the command was a one-shot.

Pseudocode:

```
currentState = initialState

state_loop:
while currentState is not null:
  currentState.onEnter()
  fork currentState.command
  didYield = false

  while currentState.command is running
    for each transition in currentState.transitions
      if transition.check()
        currentState.onExit()
        currentState = transition.targetState
        restart state_loop

    didYield = true
    yield

  currentState.onExit()
  currentState = currentState.completions.find(completion -> completion.active())?.targetState

  if didYield is false and currentState is not null
    yield
```

Note that state completions are different from transitions: transitions are only active on rising edge, while state
completions are active on every loop iteration (but are only checked once, when the state exits).

### Transitions

There are two kinds of transitions: conditional transitions (checked while the originating state's
command is running) and completion transitions (taken after the originating state's command finishes
on its own, if no conditional transition was taken).

Transitions are configured using a staged builder setup similar to command builders. The initial builder stage starts
with one or more originating states, then moves to a stage for specifying a target state (which may be null, indicating
that the state machine should exit), and finally to a stage for specifying the condition that triggers the transition.

Transitions start from one or more non-null originating states, and end with a single target state (which may be null,
indicating that the state machine should exit), and a condition that triggers the transition. Transitions are stored on
the originating states, rather than being stored on the state machine (this simplifies the implementation). Each
originating state gets its own copy of the transition.

Transitions can be defined starting from the state machine itself with `StateMachine.switchFromAny(...)`, or from a
specific starting state with `State.switchTo(...)`:

```
stateMachine.switchFromAny(state1, state2).to(state3).when(...)

// Identical to:
state1.switchTo(state3).when(...)
state2.switchTo(state3).when(...)
```

Builders have private constructors, so they cannot be instantiated directly. Users must use the fluent builder factories
with `StateMachine.switchFromAny(...)` or `State.switchTo(...)`.

Transitions must be resilient to commands that transition to themselves like `state.switchTo(state).when(...)`. If the
condition is naively checked, the state machine will enter an infinite loop before it would naturally yield (check
transition -> cancel command -> enter new state -> check transition -> ...). To avoid
this, the condition is checked with rising-edge logic so that the transition is only triggered once per internal loop
iteration:

```java
class Transition {
  State targetState;
  BooleanSupplier signal;
  boolean previousSignal;

  boolean shouldTransition() {
    boolean currentSignal = signal.getAsBoolean();
    boolean signalBecameTrue = currentSignal && !previousSignal;
    previousSignal = currentSignal;
    return signalBecameTrue;
  }
}
```

#### Exiting the state machine explicitly

`State.switchTo(State)` is to be used for state-to-state transitions and cannot accept `null` as an input. To support
exiting the state machine explicitly, there are two additional methods available on the builders:

`State.exitStateMachine()` builds a transition that bypasses the null check and makes the user's intent clear, versus
`State.switchTo(null)` which may be confusing.

`TransitionNeedsTargetStage.exitStateMachine()` is a convenience method that returns a transition to `null`.

```
state.switchTo(null).when(...) // NullPointerException
state.exitStateMachine().when(...) // OK
stateMachine.switchFromAny(state1, state2).toExitStateMachine().when(...)
```

### Callbacks on state entry/exit

States maintain a list of `Runnable` callbacks that are run when the state is entered, and a separate list of callbacks
that are run when the state is exited. Callbacks are run in the order they were added.

Entry callbacks are run immediately after the state's command is forked, so it can see the current command. However,
one-shot commands will complete in the `fork` call, so entry callbacks will not see it.

Exit callbacks are run immediately before canceling the state's command (if the exit was caused by a transition). For
states that complete without a transition, the exit callbacks are run immediately after the state's command finishes and
before the next state is selected.

### Runtime semantics

- When a state becomes active:
    1) The state's command is scheduled via `coroutine.fork(state.command)`.
    2) `onEnter` callbacks run.
    3) While the command is running each scheduler iteration:
        - All conditional transitions from this state are checked in insertion order; each evaluates
          `shouldTransition()`. The first to trigger wins.
        - If a transition triggers: `onExit` callbacks run, the command is canceled, and the next state is set. The
          machine immediately begins the next loop iteration with the new state without an extra yield; the next state's
          command can start in the same scheduler run. If the next state is null, the machine exits.
        - If no transition triggers: the coroutine yields once for this iteration.
- If the command stops running without any conditional transition firing:
    - `onExit` callbacks run.
    - The next state is selected from completion transitions in insertion order. If none match, the machine exits (next
      state is null).
    - To ensure fairness and prevent tight looping with one-shot commands, the machine guarantees at least one yield per
      state. If the state command finished without ever yielding (one-shot), the machine yields once before starting the
      next state's command (unless exiting).

### Edge cases and guarantees

- Self-transition is supported; the rising-edge guard ensures only a single re-entry per loop when the condition rises.
  The exiting command is canceled and then immediately re-scheduled.
- One-shot commands should use completion transitions to continue the flow; conditional transitions cannot trigger for
  them because the commands exit before conditional transitions can be checked.
- If multiple transitions are configured with the same condition on the same state, only the first will ever trigger in
  a given loop iteration.
- Transitions cannot target states in a different state machine; an exception is thrown if attempted.
- The initial state must be set explicitly; otherwise the machine throws on first run and will not
  remain scheduled.
