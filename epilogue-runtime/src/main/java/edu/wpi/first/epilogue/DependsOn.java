// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue;

import java.lang.annotation.ElementType;
import java.lang.annotation.Repeatable;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks a logged field or method as dependent on other data points. This annotation is used to
 * provide metadata for analysis tools like AdvantageScope, allowing them to understand
 * relationships between different logged values.
 *
 * <p>This annotation is particularly useful for triggers and other composite conditions where
 * understanding the component values is important for debugging.
 *
 * <pre><code>
 * {@literal @}Logged
 * class Arm {
 *   {@literal @}DependsOn("getPosition()")
 *   {@literal @}DependsOn("getTargetPosition()")
 *   {@literal @}DependsOn("getTolerance()")
 *   public final Trigger exampleTrigger =
 *     new Trigger(() -> getPosition().isNear(getTargetPosition(), getTolerance()));
 *
 *   public Angle getPosition() { ... }
 *   public Angle getTargetPosition() { ... }
 *   public Angle getTolerance() { ... }
 * }
 * </code></pre>
 */
@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.FIELD, ElementType.METHOD})
@Repeatable(DependsOn.Container.class)
public @interface DependsOn {
  /**
   * The name of the method or field that this item depends on. This should match the actual name of
   * a method or field in the same class that provides data relevant to understanding this item.
   *
   * <p>For methods, include the parentheses (e.g., "getPosition()"). For fields, use just the field
   * name (e.g., "position").
   *
   * @return the name of the dependency
   */
  String value();

  /** Container annotation for multiple {@link DependsOn} annotations. */
  @Retention(RetentionPolicy.RUNTIME)
  @Target({ElementType.FIELD, ElementType.METHOD})
  @interface Container {
    /**
     * Array of {@link DependsOn} annotations.
     *
     * @return the array of dependencies
     */
    DependsOn[] value();
  }
}
