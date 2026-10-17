// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a method as needing to be called when overridden by a subclass. The WPILib compiler plugin
 * will check for uses of methods with this annotation and report a compiler error if the subclass
 * implementation does not call the superclass implementation.
 */
@Target({ElementType.METHOD})
@Retention(RetentionPolicy.CLASS) // needs to be stored in the class for use by libraries
public @interface CallSuper {
  /**
   * The string key to use in {@link SuppressWarnings} annotations to suppress compiler error
   * messages related to this annotation.
   */
  String SUPPRESSION_KEY = "WPILib.CallSuper";

  /**
   * An error message to display if the superclass method is not called.
   *
   * @return The error message.
   */
  String value() default "";
}
