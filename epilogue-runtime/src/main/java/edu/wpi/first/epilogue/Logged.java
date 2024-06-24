// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue;

import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Place this annotation on a class to automatically log every field and every public accessor
 * method (methods with no arguments and return a loggable data type). Use {@link #strategy()} to
 * flag a class as logging everything it can, except for those elements tagged with
 * {@code @Logged(importance = NONE)}; or for logging only specific items also tagged with
 * {@code @Logged}.
 *
 * <p>Logged fields may have any access modifier. Logged methods must be public; non-public methods
 * will be ignored.
 *
 * <p>Epilogue can log all primitive types, arrays of primitive types (except char and short),
 * Strings, arrays of Strings, sendable objects, objects with a struct serializer, and arrays of
 * objects with struct serializers.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.FIELD, ElementType.METHOD, ElementType.TYPE})
public @interface Logged {
  /**
   * The name for the annotated element to be logged as. Does nothing on class-level annotations.
   * Fields and methods will default to be logged using their in-code names; use this attribute to
   * set it to something custom.
   *
   * <p>If the annotation is placed on a class, the specified name will not change logged data
   * (since that uses the names of the specific usages of the class in fields and methods); however,
   * it will be used to set the names of the generated logger that Logged will use to log instances
   * of the class. This can be used to avoid name conflicts if you have multiple classes with the
   * same name, but in different packages, and want to be able to log both.
   *
   * @return the name to use to log the field or method under; or the name of the generated
   *     class-specific logger
   */
  String name() default "";

  /** Opt-in or opt-out strategies for logging. */
  enum Strategy {
    /**
     * Log everything except for those elements explicitly opted out of with the skip = true
     * attribute. This is the default behavior.
     */
    OPT_OUT,

    /** Log only fields and methods tagged with an {@link Logged} annotation. */
    OPT_IN
  }

  /**
   * The strategy to use for logging. Only has an effect on annotations on class or interface
   * declarations.
   *
   * @return the strategy to use to determine which fields and methods in the class to log
   */
  Strategy strategy() default Strategy.OPT_OUT;

  /**
   * Data importance. Can be used at the class level to set the default importance for all data
   * points in the class, and can be used on individual fields and methods to set a specific
   * importance level overriding the class-level default.
   */
  enum Importance {
    /** Debug information. Useful for low-level information like raw sensor values. */
    DEBUG,

    /**
     * Informational data. Useful for higher-level information like pose estimates or subsystem
     * state.
     */
    INFO,

    /** Critical data that should always be present in logs. */
    CRITICAL
  }

  /**
   * The importance of the annotated data. If placed on a class or interface, this will be the
   * default importance of all data within that class; this can be overridden on a per-element basis
   * by annotating fields and methods with their own {@code @Logged(importance = ...)} annotation.
   *
   * @return the importance of the annotated element
   */
  Importance importance() default Importance.DEBUG;
}
