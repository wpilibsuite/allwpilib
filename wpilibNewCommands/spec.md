# Command-Based Specification

## Notations

### Blockquotes

Comments in blockquotes are intended 
to give the reader a deeper understanding of the discussed topics,
and do not give any guarantees -- though they are likely to be true.

> The reader of this text is sitting on a chair.

Per the blockquote comment, the reader is likely to be sitting on a chair.
However, that is not guaranteed: they may be standing, lounging on a couch,
lying in a bed, or floating three meters in the air.  

### RFC 2119
The keywords "MUST", "MUST NOT", "SHALL", "SHALL NOT",
"SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL"
in this document are
to be interpreted as described in [RFC 2119](https://datatracker.ietf.org/doc/html/rfc2119).

Note that this document does not use the RFC 2119 definition of the term "REQUIRED";
any usage of the term "REQUIRED"
in this document follows the definition [here](#Required Subsystems).

## Command

### Command Properties

Commands have the following properties.
The values of command properties MUST not change over the course of the program,
and any operations to query the value of properties MUST be idempotent.  

#### Required Subsystems

Each command SHOULD declare the set of subsystems it controls;
this set is referred to from here on as the command's _requirements_.
A command's _requirements_ MAY be the empty set.    
Subsystems SHALL NOT be _required_ by more than one (1) running command at a time;
conflicts SHALL be resolved according to the [Interruption Behavior](#Interruption Behavior) of the command
that is presently running.
Requirement collisions MUST be checked and resolved
before calling [`initialize`](#void-initializevoid) when scheduling a command.

#### Interruption Behavior

This property dictates the resolution strategy of [requirement](#required-subsystems) conflicts:
what happens when another command sharing requirements with this one is scheduled.

The following strategies are defined in this specification:
- `CancelSelf`: this command is interrupted, [`end(true)`](#void-endbool-interrupted) is called, and scheduling of the incoming command commences.
- `CancelIncoming`: scheduling of the incoming command is aborted, and this command continues running unaffected.

[//]: # (TODO: do we want to explicitly allow alternate interruption behaviors? do we want to dictate a default?)
[//]: # (      might be interesting to see alternate impls -- maybe coroutine-based in the future?)

#### Runs When Disabled

For implementations defining a "disabled mode",
this property dictates whether a command may run during it.
During "disabled mode", any commands not declared as "running when disabled"
MUST be interrupted (i.e. `end(true)` is called), and any attempts to schedule them MUST abort.

### Command Lifecycle

A command's lifecycle is split into four stages, each represented by a method.

#### void initialize(void)

> Calling this method marks the command start.

Any state needed for the command's functionality should be initialized here.
Any resources needed should be opened here, not in the constructor.
The command object's constructor should not be counted on for initialization:
the same command object might be reused multiple times,
`initialize` is guaranteed to be called at the start of every use. 
Any two command objects of the same type should be interchangeable for all purposes
after their `initialize` method is called.

#### void execute(void)

> This method is called once per scheduler iteration while the command is running.

This method may only be called between `initialize` and `end`.
Calling `execute` method before `initialize` or after `end` is undefined behavior and inherently unsafe.

#### bool isFinished(void)

> This method indicates when the command has finished, and is polled after each call to `execute()`.

This method may only be called between `initialize` and `end`.
Calling `isFinished` method before `initialize` or after `end` is undefined behavior and inherently unsafe.

Returning `true` signals that the command should end, and `end(false)` will be called.

#### void end(bool interrupted)

> This method marks the end of the command, and will be called when the command stops running.

Any resources opened in `initialize` should be closed here.

`end(false)` will be called only following a call to `isFinished()` that returned `true`.
Calls to `end(false)` that
were not closely preceded by `isFinished()` returning `true` are undefined behavior.

`end(true)` marks that the command has been cancelled, and may be called at any time -- including before `initialize`.

No more lifecycle methods will be called on the command instance after `end` for this current usage.
`initialize` will be called at the start of any following reuse.

### Command Compositions

Commands may be composed to achieve advanced functionality.
As a rule, a composition takes ownership of the commands composing it,
and they MUST NOT be scheduled individually.

This specification defines some core compositions,
implementations may define additional composition types.

#### Sequential

> A sequential command composition executes a series of commands in sequence,
starting each one after the previous one ends.

Interrupting a sequential composition forwards the interruption event to the currently-running command.

#### Parallel

> A parallel composition runs multiple commands in parallel
(note: the term "parallel" here is loosely defined -- no multithreading or other parallel-execution techniques are guaranteed to be used.
Implementations should specify whether they are multithreaded or require thread safety).

Interruptions are forwarded to all currently-running commands.

There are three types of parallel compositions, differing by what command in the composition ends the composition:
- First (this is also called a "race" group): composition ends when any command ends, all other commands are cancelled.
- Specific/Deadline: composition ends when a specific command ends, all commands running at that time are cancelled.
- Last: composition ends when all commands end.

#### Repeat

> A repeat composition restarts its wrapped command when it ends, until interrupted.

#### Select

> A selection composition selects at schedule-time one command of a given set.
 
Implementations may provide optimized versions of selection compositions.
