// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import java.util.List;

public class CompileTestOptions {
  public static final int kJavaVersion = 17;
  public static final List<Object> kJavaVersionOptions =
      List.of("-source", kJavaVersion, "-target", kJavaVersion);
}
