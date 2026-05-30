package org.wpilib.annotation;

import java.lang.annotation.Target;

/**
 * Generates a mermaid graph for a state machine encapsulated within a method.
 * Note that this doesn't support multiple state machines per method.
 */
@Target(java.lang.annotation.ElementType.METHOD)
public @interface MakeStateMachineGraph {}