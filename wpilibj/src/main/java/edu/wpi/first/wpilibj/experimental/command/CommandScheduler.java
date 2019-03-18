/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.experimental.command;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.function.Consumer;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.RobotState;
import edu.wpi.first.wpilibj.SendableBase;
import edu.wpi.first.wpilibj.command.IllegalUseOfCommandException;
import edu.wpi.first.wpilibj.experimental.buttons.Trigger.ButtonScheduler;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * The scheduler responsible for running {@link Command}s.  A Command-based robot should call
 * {@link CommandScheduler#run()} on the singleton instance in its periodic block in order to
 * run commands synchronously from the main loop.  Subsystems should be registered with the
 * scheduler using {@link CommandScheduler#registerSubsystem(Subsystem)} in order for their
 * {@link Subsystem#periodic()} methods to be called and for their default commands to be scheduled.
 */
@SuppressWarnings({"PMD.GodClass", "PMD.TooManyMethods"})
public final class CommandScheduler extends SendableBase {
  /**
   * The Singleton Instance.
   */
  private static CommandScheduler instance;

  /**
   * Returns the Scheduler instance.
   *
   * @return the instance
   */
  public static synchronized CommandScheduler getInstance() {
    if (instance == null) {
      instance = new CommandScheduler();
    }
    return instance;
  }

  //A map from commands to their scheduling state.  Also used as a set of the currently-running
  //commands.
  private final Map<Command, CommandState> m_scheduledCommands = new LinkedHashMap<>();

  //A map from required subsystems to their requiring commands.  Also used as a set of the
  //currently-required subsystems.
  private final Map<Subsystem, Command> m_requirements = new LinkedHashMap<>();

  //The set of all subsystems that have been registered with the scheduler.
  private final Collection<Subsystem> m_subsystems = new HashSet<>();

  //The set of currently-bound buttons.
  private final Collection<ButtonScheduler> m_buttons = new LinkedHashSet<>();

  private boolean m_disabled;

  //NetworkTable entries for use in Sendable impl
  private NetworkTableEntry m_namesEntry;
  private NetworkTableEntry m_idsEntry;
  private NetworkTableEntry m_cancelEntry;

  //Lists of user-supplied actions to be executed on scheduling events for every command.
  private final List<Consumer<Command>> m_initActions = new ArrayList<>();
  private final List<Consumer<Command>> m_executeActions = new ArrayList<>();
  private final List<Consumer<Command>> m_interruptActions = new ArrayList<>();
  private final List<Consumer<Command>> m_endActions = new ArrayList<>();

  CommandScheduler() {
    HAL.report(tResourceType.kResourceType_Command, tInstances.kCommand_Scheduler);
    setName("Scheduler");
  }

  /**
   * Adds a button binding to the scheduler, which will be polled to schedule commands.
   *
   * @param button the button to add
   */
  public void addButton(ButtonScheduler button) {
    m_buttons.add(button);
  }

  /**
   * Removes all button bindings from the scheduler.
   */
  public void clearButtons() {
    m_buttons.clear();
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
  @SuppressWarnings({"PMD.CyclomaticComplexity", "PMD.NPathComplexity"})
  public void scheduleCommand(Command command, boolean interruptible) {

    if (CommandGroupBase.getGroupedCommands().contains(command)) {
      throw new IllegalUseOfCommandException(
          "A command that is part of a command group cannot be independently scheduled");
    }

    //Do nothing if the scheduler is disabled, the robot is disabled and the command doesn't
    //run when disabled, or the command is already scheduled.
    if (m_disabled
        || (RobotState.isDisabled() && !command.runsWhenDisabled())
        || m_scheduledCommands.containsKey(command)) {
      return;
    }

    Set<Subsystem> requirements = command.getRequirements();

    //Schedule the command if the requirements are not currently in-use.
    if (Collections.disjoint(m_requirements.keySet(), requirements)) {
      command.initialize();
      CommandState scheduledCommand = new CommandState(interruptible);
      m_scheduledCommands.put(command, scheduledCommand);
      for (Consumer<Command> action : m_initActions) {
        action.accept(command);
      }
      for (Subsystem requirement : requirements) {
        m_requirements.put(requirement, command);
      }
    } else {
      //Else check if the requirements that are in use have all have interruptible commands,
      //and if so, interrupt those commands and schedule the new command.
      boolean allInterruptible = true;
      for (Subsystem requirement : requirements) {
        if (m_requirements.keySet().contains(requirement)) {
          allInterruptible &=
              m_scheduledCommands.get(m_requirements.get(requirement)).isInterruptible();
        }
      }
      if (allInterruptible) {
        for (Subsystem requirement : requirements) {
          if (m_requirements.containsKey(requirement)) {
            cancelCommand(m_requirements.get(requirement));
          }
        }
        command.initialize();
        CommandState scheduledCommand = new CommandState(interruptible);
        m_scheduledCommands.put(command, scheduledCommand);
        for (Consumer<Command> action : m_initActions) {
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

    if (m_disabled) {
      return;
    }

    //Run the periodic method of all registered subsystems.
    for (Subsystem subsystem : m_subsystems) {
      subsystem.periodic();
    }

    //Poll buttons for new commands to add.
    for (ButtonScheduler button : m_buttons) {
      button.execute();
    }

    //Run scheduled commands, and remove if finished.
    for (Command command : m_scheduledCommands.keySet()) {

      if (RobotState.isDisabled() && !command.runsWhenDisabled()) {
        cancelCommand(command);
        continue;
      }

      command.execute();
      for (Consumer<Command> action : m_executeActions) {
        action.accept(command);
      }
      if (command.isFinished()) {
        command.end();
        for (Consumer<Command> action : m_endActions) {
          action.accept(command);
        }
        m_scheduledCommands.remove(command);
        m_requirements.keySet().removeAll(command.getRequirements());
      }
    }

    //Add default commands for un-required registered subsystems.
    for (Subsystem subsystem : m_subsystems) {
      if (!m_requirements.containsKey(subsystem) && subsystem.getDefaultCommand() != null) {
        scheduleCommand(subsystem.getDefaultCommand(), true);
      }
    }
  }

  /**
   * Registers a subsystem with the scheduler.  This must be called for the subsystem's periodic
   * block to run when the scheduler is run, and for the subsystem's default command to be
   * scheduled.  It is recommended to call this from the constructor of your subsystem
   * implementations.
   *
   * @param subsystem the subsystem to register
   */
  void registerSubsystem(Subsystem subsystem) {
    m_subsystems.add(subsystem);
  }

  /**
   * Un-registers a subsystem with the scheduler.  The subsystem will no longer have its periodic
   * block called, and will not have its default command scheduled.
   *
   * @param subsystem the subsystem to un-register
   */
  void unregisterSubsystem(Subsystem subsystem) {
    m_subsystems.remove(subsystem);
  }

  /**
   * Cancels a command.  The scheduler will only call the interrupted method of a canceled command,
   * not the end method.  Commands will be canceled even if they are not scheduled as interruptible.
   *
   * @param command the command to cancel
   */
  public void cancelCommand(Command command) {

    if (!m_scheduledCommands.containsKey(command)) {
      return;
    }

    command.interrupted();
    for (Consumer<Command> action : m_interruptActions) {
      action.accept(command);
    }
    m_scheduledCommands.remove(command);
    m_requirements.keySet().removeAll(command.getRequirements());
  }

  /**
   * Cancels all commands that are currently scheduled.
   */
  public void cancelAll() {
    for (Command command : m_scheduledCommands.keySet()) {
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
  public double timeSinceScheduled(Command command) {
    CommandState commandState = m_scheduledCommands.get(command);
    if (commandState != null) {
      return commandState.timeSinceInitialized();
    } else {
      return -1;
    }
  }

  /**
   * Whether a given command is running.  Note that this only works on commands that are directly
   * scheduled by the scheduler; it will not work on commands inside of CommandGroups, as the
   * scheduler does not see them.
   *
   * @param command the command to query
   * @return whether the command is currently scheduled
   */
  public boolean isScheduled(Command command) {
    return m_scheduledCommands.containsKey(command);
  }

  /**
   * Returns the command currently requiring a given subsystem.  Null if no command is currently
   * requiring the subsystem
   *
   * @param subsystem the subsystem to be inquired about
   * @return the command currently requiring the subsystem
   */
  public Command requiring(Subsystem subsystem) {
    return m_requirements.get(subsystem);
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
  public void onCommandInitialize(Consumer<Command> action) {
    m_initActions.add(action);
  }

  /**
   * Adds an action to perform on the execution of any command by the scheduler.
   *
   * @param action the action to perform
   */
  public void onCommandExecute(Consumer<Command> action) {
    m_executeActions.add(action);
  }

  /**
   * Adds an action to perform on the interruption of any command by the scheduler.
   *
   * @param action the action to perform
   */
  public void onCommandInterrupted(Consumer<Command> action) {
    m_interruptActions.add(action);
  }

  /**
   * Adds an action to perform on the ending of any command by the scheduler.
   *
   * @param action the action to perform
   */
  public void onCommandEnd(Consumer<Command> action) {
    m_endActions.add(action);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Scheduler");
    m_namesEntry = builder.getEntry("Names");
    m_idsEntry = builder.getEntry("Ids");
    m_cancelEntry = builder.getEntry("Cancel");
    builder.setUpdateTable(() -> {

      if (m_namesEntry == null || m_idsEntry == null || m_cancelEntry == null) {
        return;
      }

      Map<Double, Command> ids = new LinkedHashMap<>();


      for (Command command : m_scheduledCommands.keySet()) {
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
    });
  }
}
