// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.annotation;

import java.lang.annotation.Target;

/**
 * Generates a mermaid graph for a state machine encapsulated within a method. Note that this
 * doesn't support multiple state machines per method.
 */
@Target(java.lang.annotation.ElementType.METHOD)
public @interface MakeStateMachineGraph {
  /**
   * An advanced configuration option that allows you to generate diagrams for a custom state
   * machine class sharing the same syntax as WPILib's state machine.
   *
   * @return The fully qualified name of the state machine class.
   */
  String stateMachineType() default "StateMachine";
}
