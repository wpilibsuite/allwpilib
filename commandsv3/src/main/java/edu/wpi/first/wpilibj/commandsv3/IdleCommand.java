// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Objects;
import java.util.Set;

/**
 * An idle command that owns a resource without doing anything. It has the lowest possible priority,
 * and can be interrupted by any other command that requires the same resource. Cancellation and
 * interruption function like a normal command.
 */
public final class IdleCommand implements Command {
  private final RequireableResource resource;

  /**
   * @param resource the resource to idle.
   */
  public IdleCommand(RequireableResource resource) {
    this.resource = requireNonNullParam(resource, "resource", "IdleCommand");
  }

  @Override
  public void run(Coroutine coroutine) {
    coroutine.park();
  }

  @Override
  public Set<RequireableResource> requirements() {
    return Set.of(resource);
  }

  @Override
  public String name() {
    return resource.getName() + "[IDLE]";
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
  public RobotDisabledBehavior robotDisabledBehavior() {
    return RobotDisabledBehavior.RunWhileDisabled;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof IdleCommand other && Objects.equals(this.resource, other.resource);
  }

  @Override
  public int hashCode() {
    return Objects.hash(resource);
  }
}
