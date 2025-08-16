// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Objects;
import java.util.Set;

/**
 * An idle command that owns a mechanism without doing anything. It has the lowest possible priority,
 * and can be interrupted by any other command that requires the same mechanism. Cancellation and
 * interruption function like a normal command.
 */
public final class IdleCommand implements Command {
  private final Mechanism m_mechanism;

  /**
   * Creates a new idle command.
   *
   * @param mechanism the mechanism to idle.
   */
  public IdleCommand(Mechanism mechanism) {
    m_mechanism = requireNonNullParam(mechanism, "mechanism", "IdleCommand");
  }

  @Override
  public void run(Coroutine coroutine) {
    coroutine.park();
  }

  @Override
  public Set<Mechanism> requirements() {
    return Set.of(m_mechanism);
  }

  @Override
  public String name() {
    return m_mechanism.getName() + "[IDLE]";
  }

  @Override
  public int priority() {
    // lowest priority - an idle command can be interrupted by anything else
    return LOWEST_PRIORITY;
  }

  @Override
  public String toString() {
    return name();
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof IdleCommand other && Objects.equals(m_mechanism, other.m_mechanism);
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_mechanism);
  }
}
