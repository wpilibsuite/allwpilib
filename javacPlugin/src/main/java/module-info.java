// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/** Defines the WPILib javac plugin module for compile-time annotation-based checks. */
module wpilib.javac {
  requires transitive jdk.compiler;
  requires wpilib.annotation;

  exports org.wpilib.javacplugin;
}
