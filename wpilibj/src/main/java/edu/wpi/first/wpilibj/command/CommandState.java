package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.Timer;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.function.Consumer;

public class CommandState {
    /**
     * The time since this command was initialized.
     */
    private double m_startTime = -1;

    /**
     * The time (in seconds) before this command "times out" (or -1 if no timeout).
     */
    private double m_timeout = -1;

    /**
     * Whether or not this command has been initialized.
     */
    private boolean m_initialized;

    /**
     * The required subsystems.
     */
    private final Collection<Subsystem> m_requirements = new HashSet<>();

    /**
     * Whether or not it is running.
     */
    private boolean m_running;

    /**
     * Whether or not it is interruptible.
     */
    private boolean m_interruptible = true;

    /**
     * Whether or not it has been canceled.
     */
    private boolean m_canceled;

    /**
     * Whether or not it has been locked.
     */
    private boolean m_locked;

    /**
     * Whether this command should run when the robot is disabled.
     */
    private boolean m_runWhenDisabled;

    /**
     * Whether or not this command has completed running.
     */
    private boolean m_completed;

    /**
     * The {@link CommandGroup} this is in.
     */
    private CommandGroup m_parent;

    private List<Consumer<Command>> m_initActions = new ArrayList<>();
    private List<Consumer<Command>> m_executeActions = new ArrayList<>();
    private List<Consumer<Command>> m_interruptActions = new ArrayList<>();
    private List<Consumer<Command>> m_endActions = new ArrayList<>();

    /**
     * Called to indicate that the timer should start. This is called right before {@link
     * Command#initialize() initialize()} is, inside the {@link Command#run() run()} method.
     */
    private void startTiming() {
        m_startTime = Timer.getFPGATimestamp();
    }

    /**
     * Prevents further changes from being made.
     */
    synchronized void lockChanges() {
        m_locked = true;
    }

    /**
     * If changes are locked, then this will throw an {@link IllegalUseOfCommandException}.
     *
     * @param message the message to say (it is appended by a default message)
     */
    synchronized void validate(String message) {
        if (m_locked) {
            throw new IllegalUseOfCommandException(message
                    + " after being started or being added to a command group");
        }
    }

    /**
     * Sets the parent of this command. No actual change is made to the group.
     *
     * @param parent the parent
     * @throws IllegalUseOfCommandException if this {@link Command} already is already in a group
     */
    synchronized void setParent(CommandGroup parent) {
        if (m_parent != null) {
            throw new IllegalUseOfCommandException(
                    "Can not give command to a command group after already being put in a command group");
        }
        lockChanges();
        m_parent = parent;
    }

    /**
     * Returns whether the command has a parent.
     *
     * @return true if the command has a parent.
     */
    synchronized boolean isParented() {
        return m_parent != null;
    }

    /**
     * This is used internally to mark that the command has been started. The lifecycle of a command
     * is:
     *
     * <p>startRunning() is called. run() is called (multiple times potentially) removed() is called.
     *
     * <p>It is very important that startRunning and removed be called in order or some assumptions of
     * the code will be broken.
     */
    synchronized void startRunning() {
        m_running = true;
        m_startTime = -1;
    }

    /**
     * Returns whether or not the command is running. This may return true even if the command has
     * just been canceled, as it may not have yet called {@link Command#interrupted()}.
     *
     * @return whether or not the command is running
     */
    synchronized boolean isRunning() {
        return m_running;
    }

    /**
     * This will cancel the current command. <p> This will cancel the current command eventually. It
     * can be called multiple times. And it can be called when the command is not running. If the
     * command is running though, then the command will be marked as canceled and eventually removed.
     * </p> <p> A command can not be canceled if it is a part of a command group, you must cancel the
     * command group instead. </p>
     *
     * @throws IllegalUseOfCommandException if this command is a part of a command group
     */
    public synchronized void cancel() {
        if (m_parent != null) {
            throw new IllegalUseOfCommandException("Can not manually cancel a command in a command "
                    + "group");
        }
        _cancel();
    }

    /**
     * This works like cancel(), except that it doesn't throw an exception if it is a part of a
     * command group. Should only be called by the parent command group.
     */
    @SuppressWarnings("MethodName")
    synchronized void _cancel() {
        if (isRunning()) {
            m_canceled = true;
        }
    }

    /**
     * Returns whether or not this has been canceled.
     *
     * @return whether or not this has been canceled
     */
    public synchronized boolean isCanceled() {
        return m_canceled;
    }

    /**
     * Whether or not this command has completed running.
     *
     * @return whether or not this command has completed running.
     */
    public synchronized boolean isCompleted() {
        return m_completed;
    }
}
