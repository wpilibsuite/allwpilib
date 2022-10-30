# Command-Based Specification

## Notations

Comments in blockquotes are intended 
to give the reader a deeper understanding of the discussed topics,
and do not give any guarantees -- though they are likely to be true.

> The reader of this text is sitting on a chair.

Per the blockquote comment, the reader is likely to be sitting on a chair.
However, that is not guaranteed: they may be standing, lounging on a couch,
lying in a bed, or floating three meters in the air.  

## Command

### Command Lifecycle

A command's lifecycle is split into four stages, each represented by a method.

#### void initialize(void)

> This method is called once on command start.

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

Returning `true` marks the end of the command, and `end(false)` will be called.

#### void end(bool interrupted)

> This method will be called when the command stops running.

Any resources opened in `initialize` should be closed here.

`end(false)` will be called only following a call to `isFinished()` that returned `true`.
Calls to `end(false)` that
were not closely preceded by `isFinished()` returning `true` are undefined behavior.

`end(true)` marks that the command has been cancelled, and may be called at any time -- including before `initialize`.

No more lifecycle methods will be called on the command instance after `end` for this current usage.
`initialize` will be called at the start of any following reuse.
