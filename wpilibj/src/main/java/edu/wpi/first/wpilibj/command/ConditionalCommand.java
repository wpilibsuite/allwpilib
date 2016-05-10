/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import java.util.Enumeration;
import java.util.function.BooleanSupplier;

/**
 * A {@link ConditionalCommand} is a {@link Command} that starts one of two commands.
 *
 * <p>
 * A {@link ConditionalCommand} uses m_condition to determine whether it should run m_onTrue or
 * m_onFalse.
 * </p>
 *
 * <p>
 * A {@link ConditionalCommand} adds the proper {@link Command} to the {@link Scheduler} during
 * {@link ConditionalCommand#initialize()} and then {@link ConditionalCommand#isFinished()} will
 * return true once that {@link Command} has finished executing.
 * </p>
 *
 * <p>
 * If no {@link Command} is specified for m_onFalse, the occurrence of that condition will be a
 * no-op.
 * </p>
 *
 * @see Command
 * @see Scheduler
 */
public class ConditionalCommand extends Command {
  /**
   * The Command to execute if {@link ConditionalCommand#condition()} returns true.
   */
  private Command m_onTrue;

  /**
   * The Command to execute if {@link ConditionalCommand#condition()} returns false.
   */
  private Command m_onFalse;

  /**
   * Stores command chosen by condition.
   */
  private Command m_chosenCommand;

  private void requireAll() {
    if (m_onTrue != null) {
      for (Enumeration e = m_onTrue.getRequirements(); e.hasMoreElements(); ) {
        requires((Subsystem) e.nextElement());
      }
    }

    if (m_onFalse != null) {
      for (Enumeration e = m_onFalse.getRequirements(); e.hasMoreElements(); ) {
        requires((Subsystem) e.nextElement());
      }
    }
  }

  /**
   * The condition to use to determine which Command should be run.
   */
  private BooleanSupplier m_condition;

  /**
   * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
   *
   * <p>Users of this constructor should also override condition().
   *
   * @param onTrue The Command to execute if {@link ConditionalCommand#condition()} returns true
   */
  public ConditionalCommand(Command onTrue) {
    this(onTrue, null);
  }

  /**
   * Creates a new ConditionalCommand with given onTrue and onFalse Commands.
   *
   * <p>Users of this constructor should also override condition().
   *
   * @param onTrue The Command to execute if {@link ConditionalCommand#condition()} returns true
   * @param onFalse The Command to execute if {@link ConditionalCommand#condition()} returns false
   */
  public ConditionalCommand(Command onTrue, Command onFalse) {
    m_onTrue = onTrue;
    m_onFalse = onFalse;

    requireAll();
  }

  /**
   * Creates a new ConditionalCommand with given onTrue and onFalse Commands and condition.
   *
   * @param onTrue The Command to execute if {@link ConditionalCommand#condition()} returns true
   * @param onFalse The Command to execute if {@link ConditionalCommand#condition()} returns false
   * @param condition a BooleanSupplier that returns true if onTrue should be run
   */
  public ConditionalCommand(Command onTrue, Command onFalse, BooleanSupplier condition) {
    this.m_onTrue = onTrue;
    this.m_onFalse = onFalse;
    this.m_condition = condition;
  }

  /**
   * Creates a new ConditionalCommand with given name and onTrue and onFalse Commands.
   *
   * <p>Users of this constructor should also override condition().
   *
   * @param name the name for this command group
   * @param onTrue The Command to execute if {@link ConditionalCommand#condition()} returns true
   */
  public ConditionalCommand(String name, Command onTrue) {
    this(name, onTrue, null);
  }

  /**
   * Creates a new ConditionalCommand with given name and onTrue and onFalse Commands.
   *
   * <p>Users of this constructor should also override condition().
   *
   * @param name the name for this command group
   * @param onTrue The Command to execute if {@link ConditionalCommand#condition()} returns true
   * @param onFalse The Command to execute if {@link ConditionalCommand#condition()} returns false
   */
  public ConditionalCommand(String name, Command onTrue, Command onFalse) {
    super(name);
    m_onTrue = onTrue;
    m_onFalse = onFalse;

    requireAll();
  }

  /**
   * Creates a new ConditionalCommand with given name, onTrue and onFalse Commands, and condition.
   *
   * @param name the name for this command group
   * @param onTrue The Command to execute if {@link ConditionalCommand#condition()} returns true
   * @param onFalse The Command to execute if {@link ConditionalCommand#condition()} returns false
   * @param condition a BooleanSupplier that returns true if onTrue should be run
   */
  public ConditionalCommand(String name, Command onTrue, Command onFalse,
                            BooleanSupplier condition) {
    super(name);
    this.m_onTrue = onTrue;
    this.m_onFalse = onFalse;
    this.m_condition = condition;
  }

  /**
   * The Condition to test to determine which Command to run.
   *
   * @return true if m_onTrue should be run or false if m_onFalse should be run.
   */
  protected boolean condition() {
    if (m_condition == null) {
      throw new IllegalArgumentException(
          "Either override condition() or pass a predicate to the constructor");
    }
    return m_condition.getAsBoolean();
  }

  /**
   * Calls {@link ConditionalCommand#condition()} and runs the proper command.
   */
  @Override
  protected void _initialize() {
    if (condition()) {
      m_chosenCommand = m_onTrue;
    } else {
      m_chosenCommand = m_onFalse;
    }

    if (m_chosenCommand != null) {
      /*
       * This is a hack to make cancelling the chosen command inside a
       * CommandGroup work properly
       */
      m_chosenCommand.clearRequirements();

      m_chosenCommand.start();
    }
    super._initialize();
  }

  @Override
  protected synchronized void _cancel() {
    if (m_chosenCommand != null && m_chosenCommand.isRunning()) {
      m_chosenCommand.cancel();
    }

    super._cancel();
  }

  @Override
  protected boolean isFinished() {
    if (m_chosenCommand != null) {
      return m_chosenCommand.isCompleted();
    } else {
      return true;
    }
  }

  @Override
  protected void _interrupted() {
    if (m_chosenCommand != null && m_chosenCommand.isRunning()) {
      m_chosenCommand.cancel();
    }

    super._interrupted();
  }
}
