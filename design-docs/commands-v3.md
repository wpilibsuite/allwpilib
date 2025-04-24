# WPILib Commands Version 3

[TOC]

## Problem Statement {#problem-statement}

The WPILib command framework uses cooperative multitasking to execute many commands concurrently;
each command has its own `execute()` function called periodically by the scheduler’s `run()` loop.
This API share has a steep learning curve, since new students learn looping by writing their own
`for` or `while` loops - not by using a framework that does the looping for them. Programmers
unfamiliar with the command framework commonly run into a problem where they write a while loop
inside their command’s `execute()` function, thus stalling the scheduler (and, by extension, the
entire robot program) by never ceding control until its end condition is met.

Version 3 of the commands framework proposes to fix this issue by leveraging a new API introduced in
Java 21: continuations. While they are currently an internal JDK API used to underpin virtual thread
runtimes, we can still access and use it. Continuations are used by the JDK to take and save a
snapshot of a mounted function’s call stack and registers, and can be remounted later to continue
from where it left off; a canceled command would simply never be remounted, and its continuation
left for the garbage collector to clean up.

## Goals

### Make command logic look like normal functions

The hardest part of learning the v1 and v2 command frameworks is the splitting out of logic into
separate init/execute/end/isFinished methods. Coroutines allow us to merge those all back into a
single function:

```java
void commandBody(Coroutine coroutine) {
  initialize();
  while (!isFinished()) {
    execute();
    coroutine.yield();
  }
  end();
}
```

Coroutines allow commands to be normal methods that yield control back to the scheduler mid-method;
the scheduler will issue a continuation to each command, and the JVM can unmount a continuation -
saving the stack and registers - which can be remounted later. **The primary goal of the new
framework is to allow for commands to be written as normal methods by taking advantage of this
mount/unmount feature.** Everything else is focused on quality of life improvements to the
framework.

### Forced Naming

