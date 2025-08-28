// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a method as returning a value that must be used. The WPILib compiler plugin will check for
 * uses of methods with this annotation and report a compiler error if the value is unused. Marking
 * a class or interface as {@code @NoDiscard} will act as if any method that returns that type or
 * any subclass or implementor of that type has been marked with {@code @NoDiscard}.
 */
@Target({ElementType.METHOD, ElementType.TYPE})
@Retention(RetentionPolicy.SOURCE) // only needed at compile time
public @interface NoDiscard {
  /**
   * An error message to display if the return value is not used.
   *
   * @return The error message.
   */
  String value() default "";
}
