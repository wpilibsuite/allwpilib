// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import java.util.function.BooleanSupplier;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.event.EventLoop;

/**
 * A Context represents a condition that determines when Trigger edges should be considered.
 *
 * <p>Contexts are used to scope when triggers are active. For example, joystick button triggers
 * might use {@link #allTeleop} to ensure they only fire during teleop mode, preventing accidental
 * input during autonomous.
 *
 * <p>Contexts can be composed using logical operations to create more complex conditions:
 *
 * <pre>{@code
 * Context myContext = Context.allTeleop.and(() -> robot.isReady());
 * Trigger myTrigger = new Trigger(myContext, controller.a()::get);
 * }</pre>
 *
 * <p>When a trigger's context evaluates to false, edge detection is paused and will resume when the
 * context becomes true again.
 */
public class Context implements BooleanSupplier {
  /** A context that is always true. */
  public static final Context all = new Context(() -> true);

  /** A context that is true when the robot is in autonomous mode (enabled or disabled). */
  public static final Context allAuto = all.and(DriverStation::isAutonomous);

  /** A context that is true when the robot is in teleop mode (enabled or disabled). */
  public static final Context allTeleop = all.and(DriverStation::isTeleop);

  /** A context that is true when the robot is in test mode (enabled or disabled). */
  public static final Context allTest = all.and(DriverStation::isTest);

  private final BooleanSupplier m_condition;

  /**
   * Creates a new Context with the given condition.
   *
   * <p>This constructor is protected and intended for internal use and subclassing. Use the static
   * factory methods or composition methods to create contexts.
   *
   * @param condition the boolean condition that determines when this context is active
   */
  protected Context(BooleanSupplier condition) {
    m_condition = condition;
  }

  /**
   * Creates a Trigger with this context and the given condition.
   *
   * <p>The resulting trigger will only fire edges when both this context and the provided condition
   * change, and only when this context is true.
   *
   * @param condition the trigger condition
   * @return a new Trigger that combines this context with the given condition
   */
  public Trigger trigger(BooleanSupplier condition) {
    return new Trigger(Scheduler.getDefault(), this, Conditions.and(this, condition));
  }

  /**
   * Creates a Trigger with this context, the given condition, and a specific event loop.
   *
   * @param loop the event loop to attach the trigger to
   * @param condition the trigger condition
   * @return a new Trigger that combines this context with the given condition
   */
  public Trigger trigger(EventLoop loop, BooleanSupplier condition) {
    return new Trigger(Scheduler.getDefault(), this, loop, Conditions.and(this, condition));
  }

  /**
   * Creates a new Context that is true when both this context and the given condition are true.
   *
   * @param condition the condition to combine with this context
   * @return a new Context representing the logical AND of this context and the condition
   */
  public Context and(BooleanSupplier condition) {
    return new Context(Conditions.and(this, condition));
  }

  /**
   * Creates a new Context that is true when either this context or the given context is true.
   *
   * @param condition the context to combine with this context
   * @return a new Context representing the logical OR of this context and the other context
   */
  public Context or(Context condition) {
    return new Context(Conditions.or(this, condition));
  }

  @Override
  public boolean getAsBoolean() {
    return m_condition.getAsBoolean();
  }
}
