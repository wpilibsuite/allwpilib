/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.RobotState;
import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.buttons.Trigger;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

@SuppressWarnings({"PMD.GodClass", "PMD.TooManyMethods"})
public final class SchedulerNew extends SendableBase {
  /**
   * The Singleton Instance.
   */
  private static SchedulerNew instance;

  /**
   * Returns the Scheduler instance.
   *
   * @return the instance
   */
  public static synchronized SchedulerNew getInstance() {
    if (instance == null) {
      instance = new SchedulerNew();
    }
    return instance;
  }

  private final Map<ICommand, CommandState> m_scheduledCommands = new LinkedHashMap<>();
  private final Map<Subsystem, ICommand> m_requirements = new LinkedHashMap<>();
  private final Collection<Subsystem> m_subsystems = new HashSet<>();


  private boolean m_disabled;

  @SuppressWarnings({"PMD.LooseCoupling", "PMD.UseArrayListInsteadOfVector"})
  private NetworkTableEntry m_namesEntry;
  private NetworkTableEntry m_idsEntry;
  private NetworkTableEntry m_cancelEntry;

  @SuppressWarnings("PMD.LooseCoupling")
  private Collection<Trigger.ButtonSchedulerNew> m_buttons;

  private final List<Consumer<ICommand>> m_initActions = new ArrayList<>();
  private final List<Consumer<ICommand>> m_executeActions = new ArrayList<>();
  private final List<Consumer<ICommand>> m_interruptActions = new ArrayList<>();
  private final List<Consumer<ICommand>> m_endActions = new ArrayList<>();

  private SchedulerNew() {
    HAL.report(tResourceType.kResourceType_Command, tInstances.kCommand_Scheduler);
    setName("Scheduler");
  }

  /**
   * Adds a button binding to the scheduler, which will be polled to schedule commands.
   *
   * @param button the button to add
   */
  public void addButton(Trigger.ButtonSchedulerNew button) {
    if (m_buttons == null) {
      m_buttons = new HashSet<>();
    }
    m_buttons.add(button);
  }

  /**
   * Schedules a command for execution.  Does nothing if the command is already scheduled.
   * If a command's requirements are not available, it will only be started if all the commands
   * currently using those requirements have been scheduled as interruptible.  If this is
   * the case, they will be interrupted and the command will be scheduled.
   *
   * @param command the command to schedule
   * @param interruptible whether this command can be interrupted
   */
  @SuppressWarnings("PMD.CyclomaticComplexity")
  public void scheduleCommand(ICommand command, boolean interruptible) {

    if (CommandGroupBase.getGroupedCommands().contains(command)) {
      throw new IllegalUseOfCommandException(
          "A command that is part of a command group cannot be independently scheduled");
    }

    if (m_disabled
        || (RobotState.isDisabled() || !command.getRunWhenDisabled())
        || m_scheduledCommands.containsKey(command)) {
      return;
    }

    Collection<Subsystem> requirements = command.getRequirements();

    if (Collections.disjoint(m_requirements.keySet(), requirements)) {
      command.initialize();
      CommandState scheduledCommand = new CommandState(interruptible);
      m_scheduledCommands.put(command, scheduledCommand);
      for (Consumer<ICommand> action : m_initActions) {
        action.accept(command);
      }
      for (Subsystem requirement : requirements) {
        m_requirements.put(requirement, command);
      }
    } else {
      boolean allInterruptible = true;
      for (Subsystem requirement : requirements) {
        if (m_requirements.keySet().contains(requirement)) {
          allInterruptible &=
              m_scheduledCommands.get(m_requirements.get(requirement)).isInterruptible();
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

  /**
   * Runs a single iteration of the scheduler.  The execution occurs in the following order:
   *
   * <p>Subsystem periodic methods are called.
   *
   * <p>Button bindings are polled, and new commands are scheduled from them.
   *
   * <p>Currently-scheduled commands are executed.
   *
   * <p>End conditions are checked on currently-scheduled commands, and commands that are finished
   * have their end methods called and are removed.
   *
   * <p>Any subsystems not being used as requirements have their default methods started.
   */
  @SuppressWarnings({"PMD.CyclomaticComplexity", "PMD.NPathComplexity"})
  public void run() {
    if (!m_disabled) {
      for (Subsystem subsystem : m_subsystems) {
        subsystem.periodic();
      }

      for (Trigger.ButtonSchedulerNew button : m_buttons) {
        button.execute();
      }

      for (ICommand command : m_scheduledCommands.keySet()) {

        if (RobotState.isDisabled() && !command.getRunWhenDisabled()) {
          cancelCommand(command);
          continue;
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
        if (!m_requirements.containsKey(subsystem) && subsystem.getDefaultICommand() != null) {
          scheduleCommand(subsystem.getDefaultICommand(), true);
        }
      }
    }
  }

  void registerSubsystem(Subsystem subsystem) {
    m_subsystems.add(subsystem);
  }

  /**
   * Cancels a command.  The scheduler will only call the interrupted method of a canceled command,
   * not the end method.
   *
   * @param command the command to cancel
   */
  public void cancelCommand(ICommand command) {
    command.interrupted();
    for (Consumer<ICommand> action : m_interruptActions) {
      action.accept(command);
    }
    m_scheduledCommands.remove(command);
    m_requirements.keySet().removeAll(command.getRequirements());
  }

  /**
   * Cancels all commands that are currently scheduled.
   */
  public void cancelAll() {
    for (ICommand command : m_scheduledCommands.keySet()) {
      cancelCommand(command);
    }
  }

  /**
   * Returns the time since a given command was scheduled.  Note that this only works on commands
   * that are directly scheduled by the scheduler; it will not work on commands inside of
   * commandgroups, as the scheduler does not see them.
   *
   * @param command the command to query
   * @return the time since the command was scheduled, in seconds
   */
  public double timeSinceScheduled(ICommand command) {
    CommandState commandState = m_scheduledCommands.get(command);
    if (commandState != null) {
      return commandState.timeSinceInitialized();
    } else {
      return -1;
    }
  }

  /**
   * Whether a given command is running.  Note that this only works on commands that are directly
   * scheduled by the scheduler; it will not work on commands inside of commandgroups, as the
   * scheduler does not see them.
   *
   * @param command the command to query
   * @return whether the command is currently scheduled
   */
  public boolean isScheduled(ICommand command) {
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

        Map<Double, ICommand> ids = new LinkedHashMap<>();


        for (ICommand command : m_scheduledCommands.keySet()) {
          ids.put((double) command.hashCode(), command);
        }

        double[] toCancel = m_cancelEntry.getDoubleArray(new double[0]);
        if (toCancel.length > 0) {
          for (double hash : toCancel) {
            cancelCommand(ids.get(hash));
            ids.remove(hash);
          }
          m_cancelEntry.setDoubleArray(new double[0]);
        }

        List<String> names = new ArrayList<>();

        ids.values().forEach(command -> names.add(command.getName()));

        m_namesEntry.setStringArray(names.toArray(new String[0]));
        m_idsEntry.setNumberArray(ids.keySet().toArray(new Double[0]));
      }
    });
  }
}
