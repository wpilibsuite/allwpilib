/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import java.util.*;
import java.util.function.Consumer;


import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.RobotState;
import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.buttons.Trigger.ButtonScheduler;
import edu.wpi.first.wpilibj.newbuttons.NewTrigger;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

public final class SchedulerNew extends SendableBase {
    /**
     * The Singleton Instance.
     */
    private static SchedulerNew instance;

    /**
     * Returns the {@link SchedulerNew}, creating it if one does not exist.
     *
     * @return the {@link SchedulerNew}
     */
    public static synchronized SchedulerNew getInstance() {
        if (instance == null) {
            instance = new SchedulerNew();
        }
        return instance;
    }

    private final Map<ICommand, CommandState> m_scheduledCommands = new HashMap<>();
    private final Map<Subsystem, ICommand> m_requirements = new HashMap<>();
    private final Collection<Subsystem> m_subsystems = new HashSet<>();

    /**
     * A hashtable of active {@link Command Commands} to their {@link LinkedListElement}.
     */
    @SuppressWarnings("PMD.LooseCoupling")
    private final Hashtable<Command, LinkedListElement> m_commandTable = new Hashtable<>();

    /**
     * Whether or not we are currently disabled.
     */
    private boolean m_disabled;
    /**
     * A list of all {@link Command Commands} which need to be added.
     */
    @SuppressWarnings({"PMD.LooseCoupling", "PMD.UseArrayListInsteadOfVector"})
    private NetworkTableEntry m_namesEntry;
    private NetworkTableEntry m_idsEntry;
    private NetworkTableEntry m_cancelEntry;
    /**
     * A list of all {@link edu.wpi.first.wpilibj.buttons.Trigger.ButtonScheduler Buttons}. It is
     * created lazily.
     */
    @SuppressWarnings("PMD.LooseCoupling")
    private Collection<NewTrigger.ButtonScheduler> m_buttons;
    private boolean m_runningCommandsChanged;

    private final List<Consumer<ICommand>> m_initActions = new ArrayList<>();
    private final List<Consumer<ICommand>> m_executeActions = new ArrayList<>();
    private final List<Consumer<ICommand>> m_interruptActions = new ArrayList<>();
    private final List<Consumer<ICommand>> m_endActions = new ArrayList<>();

    /**
     * Instantiates a {@link Scheduler}.
     */
    private SchedulerNew() {
        HAL.report(tResourceType.kResourceType_Command, tInstances.kCommand_Scheduler);
        setName("Scheduler");
    }

    /**
     * Adds a button to the {@link Scheduler}. The {@link Scheduler} will poll the button during its
     * {@link Scheduler#run()}.
     *
     * @param button the button to add
     */
    @SuppressWarnings("PMD.UseArrayListInsteadOfVector")
    public void addButton(NewTrigger.ButtonScheduler button) {
        if (m_buttons == null) {
            m_buttons = new HashSet<>();
        }
        m_buttons.add(button);
    }

    public void scheduleCommand(ICommand command, boolean interruptible) {

        if (!m_disabled && (!RobotState.isDisabled() || command.getRunWhenDisabled())) {

            Collection<Subsystem> requirements = command.getRequirements();

            if (Collections.disjoint(m_requirements.keySet(), requirements)) {
                command.initialize();
                for (Consumer<ICommand> action : m_initActions) {
                    action.accept(command);
                }
                for (Subsystem requirement : requirements) {
                    CommandState scheduledCommand = new CommandState(interruptible);
                    m_scheduledCommands.put(command, scheduledCommand);
                    m_requirements.put(requirement, command);
                }
            } else {
                boolean allInterruptible = true;
                for (Subsystem requirement : requirements) {
                    if (m_requirements.keySet().contains(requirement)) {
                        allInterruptible &= m_scheduledCommands.get(m_requirements.get(requirement)).isInterruptible();
                    }
                }
                if (allInterruptible) {
                    for (Subsystem requirement : requirements) {
                        ICommand toInterrupt = m_requirements.get(requirement);
                        if (toInterrupt != null) {
                           cancelCommand(toInterrupt);
                        }
                    }
                    command.initialize();
                    for (Consumer<ICommand> action : m_initActions) {
                        action.accept(command);
                    }
                }
            }
        }
    }

