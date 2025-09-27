// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.opmode;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/** Annotation for automatic registration of teleoperated opmode classes. */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
@Documented
public @interface Teleoperated {
  /**
   * Name. This is shown as the selection name in the Driver Station, and must be unique across all
   * teleoperated opmodes in the project. If not specified, defaults to the name of the class.
   */
  String name() default "";

  /**
   * Group. All opmodes with the same group are grouped together for selection. If not specified,
   * defaults to ungrouped.
   */
  String group() default "";

  /** Extended description. Optional. */
  String description() default "";

  /**
   * Text color. Optional. Supports all formats supported by {@link
   * edu.wpi.first.wpilibj.util.Color#fromString(String)}.
   */
  String textColor() default "";

  /**
   * Text background color. Optional. Supports all formats supported by {@link
   * edu.wpi.first.wpilibj.util.Color#fromString(String)}.
   */
  String backgroundColor() default "";
}
