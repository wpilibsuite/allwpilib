// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.opmode;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/** Annotation for automatic registration of test opmode classes. */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
@Documented
public @interface TestOpMode {
  /**
   * Name. This is shown as the selection name in the Driver Station, and must be unique across all
   * test opmodes in the project. If not specified, defaults to the name of the class.
   *
   * @return Name
   */
  String name() default "";

  /**
   * Group. All opmodes with the same group are grouped together for selection. If not specified,
   * defaults to ungrouped.
   *
   * @return Group
   */
  String group() default "";

  /**
   * Extended description. Optional.
   *
   * @return Description
   */
  String description() default "";

  /**
   * Text color. Optional. Supports all formats supported by {@link
   * org.wpilib.util.Color#fromString(String)}.
   *
   * @return Text color
   */
  String textColor() default "";

  /**
   * Text background color. Optional. Supports all formats supported by {@link
   * org.wpilib.util.Color#fromString(String)}.
   *
   * @return Text background color
   */
  String backgroundColor() default "";
}