The v2 commands framework makes naming opt-in; it is very easy to forget to set a name on a command
in a factory method, making telemetry less useful than it should be; seeing a
“SequentialCommandGroup” in logs isn’t very useful. The v3 framework will make command names
required in order for commands to be constructed. See [Telemetry](#telemetry) for details.

### Uncommanded Behavior

Command groups - or, more generally, commands that schedule other commands - need to own all
resources needed by all their inner commands. However, if they don’t *directly* control a resource,
then they will still own the resource whenever they’re not running an inner command that uses it,
causing the resource to be in an *uncommanded state*. This can cause unexpected behavior with
parallel groups where inner commands don’t all end at the same time, and especially with sequential
groups. For example, something as seemingly basic as `elevator.toL4().andThen(coral.score())` would
*own* the elevator and coral resources for the entire sequence, but not control the elevator during
the coral scoring section, nor control the coral scoring mechanism while the elevator is moving.

The v2 framework worked around this problem with so-called proxy commands, which have no
requirements of their own and simply schedule and await the *real* command. This removed the
requirement for their used subsystems from the parent group, and allowed for the subsystem’s default
command to run after the proxied command completed. It also meant that *every* command that required
that subsystem would also need to be proxied; otherwise, the group would still have ownership of the
subsystem, and the proxied command would interrupt it (and thus itself).

The v3 framework allows nested commands to use resources not required by the parent command;
however, the built-in parallel and sequential groups will take full ownership of all resources
required by their nested commands for safety and to keep behavior parity with previous command
frameworks.

```java
public Command outerCommand() {
  // The outer command only requires the outer resource
  return run(coroutine -> {
    // But can schedule a command that requires any other resource
    coroutine.await(innerResource.innerCommand());
    // And only requires that inner resource while its command runs
    coroutine.await(otherResource().otherCommand());
  }).named("Outer");
}
```

Scheduling a command that requires an inner resource will also cancel its parent, even if the parent
does not require that inner resource. Using the above example, directly scheduling a command that
requires `innerResource` will cancel a running `outerCommand` - but only if `outerCommand` is
*currently using* the inner resource at the time.

### Priority Levels

Priority levels allow for finer-grained control over when commands should be interruptible than a
binary interrupt/ignore interrupt flag. This can be particularly helpful for LED control, where
teams use lights to indicate robot activity or error states with error indicators taking priority.

### Suspend/Resume

Allow commands to be temporarily paused while a higher priority or interrupting command runs, then
be automatically resumed after that higher priority command finishes. Note that resuming commands
must check for a condition to determine if they should still be running, since the time between
pause and resume can be arbitrarily long.

Suspending a command will suspend all its children, regardless of if those children have declared
themselves suspendable or not. Likewise, cancelling a non-suspendable command will also cancel all
its children, even if some of those children are suspendable.

Suspending a command that already has suspended children will still suspend everything; however,
upon resume, those suspended children will remain suspended.

### Improved Transparency

The v2 commands framework only runs top-level commands in the scheduler; commands nested within a
composition like a sequential group or a simple `withTimeout` decoration are hidden from the
scheduler and do not appear in its sendable implementation, making telemetry difficult. A
“SequentialCommandGroup” in logs is less useful than “Wait 10 seconds -> Drive Forward”. Command
groups will automatically include the names of all their constituent commands, with options to
override when desired.

The v3 framework will also provide a map of what command each resource is owned by. The v2 framework
only provides a list of the names of the running commands, without mapping those to subsystems. This
also makes telemetry difficult, since the order of commands in the list does not correspond with
subsystems; a command at a particular index may require different subsystems than a different
command at that same index that’s running at a later point in time, making data analysis in
AdvantageScope difficult since we can’t rely on consistent ordering.

## Non-Goals

### Preemptive Multitasking

It is not a goal of the v3 framework to offer preemptive multitasking (that is, having the scheduler
forcibly suspend a long-running command in order to run another queued one). This would require
support from the JDK for allowing custom virtual thread schedulers, which it does not currently
offer, and a custom thread scheduler would be complicated and difficult to maintain.

### Multithreading Support

Like the previous iterations of the command-based framework, v3 will be designed for a
single-threaded environment. All commands will be run by a single thread, which is expected to be
the same thread on which commands are scheduled and canceled via triggers. No guarantees are made
for stability or proper functioning if used in a multithreaded environment.


## Implementation Details

### Nomenclature 

**Schedule**: Adding a command to a queue in the scheduler, requesting that that command start
running in the next call to the scheduler’s `run()` method.

**Mount**: Make a command active by resuming its stack and executing it on the scheduler’s thread.

**Unmount**: Freeze a command’s stack and state and remove it from the scheduler’s thread. An
unmounted command may be re-mounted in the future.

**Run**: Adding a command to the scheduler and have it begin executing. Every unfinished command
will be run by the scheduler until they reach a `Coroutine.yield()` call, at which point the command
is unmounted and the next command can begin.

**Cancel**: Request that a command stop running. A command that is scheduled and hasn’t yet started
running will be removed from the queue of scheduled commands. Cancellation requests are handled at
the end of each `run() `invocation. Cancelled commands are unmounted and will not be re-mounted;
they must be rescheduled and be issued new carrier coroutines.

**Interrupt**: Scheduling a command that requires one or more resources already in use by a running
command will interrupt and cancel that running command, so long as the running command has an equal
or lower priority level. Higher-priority commands cannot be interrupted by lower-priority ones.

### Coroutines and Continuations

`Continuation` and `ContinuationScope` are JDK-internal classes that manage stack saving and
loading (“unmounting” and “mounting”, respectively). Mounting a continuation essentially means
placing the continuation’s saved stack on top of the current stack. Calling
`Continuation.yield(ContinuationScope)` pauses the code that the continuation is evaluating and
cedes control back to the caller, which can then unmount the continuation. The scheduler uses this
by looping over every running command, mounting the continuation for the command, running the
command - which eventually either returns or calls `coroutine.yield()` - then unmounts the
continuation and moves on to the next command in the list.

Coroutines are essentially wrappers around a continuation primitive that allows additional access to
the scheduler and provides async/await-like functionality. For example, `Coroutine.await(Command)`
will schedule a given command and call `yield()` in a loop until that command has completed
execution (note: this does *not* naively call `command.run(coroutine)`, which would hide the inner
command from the scheduler and potentially sidestep requirement mutexing)

### Command Function Bodies

Command logic lives in a single function `run` that accepts a `Coroutine` object argument. All
yielding is done via that coroutine, as is any management of nested commands (scheduling, awaiting,
cancelling, etc).

The coroutine’s `yield()` method always returns `true`. When a command is cancelled, it is removed
from the set of running commands and its carrier objects left to be garbage collected. An
`onCancel()` method on the cancelled command will be invoked to let the command do any necessary
cleanup; this is needed because the command body will not be re-mounted and run, so a separate
function is used. Command builders allow this to be specified with a `whenCancelled(Runnable)` step.

### Command Lifetimes

All commands are managed and executed by the scheduler. No commands should manually run their inner
commands (which is what the old sequential and parallel groups did), since that hides those inner
commands from the scheduler and telemetry. However, this is not prohibited, nor would there be a way
to detect it.

Inner commands are bound to the lifetime of their parents. Cancelling a higher-level command must
also cancel all the commands it scheduled (and so on down the hierarchy). Suspending a higher-level
command must likewise also suspend all inner commands, and resuming the higher-level command must
resume all suspended inner commands. Inner commands cannot be resumed before their parent command,
but may be suspended while the parent command still runs.

When a command is running, the scheduler tracks in a wrapper CommandState object that includes the
command, the command that scheduled it (if applicable), and the coroutine object carrying the
command.

Each command run has its own coroutine to back its execution. When a command finishes, its backing
continuation is done, and the command is removed from the scheduler. Any unfinished inner commands
are cancelled at this point.

### Factory Methods and Builders

In contrast with v2’s decorator API that returns a new command object for each modification (which
had some issues with naming and making telemetry difficult with deeply nested wrapper objects), v3
uses builder objects that allow configuration to be set before creating a single command at the end.

#### Forced Naming

Builder objects will only permit command creation when a name for the command has been provided.

#### Automatic Naming

Parallel and sequential groups and builders permit automatically generated names to be used instead
of manually specified ones.

Automatic sequence names will simply be the list of all the commands in the sequence, separated by
a “->” arrow sign; for example, “Step 1 -> Step 2 -> Step 3”

Automatic parallel group names will be the list of the names of the commands in the group, separated
by a pipe “|”. Required commands will be in their own group, separated from non-required commands.
If either subgroup is empty, then there will be no indication in the output (ie, no empty group
token like “()” will appear)

### Cooperative Multitasking

All commands *must* yield control back to the coroutine in their control loops. Because we cannot
preempt a running command at any arbitrary point, the command must explicitly and deliberately cede
control back to the scheduler in order for the framework to function. Commands may do this by
calling `coroutine.yield()` on the injected coroutine object, which ultimately delegates to
`Continuation.yield()` to pause the carrier continuation object.

The scheduler has a single `ContinuationScope` object. When a command is mounted, the scheduler will
create a new `Continuation` object bound to that scope, and a `Coroutine` object bound to the
continuation. `ContinuationScope` and `Continuation` are currently JDK-private classes; the build
needs to open the module to the wpilib/unnamed module for reflective access at compile and runtime.
Because access is entirely reflective, wrapper classes at the library level will be used to make
access easier.

### Scheduling

Scheduling a command with `Scheduler.schedule()` will add the command to a queue of pending
commands. Any command in the queue that conflicts and has a lower priority will be removed from the
queue without calling the `onCancel` hook. At the start of `Scheduler.run()`, all commands that
conflict with those in the queue are cancelled, then the queued commands are promoted to running.

Scheduling an inner command will bypass the queue and immediately begin to run. This avoids delays
caused by loop timings for deeply nested commands.

### Scheduler `run()` Cycle

1. Poll the event loop for triggers (which may queue or cancel commands)
2. Run periodic sideload functions
3. Promote scheduled commands to running
    1. Cancels any running commands that conflict with scheduled ones
4. Iterate over running commands
    1. Mount
    2. Run until yield point is reached or an error is raised
    3. Unmount
    4. Evict if the command finished
        1. Any inner command still running is canceled
5. Schedule default commands for the next iteration to pick up and start running

### Interruption

* Caused by scheduling a command that requires resources already in use by one or more running
  commands
* Results in cancellation of the conflicting commands
    * Commands are moved to a pending-cancellation state
    * Then when `run()` is next called, they are removed from the scheduler
        * This occurs prior to the scheduled commands being promoted to the running state
    * The conflicting command’s entire tree (all ancestors and children) will be cancelled
* Siblings in a composition can interrupt each other
    * eg `fork(command1); fork(command2)`, where `command1` and `command2` share at least one
      common requirement, would cause `command1` to be interrupted by `command2`

### Telemetry

Scheduler state is serialized using protobuf. The scheduler will send a list of the currently queued
commands and a list of the current running commands. Commands are serialized as (id: uint32,
parent_id: uint32, name: string, priority: int32, required_resources: string array). Consumers can
use the `id` and `parent_id` attributes to reconstruct the tree structure, if desired. `id` and
`parent_id` marginally increase the size of serialized data, but make the schema and deserialization
quite simple.
