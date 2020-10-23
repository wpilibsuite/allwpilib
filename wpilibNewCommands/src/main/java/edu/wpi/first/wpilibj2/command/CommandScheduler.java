/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
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
import edu.wpi.first.wpilibj.RobotBase;
import edu.wpi.first.wpilibj.RobotState;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.Watchdog;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * The scheduler responsible for running {@link Command}s.  A Command-based robot should call {@link
 * CommandScheduler#run()} on the singleton instance in its periodic block in order to run commands
 * synchronously from the main loop.  Subsystems should be registered with the scheduler using
 * {@link CommandScheduler#registerSubsystem(Subsystem...)} in order for their {@link
 * Subsystem#periodic()} methods to be called and for their default commands to be scheduled.
 */
@SuppressWarnings({"PMD.GodClass", "PMD.TooManyFields"})
public final class CommandScheduler implements Sendable, AutoCloseable {
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

  //A map from subsystems registered with the scheduler to their default commands.  Also used
  //as a list of currently-registered subsystems.
  private final Map<Subsystem, Command> m_subsystems = new LinkedHashMap<>();

  //The set of currently-registered buttons that will be polled every iteration.
  private final Collection<Runnable> m_buttons = new LinkedHashSet<>();

  private boolean m_disabled;

  //Lists of user-supplied actions to be executed on scheduling events for every command.
  private final List<Consumer<Command>> m_initActions = new ArrayList<>();
  private final List<Consumer<Command>> m_executeActions = new ArrayList<>();
  private final List<Consumer<Command>> m_interruptActions = new ArrayList<>();
  private final List<Consumer<Command>> m_finishActions = new ArrayList<>();

  // Flag and queues for avoiding ConcurrentModificationException if commands are
  // scheduled/canceled during run
  private boolean m_inRunLoop;
  private final Map<Command, Boolean> m_toSchedule = new LinkedHashMap<>();
  private final List<Command> m_toCancel = new ArrayList<>();

  private final Watchdog m_watchdog = new Watchdog(TimedRobot.kDefaultPeriod, () -> { });

  CommandScheduler() {
    HAL.report(tResourceType.kResourceType_Command, tInstances.kCommand2_Scheduler);
    SendableRegistry.addLW(this, "Scheduler");
    LiveWindow.setEnabledListener(() -> {
      disable();
      cancelAll();
    });
    LiveWindow.setDisabledListener(() -> {
      enable();
    });
  }

  /**
   * Changes the period of the loop overrun watchdog.  This should be be kept in sync with the
   * TimedRobot period.
   *
   * @param period Period in seconds.
   */
  public void setPeriod(double period) {
    m_watchdog.setTimeout(period);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    LiveWindow.setEnabledListener(null);
    LiveWindow.setDisabledListener(null);
  }

  /**
   * Adds a button binding to the scheduler, which will be polled to schedule commands.
   *
   * @param button The button to add
   */
  public void addButton(Runnable button) {
    m_buttons.add(button);
  }

  /**
   * Removes all button bindings from the scheduler.
   */
  public void clearButtons() {
    m_buttons.clear();
  }

  /**
   * Initializes a given command, adds its requirements to the list, and performs the init actions.
   *
   * @param command       The command to initialize
   * @param interruptible Whether the command is interruptible
   * @param requirements  The command requirements
   */
  private void initCommand(Command command, boolean interruptible, Set<Subsystem> requirements) {
    CommandState scheduledCommand = new CommandState(interruptible);
    m_scheduledCommands.put(command, scheduledCommand);
    command.initialize();
    for (Subsystem requirement : requirements) {
      m_requirements.put(requirement, command);
    }
    for (Consumer<Command> action : m_initActions) {
      action.accept(command);
    }

    m_watchdog.addEpoch(command.getName() + ".initialize()");
  }

  /**
   * Schedules a command for execution.  Does nothing if the command is already scheduled. If a
   * command's requirements are not available, it will only be started if all the commands currently
   * using those requirements have been scheduled as interruptible.  If this is the case, they will
   * be interrupted and the command will be scheduled.
   *
   * @param interruptible whether this command can be interrupted
   * @param command       the command to schedule
   */
  @SuppressWarnings({"PMD.CyclomaticComplexity", "PMD.NPathComplexity"})
  private void schedule(boolean interruptible, Command command) {
    if (m_inRunLoop) {
      m_toSchedule.put(command, interruptible);
      return;
    }

    if (CommandGroupBase.getGroupedCommands().contains(command)) {
      throw new IllegalArgumentException(
          "A command that is part of a command group cannot be independently scheduled");
    }

    //Do nothing if the scheduler is disabled, the robot is disabled and the command doesn't
    //run when disabled, or the command is already scheduled.
    if (m_disabled || (RobotState.isDisabled() && !command.runsWhenDisabled())
        || m_scheduledCommands.containsKey(command)) {
      return;
    }

    Set<Subsystem> requirements = command.getRequirements();

    //Schedule the command if the requirements are not currently in-use.
    if (Collections.disjoint(m_requirements.keySet(), requirements)) {
      initCommand(command, interruptible, requirements);
    } else {
      //Else check if the requirements that are in use have all have interruptible commands,
      //and if so, interrupt those commands and schedule the new command.
      for (Subsystem requirement : requirements) {
        if (m_requirements.containsKey(requirement)
            && !m_scheduledCommands.get(m_requirements.get(requirement)).isInterruptible()) {
          return;
        }
      }
      for (Subsystem requirement : requirements) {
        if (m_requirements.containsKey(requirement)) {
          cancel(m_requirements.get(requirement));
        }
      }
      initCommand(command, interruptible, requirements);
    }
  }

  /**
   * Schedules multiple commands for execution.  Does nothing if the command is already scheduled.
   * If a command's requirements are not available, it will only be started if all the commands
   * currently using those requirements have been scheduled as interruptible.  If this is the case,
   * they will be interrupted and the command will be scheduled.
   *
   * @param interruptible whether the commands should be interruptible
   * @param commands      the commands to schedule
   */
  public void schedule(boolean interruptible, Command... commands) {
    for (Command command : commands) {
      schedule(interruptible, command);
    }
  }

  /**
   * Schedules multiple commands for execution, with interruptible defaulted to true.  Does nothing
   * if the command is already scheduled.
   *
   * @param commands the commands to schedule
   */
  public void schedule(Command... commands) {
    schedule(true, commands);
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
    m_watchdog.reset();

    //Run the periodic method of all registered subsystems.
    for (Subsystem subsystem : m_subsystems.keySet()) {
      subsystem.periodic();
      if (RobotBase.isSimulation()) {
        subsystem.simulationPeriodic();
      }
      m_watchdog.addEpoch(subsystem.getClass().getSimpleName() + ".periodic()");
    }

    //Poll buttons for new commands to add.
    for (Runnable button : m_buttons) {
      button.run();
    }
    m_watchdog.addEpoch("buttons.run()");

    m_inRunLoop = true;
    //Run scheduled commands, remove finished commands.
    for (Iterator<Command> iterator = m_scheduledCommands.keySet().iterator();
         iterator.hasNext(); ) {
      Command command = iterator.next();

      if (!command.runsWhenDisabled() && RobotState.isDisabled()) {
        command.end(true);
        for (Consumer<Command> action : m_interruptActions) {
          action.accept(command);
        }
        m_requirements.keySet().removeAll(command.getRequirements());
        iterator.remove();
        m_watchdog.addEpoch(command.getName() + ".end(true)");
        continue;
      }

      command.execute();
      for (Consumer<Command> action : m_executeActions) {
        action.accept(command);
      }
      m_watchdog.addEpoch(command.getName() + ".execute()");
      if (command.isFinished()) {
        command.end(false);
        for (Consumer<Command> action : m_finishActions) {
          action.accept(command);
        }
        iterator.remove();

        m_requirements.keySet().removeAll(command.getRequirements());
        m_watchdog.addEpoch(command.getName() + ".end(false)");
      }
    }
    m_inRunLoop = false;

    //Schedule/cancel commands from queues populated during loop
    for (Map.Entry<Command, Boolean> commandInterruptible : m_toSchedule.entrySet()) {
      schedule(commandInterruptible.getValue(), commandInterruptible.getKey());
    }

    for (Command command : m_toCancel) {
      cancel(command);
    }

    m_toSchedule.clear();
    m_toCancel.clear();

    //Add default commands for un-required registered subsystems.
    for (Map.Entry<Subsystem, Command> subsystemCommand : m_subsystems.entrySet()) {
      if (!m_requirements.containsKey(subsystemCommand.getKey())
          && subsystemCommand.getValue() != null) {
        schedule(subsystemCommand.getValue());
      }
    }

    m_watchdog.disable();
    if (m_watchdog.isExpired()) {
      System.out.println("CommandScheduler loop overrun");
      m_watchdog.printEpochs();
    }
  }

  /**
   * Registers subsystems with the scheduler.  This must be called for the subsystem's periodic
   * block to run when the scheduler is run, and for the subsystem's default command to be
   * scheduled.  It is recommended to call this from the constructor of your subsystem
   * implementations.
   *
   * @param subsystems the subsystem to register
   */
  public void registerSubsystem(Subsystem... subsystems) {
    for (Subsystem subsystem : subsystems) {
      m_subsystems.put(subsystem, null);
    }
  }

  /**
   * Un-registers subsystems with the scheduler.  The subsystem will no longer have its periodic
   * block called, and will not have its default command scheduled.
   *
   * @param subsystems the subsystem to un-register
   */
  public void unregisterSubsystem(Subsystem... subsystems) {
    m_subsystems.keySet().removeAll(Set.of(subsystems));
  }

  /**
   * Sets the default command for a subsystem.  Registers that subsystem if it is not already
   * registered.  Default commands will run whenever there is no other command currently scheduled
   * that requires the subsystem.  Default commands should be written to never end (i.e. their
   * {@link Command#isFinished()} method should return false), as they would simply be re-scheduled
   * if they do.  Default commands must also require their subsystem.
   *
   * @param subsystem      the subsystem whose default command will be set
   * @param defaultCommand the default command to associate with the subsystem
   */
  public void setDefaultCommand(Subsystem subsystem, Command defaultCommand) {
    if (!defaultCommand.getRequirements().contains(subsystem)) {
      throw new IllegalArgumentException("Default commands must require their subsystem!");
    }

    if (defaultCommand.isFinished()) {
      throw new IllegalArgumentException("Default commands should not end!");
    }

    m_subsystems.put(subsystem, defaultCommand);
  }

  /**
   * Gets the default command associated with this subsystem.  Null if this subsystem has no default
   * command associated with it.
   *
   * @param subsystem the subsystem to inquire about
   * @return the default command associated with the subsystem
   */
  public Command getDefaultCommand(Subsystem subsystem) {
    return m_subsystems.get(subsystem);
  }

  /**
   * Cancels commands. The scheduler will only call {@link Command#end(boolean)} method
   * of the canceled command with {@code true},
   * indicating they were canceled (as opposed to finishing normally).
   *
   * <p>Commands will be canceled even if they are not scheduled as interruptible.
   *
   * @param commands the commands to cancel
   */
  public void cancel(Command... commands) {
    if (m_inRunLoop) {
      m_toCancel.addAll(List.of(commands));
      return;
    }

    for (Command command : commands) {
      if (!m_scheduledCommands.containsKey(command)) {
        continue;
      }

      command.end(true);
      for (Consumer<Command> action : m_interruptActions) {
        action.accept(command);
      }
      m_scheduledCommands.remove(command);
      m_requirements.keySet().removeAll(command.getRequirements());
      m_watchdog.addEpoch(command.getName() + ".end(true)");
    }
  }

  /**
   * Cancels all commands that are currently scheduled.
   */
  public void cancelAll() {
    for (Command command : m_scheduledCommands.keySet().toArray(new Command[0])) {
      cancel(command);
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
   * Whether the given commands are running.  Note that this only works on commands that are
   * directly scheduled by the scheduler; it will not work on commands inside of CommandGroups, as
   * the scheduler does not see them.
   *
   * @param commands the command to query
   * @return whether the command is currently scheduled
   */
  public boolean isScheduled(Command... commands) {
    return m_scheduledCommands.keySet().containsAll(Set.of(commands));
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
   * Disables the command scheduler.
   */
  public void disable() {
    m_disabled = true;
  }

  /**
   * Enables the command scheduler.
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
  public void onCommandInterrupt(Consumer<Command> action) {
    m_interruptActions.add(action);
  }

  /**
   * Adds an action to perform on the finishing of any command by the scheduler.
   *
   * @param action the action to perform
   */
  public void onCommandFinish(Consumer<Command> action) {
    m_finishActions.add(action);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Scheduler");
    final NetworkTableEntry namesEntry = builder.getEntry("Names");
    final NetworkTableEntry idsEntry = builder.getEntry("Ids");
    final NetworkTableEntry cancelEntry = builder.getEntry("Cancel");
    builder.setUpdateTable(() -> {

      if (namesEntry == null || idsEntry == null || cancelEntry == null) {
        return;
      }

      Map<Double, Command> ids = new LinkedHashMap<>();


      for (Command command : m_scheduledCommands.keySet()) {
        ids.put((double) command.hashCode(), command);
      }

      double[] toCancel = cancelEntry.getDoubleArray(new double[0]);
      if (toCancel.length > 0) {
        for (double hash : toCancel) {
          cancel(ids.get(hash));
          ids.remove(hash);
        }
        cancelEntry.setDoubleArray(new double[0]);
      }

      List<String> names = new ArrayList<>();

      ids.values().forEach(command -> names.add(command.getName()));

      namesEntry.setStringArray(names.toArray(new String[0]));
      idsEntry.setNumberArray(ids.keySet().toArray(new Double[0]));
    });
  }
}
