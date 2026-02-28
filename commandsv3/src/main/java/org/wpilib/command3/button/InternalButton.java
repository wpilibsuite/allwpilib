// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import java.util.concurrent.atomic.AtomicBoolean;
import org.wpilib.command3.Context;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;

/**
 * This class is intended to be used within a program. The programmer can manually set its value.
 * Also includes a setting for whether it should invert its value.
 */
public class InternalButton extends Trigger {
  // need to be references, so they can be mutated after being captured in the constructor.
  private final AtomicBoolean m_pressed;
  private final AtomicBoolean m_inverted;

  /**
   * Creates an InternalButton with the given scheduler and context.
   *
   * @param scheduler The scheduler that should execute triggered commands.
   * @param context The context that must be true for edges to be considered
   * @param inverted if false, then this button is pressed when set to true, otherwise it is pressed
   *     when set to false.
   */
  public InternalButton(Scheduler scheduler, Context context, boolean inverted) {
    this(scheduler, context, new AtomicBoolean(), new AtomicBoolean(inverted));
  }

  /**
   * Creates an InternalButton that is not inverted. Uses the default scheduler and universal
   * context.
   */
  public InternalButton() {
    this(false);
  }

  /**
   * Creates an InternalButton which is inverted depending on the input. Uses the default scheduler
   * and universal context.
   *
   * @param inverted if false, then this button is pressed when set to true, otherwise it is pressed
   *     when set to false.
   */
  public InternalButton(boolean inverted) {
    this(Context.all, inverted);
  }

  /**
   * Creates an InternalButton with the given context and inversion setting. Uses the default
   * scheduler.
   *
   * @param context The context that must be true for edges to be considered
   * @param inverted if false, then this button is pressed when set to true, otherwise it is pressed
   *     when set to false.
   */
  public InternalButton(Context context, boolean inverted) {
    this(Scheduler.getDefault(), context, inverted);
  }

  /*
   * Mock constructor so the AtomicBoolean objects can be constructed before the super
   * constructor invocation.
   */
  private InternalButton(
      Scheduler scheduler, Context context, AtomicBoolean state, AtomicBoolean inverted) {
    super(scheduler, context, () -> state.get() != inverted.get());
    m_pressed = state;
    m_inverted = inverted;
  }

  /**
   * Sets whether to invert button state.
   *
   * @param inverted Whether button state should be inverted.
   */
  public void setInverted(boolean inverted) {
    m_inverted.set(inverted);
  }

  /**
   * Sets whether button is pressed.
   *
   * @param pressed Whether button is pressed.
   */
  public void setPressed(boolean pressed) {
    m_pressed.set(pressed);
  }
}
