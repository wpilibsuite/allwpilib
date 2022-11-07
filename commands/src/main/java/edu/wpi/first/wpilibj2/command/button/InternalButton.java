// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * This class is intended to be used within a program. The programmer can manually set its value.
 * Also includes a setting for whether or not it should invert its value.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
@SuppressWarnings("deprecation")
public class InternalButton extends Button {
  // need to be references, so they can be mutated after being captured in the constructor.
  private final AtomicBoolean m_pressed;
  private final AtomicBoolean m_inverted;

  /** Creates an InternalButton that is not inverted. */
  public InternalButton() {
    this(false);
  }

  /**
   * Creates an InternalButton which is inverted depending on the input.
   *
   * @param inverted if false, then this button is pressed when set to true, otherwise it is pressed
   *     when set to false.
   */
  public InternalButton(boolean inverted) {
    this(new AtomicBoolean(), new AtomicBoolean(inverted));
  }

  /*
   * Mock constructor so the AtomicBoolean objects can be constructed before the super
   * constructor invocation.
   */
  private InternalButton(AtomicBoolean state, AtomicBoolean inverted) {
    super(() -> state.get() != inverted.get());
    this.m_pressed = state;
    this.m_inverted = inverted;
  }

  public void setInverted(boolean inverted) {
    m_inverted.set(inverted);
  }

  public void setPressed(boolean pressed) {
    m_pressed.set(pressed);
  }
}
