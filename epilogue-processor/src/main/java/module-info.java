// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

import javax.annotation.processing.Processor;

open module wpilib.epilogue.processor {
  requires java.compiler;
  requires jdk.compiler;
  requires wpilib.epilogue;

  provides Processor with
      edu.wpi.first.epilogue.processor.AnnotationProcessor;
}
