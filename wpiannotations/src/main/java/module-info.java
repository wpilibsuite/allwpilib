// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/**
 * Defines standard annotations for use in WPILib-based projects. These annotations can be used to
 * opt into compile-time safety checks, such as by placing {@link org.wpilib.annotation.NoDiscard}
 * on a method to ensure you don't forget to use the return value.
 *
 * <p>The annotations in this module are processed by the WPILib javac plugin, which must be on the
 * annotation processor path to work. Robot programs must use {@code annotationProcessor
 * wpi.java.deps.wpilibAnnotations()} in their build.gradle files, which the WPILib and GradleRIO
 * templates include by default.
 */
open module wpilib.annotation {
  exports org.wpilib.annotation;
}
