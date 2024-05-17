package edu.wpi.first.wpilibj3.command.async;

/**
 * Represents the state of a command as it is executed by the scheduler.
 *
 * @param command      The command being tracked.
 * @param parent       The parent command composition that scheduled the tracked command. Null if the
 *                     command was scheduled by a top level construct like trigger bindings or manually
 *                     scheduled by a user. For deeply nested compositions, this tracks the <i>direct
 *                     parent command</i> that invoked the schedule() call; in this manner, an ancestry
 *                     tree can be built, where each {@code CommandState} object references a parent
 *                     node in the tree.
 * @param continuation The continuation to which the command is bound.
 */
record CommandState(
    AsyncCommand command,
    AsyncCommand parent,
    Continuation continuation) {
}
