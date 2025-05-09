// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import java.util.List;

public class CompileTestOptions {
  public static final int JAVA_VERSION = 17;
  public static final List<Object> JAVA_VERSION_OPTIONS =
      List.of("-source", JAVA_VERSION, "-target", JAVA_VERSION);
}