    /**
     * Runs a single iteration of the loop. This method should be called often in order to have a
     * functioning {@link Command} system. The loop has five stages:
     *
     * <ol> <li>Poll the Buttons</li> <li>Execute/Remove the Commands</li> <li>Send values to
     * SmartDashboard</li> <li>Add Commands</li> <li>Add Defaults</li> </ol>
     */
    @SuppressWarnings({"PMD.CyclomaticComplexity", "PMD.NPathComplexity"})
    public void run() {
        if (!m_disabled) {
            for (Subsystem subsystem : m_subsystems) {
                subsystem.periodic();
            }

            for (NewTrigger.ButtonScheduler button : m_buttons) {
                button.execute();
            }

            for (ICommand command : m_scheduledCommands.keySet()) {

                if (RobotState.isDisabled() && !command.getRunWhenDisabled()) {
                    cancelCommand(command);
                    break;
                }

                command.execute();
                for (Consumer<ICommand> action : m_executeActions) {
                    action.accept(command);
                }
                if (command.isFinished()) {
                    command.end();
                    for (Consumer<ICommand> action : m_endActions) {
                        action.accept(command);
                    }
                    m_scheduledCommands.remove(command);
                    m_requirements.keySet().removeAll(command.getRequirements());
                }
            }

            for (Subsystem subsystem : m_subsystems) {
                if (!m_requirements.containsKey(subsystem)) {
                    if (subsystem.getDefaultICommand() != null) {
                        scheduleCommand(subsystem.getDefaultICommand(), true);
                    }
                }
            }
        }
    }

    /**
     * Registers a {@link Subsystem} to this {@link Scheduler}, so that the {@link Scheduler} might
     * know if a default {@link Command} needs to be run. All {@link Subsystem Subsystems} should call
     * this.
     *
     * @param subsystem the system
     */
    void registerSubsystem(Subsystem subsystem) {
        m_subsystems.add(subsystem);
    }

    public void cancelCommand(ICommand command) {
        command.interrupted();
        for (Consumer<ICommand> action : m_interruptActions) {
            action.accept(command);
        }
        m_scheduledCommands.remove(command);
        m_requirements.keySet().removeAll(command.getRequirements());
    }


    public void cancelAll() {
        for (ICommand command : m_scheduledCommands.keySet()) {
            cancelCommand(command);
        }
    }

    public double timeSinceInitialized(ICommand command) {
        CommandState commandState = m_scheduledCommands.get(command);
        if (commandState != null) {
            return commandState.timeSinceInitialized();
        } else {
            return -1;
        }
    }

    public boolean isRunning(ICommand command) {
        return m_scheduledCommands.containsKey(command);
    }

    /**
     * Disable the command scheduler.
     */
    public void disable() {
        m_disabled = true;
    }

    /**
     * Enable the command scheduler.
     */
    public void enable() {
        m_disabled = false;
    }

    /**
     * Adds an action to perform on the initialization of any command by the scheduler.
     *
     * @param action the action to perform
     */
    public void onCommandInitialize(Consumer<ICommand> action) {
        m_initActions.add(action);
    }

    /**
     * Adds an action to perform on the execution of any command by the scheduler.
     *
     * @param action the action to perform
     */
    public void onCommandExecute(Consumer<ICommand> action) {
        m_executeActions.add(action);
    }

    /**
     * Adds an action to perform on the interruption of any command by the scheduler.
     *
     * @param action the action to perform
     */
    public void onCommandInterrupted(Consumer<ICommand> action) {
        m_interruptActions.add(action);
    }

    /**
     * Adds an action to perform on the ending of any command by the scheduler.
     *
     * @param action the action to perform
     */
    public void onCommandEnd(Consumer<ICommand> action) {
        m_endActions.add(action);
    }

    @Override
    public void initSendable(SendableBuilder builder) {
        builder.setSmartDashboardType("Scheduler");
        m_namesEntry = builder.getEntry("Names");
        m_idsEntry = builder.getEntry("Ids");
        m_cancelEntry = builder.getEntry("Cancel");
        builder.setUpdateTable(() -> {
            if (m_namesEntry != null && m_idsEntry != null && m_cancelEntry != null) {
                // Get the commands to cancel
                double[] toCancel = m_cancelEntry.getDoubleArray(new double[0]);
                if (toCancel.length > 0) {
                    for (LinkedListElement e = m_firstCommand; e != null; e = e.getNext()) {
                        for (double d : toCancel) {
                            if (e.getData().hashCode() == d) {
                                e.getData().cancel();
                            }
                        }
                    }
                    m_cancelEntry.setDoubleArray(new double[0]);
                }

                if (m_runningCommandsChanged) {
                    // Set the the running commands
                    int number = 0;
                    for (LinkedListElement e = m_firstCommand; e != null; e = e.getNext()) {
                        number++;
                    }
                    String[] commands = new String[number];
                    double[] ids = new double[number];
                    number = 0;
                    for (LinkedListElement e = m_firstCommand; e != null; e = e.getNext()) {
                        commands[number] = e.getData().getName();
                        ids[number] = e.getData().hashCode();
                        number++;
                    }
                    m_namesEntry.setStringArray(commands);
                    m_idsEntry.setDoubleArray(ids);
                }
            }
        });
    }
}
